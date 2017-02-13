#include <algorithm>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/ip.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "err.h"
#include "helpers.h"

#define BUF_SIZE 1000

	/* Variables that define the exact job for our program */
	char *host;
	char *path;
	char *r_port;
	char *filename;
	short m_port;
	bool md;
	bool save_to_file;
	
	FILE *streaming_file;
	
	/* Variables related to the web connection */
	struct pollfd fds[2];		// listening on fds[0],
								// reading radio on fds[1]
	struct sockaddr_in listen_address;
	struct sockaddr_in respond_address;
	socklen_t rcva_len;
	
	struct addrinfo radio_hints;
	struct addrinfo *radio_address;
	int ret, rval;
	
	
	char buffer[BUF_SIZE + 1];
	int metaint;
	
	/* Radio-related variables */
	bool playing;
	bool finished;
	std::string current_title;

/*
 * Parses out the call parameters. On error,
 * exits program with error code 1 
 */
void parse_parameters(int argc, char *argv[]) {
	if (argc != 7)
		usage_err();
	host = argv[1];
	path = argv[2];
	filename = argv[4];
	save_to_file = strcmp(filename, "-") != 0;
	r_port = argv[3];
	m_port = parse_port_num(argv[5]);
	if (r_port < 0 || m_port < 0)
		fatal("Wrong parameter");
	if (strcmp(argv[6], "yes") == 0)
		md = 1;
	else if (strcmp(argv[6], "no") == 0)
		md = 0;
	else
		fatal("Wrong parameter");
			
	#ifdef DEBUG
	/* Parameters check*/
	std::cout << "Host name: " << host << std::endl;
	std::cout << "Path: " << path << std::endl;
	std::cout << "Reading from port: " << r_port << std::endl;
	std::cout << "Saving to file: " << (save_to_file ? filename : "no") << std::endl;
	std::cout << "Listening on port: " << m_port << std::endl;
	std::cout << "Demanding metadata? " << (md ? "yes" : "no") << std::endl;
	#endif

}

void open_file() {
	streaming_file = fopen(filename, "wb");
}

void close_file() {
	if (fclose(streaming_file) != 0)
		fatal("closing file");
}

/*
 * Creates a UDP socket for listening for commands,
 * binds it to a local address. On error, exits program
 * (syserr()) with an appropriate message 
 */
void create_and_bind_udp() {
	fds[0].fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fds[0].fd < 0)
		syserr("socket");
	listen_address = { 0 };
	listen_address.sin_family = AF_INET;
	listen_address.sin_addr.s_addr = htonl(INADDR_ANY);	
	listen_address.sin_port = htons(m_port);
	if (bind(
			fds[0].fd,
			(struct sockaddr*) &listen_address,
			(socklen_t) sizeof(listen_address)
			) < 0)
		syserr("bind");
}

/*
 * Creates a TCP socket and connects it to 
 * the specified server. On error, exits
 * the program with code 1
 */
void create_and_connect_tcp() {
	memset(&radio_hints, 0, sizeof(struct addrinfo));
	radio_hints.ai_family = AF_INET;
	radio_hints.ai_socktype = SOCK_STREAM;
	radio_hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(host, r_port, &radio_hints, &radio_address) != 0)
		fatal("getaddrinfo");
	fds[1].fd = socket(
				radio_address->ai_family,
				radio_address->ai_socktype,
				radio_address->ai_protocol
				);
	if (fds[1].fd < 0)
		fatal("socket");
	if (connect(fds[1].fd, radio_address->ai_addr, radio_address->ai_addrlen) < 0)
		fatal("connect");
	freeaddrinfo(radio_address);

}

/*
 * Sends the get request to the server
 */

void send_get_message() {
	std::string get = "GET ";
	get += std::string(path);
	get += " HTTP/1.0\r\nIcy-MetaData:";
	get += (md ? "1" : "0");
	get += "\r\n\r\n";

	if (write(fds[1].fd, get.c_str(), strlen(get.c_str())) < 0)
		syserr("write");
}

/*
 * Returns the header received from the stream
 */
std::string get_header() {
	std::string header = "";
	int received;
	char buf[2];
	for (;;) {
		memset(buf, 0, sizeof(buf));
		received = read(fds[1].fd, buf, sizeof(buf) - 1);
		if (received < 0)
			syserr("read");
		header += std::string(buf);
		if (endswith(header, "\n\n") || endswith(header, "\r\n\r\n"))
			return header;
	}
}


/*
 * Streams *metaint* bytes from the server
 * (if we demand metadata). Otherwise, streams
 * BUF_SIZE bytes
 */
void stream_mp3() {
	int to_read = (md ? metaint : BUF_SIZE);
	int received;
	char buf[BUF_SIZE + 1];
	while (to_read > 0) {
		memset(buf, 0, BUF_SIZE + 1);
		received = read(fds[1].fd, buf, std::min(BUF_SIZE, to_read));
		if (received < 0)
			fatal("read");
		to_read -= received;
		if (playing) {
			if (save_to_file) {
				fwrite(buf, 1, received, streaming_file);
			} else {
				std::cout.write(buf, received);
			}
		}
	}
}
/*
 * Parses the title mentioned in the metadata
 */
std::string parse_title(std::string const &metadata) {
	std::string opening = "StreamTitle=\'";
	std::string closing = "\';";
	std::size_t start = metadata.find(opening);
	if (start == std::string::npos)
		return "unknown";
	std::size_t end = metadata.find(closing, start);
	if (end == std::string::npos)
		return "unknown";
	size_t len = end - start - opening.length();
	return metadata.substr(start + opening.length(), len);
}
/*
 * Reads metadata from the server
 */
void stream_metadata() {
	uint8_t metabyte;
	int received, to_read;
	char buf[BUF_SIZE + 1];
	std::string metadata = "";
	while (read(fds[1].fd, &metabyte, sizeof(metabyte)) <= 0);
	to_read = ((int) metabyte) * 16;
	int tmp = to_read;
	while (to_read > 0) {
		memset(buf, 0, BUF_SIZE + 1);
		received = read(fds[1].fd, buf, std::min(BUF_SIZE, to_read));
		to_read -= received;
		metadata += std::string(buf);
	}
	if (tmp > 0) {
		current_title = parse_title(metadata);
	}
}

/*
 * Closes the two created sockets
 */
void close_sockets() {
	if (close(fds[0].fd) < 0 || close(fds[1].fd) < 0)
		perror("Closing socket");
}

/*
 * The following few functions form
 * a control panel for the radio 
 */
void radio_init() {
	playing = true;
	finished = false;
	#ifdef DEBUG
	std::cerr << "initialising the radio" << std::endl;
	#endif
}

namespace radio {
	void play() {
		playing = true;
		#ifdef DEBUG
		std::cerr << "PLAY clicked" << std::endl;
		#endif
	}

	void pause() {
		playing = false;
		#ifdef DEBUG
		std::cerr << "PAUSE clicked" << std::endl;
		#endif
	}

	void quit() {
		finished = true;
		#ifdef DEBUG
		std::cerr << "POWER OFF clicked" << std::endl;
		#endif
	}
	
	void send_title() {
		std::cerr << "Sending title: " << current_title << std::endl;
		int snd_len = sendto(fds[0].fd,
					current_title.c_str(), 
					(size_t) current_title.length(),
					0,
					(struct sockaddr *) &respond_address,
					(socklen_t) sizeof(respond_address));
		if (snd_len < 0)
			fatal("sendto");
	}

	void ignore() {
		#ifdef DEBUG
		std::cerr << "command ignored" << std::endl;
		#endif
	}
}
/*
 * Decides whether received command stands
 * for something and does the job accordingly
 */
 
void parse_command(char *command) {
	if (strcmp(command, "PLAY") == 0) {
		radio::play();
	} else if (strcmp(command, "PAUSE") == 0) {
		radio::pause();
	} else if (strcmp(command, "QUIT") == 0) {
		radio::quit();
	} else if (strcmp(command, "TITLE") == 0) {
		radio::send_title();
	} else {
		radio::ignore();
	}
}

int main(int argc, char *argv[]) {
	/* Parsing out the aruments */
	parse_parameters(argc, argv);

	rcva_len = (socklen_t) sizeof(respond_address);

	/* First, the command-listening server (UDP) */
	create_and_bind_udp();

	/* Second, the socket for receiving data from the radio server */
	create_and_connect_tcp();
	send_get_message();
	if (save_to_file)
		open_file();
	std::string header = get_header();
	#ifdef DEBUG
	std::cout << header;
	#endif
	if (md) {
		metaint = parse_value(header, "icy-metaint");
	}

	/* Now, doing the actual job */
	fds[0].events = fds[1].events = POLLIN | POLLERR;
	radio_init();
	do {
		fds[0].revents = fds[1].revents = 0;
		memset(buffer, 0, BUF_SIZE + 1);
		ret = poll(fds, 2, -1);
		if (ret < 0) {
			perror("poll");
		} else if (ret > 0) {
			if (fds[0].revents & (POLLIN | POLLERR)) { // a command
				rval = recvfrom(
					fds[0].fd,
					buffer,
					BUF_SIZE,
					0,
					(struct sockaddr *) &respond_address,
					&rcva_len
					);
				if (rval < 0) {
					syserr("datagram");	
				} else {
					parse_command(buffer);
				}
			}
			if (fds[1].revents & (POLLIN | POLLERR)) { // data from the radio server
				stream_mp3();
				if (md) {
					stream_metadata();
				}
			}
		}
	} while (!finished);
	if (save_to_file)
		close_file();
	close_sockets();
	return 0;
}
