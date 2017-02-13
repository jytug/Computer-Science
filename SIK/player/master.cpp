#include <algorithm>
#include <arpa/inet.h>
#include <cstdio>
#include <stdlib.h>
#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <libssh2.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sstream>
#include <unistd.h>
#include <vector>
#include "err.h"
#include "helpers.h"

#define BUF_SIZE 1000
#define DEFAULT_TELNET_PORT 22
#define MAX_QUEUE_LENGTH 5
#define POLL_TIMEOUT 5000

class PlayerConnection;
void disconnect_player(PlayerConnection &);

class PlayerConnection {
	public:
		PlayerConnection() {}
		PlayerConnection(int id, std::string const &addr, long port) :
			id(id), addr(addr), port(port) {}
		int id;						/* unique id of the player */
		std::string addr;			/* addres of the computer */
		short port;					/* UDP port for player commands */
		LIBSSH2_SESSION *session;	/* ssh session */
		LIBSSH2_CHANNEL *channel;	/* ssh channel */
		int sock;					/* UDP socket for player commands */
		bool operator==(int id) { return this->id == id; }
};

short port;
bool show_port;

/* telnet_clients[0] serves as the central socket */
struct pollfd telnet_clients[_POSIX_OPEN_MAX];
struct sockaddr_in telnet_server;
int active_clients;
size_t length;

int i;
int id = 0;

static bool finished = false;

/* This vector hold the information about 
 * active ssh connections (to remote players) */
std::vector<PlayerConnection> players;

void disconnect_player(PlayerConnection &conn) {
	if (conn.channel) {
		libssh2_channel_free(conn.channel);

		conn.channel = NULL;
	}
	libssh2_session_disconnect(conn.session,
				"Normal Shutdown, Thank you for playing");
	libssh2_session_free(conn.session);
}

bool connect_player(PlayerConnection &conn) {
	int sock;
	struct addrinfo addr_hints;
	struct addrinfo *addr_result;
	const char *hostname = conn.addr.c_str();
	char username[BUF_SIZE];
	char *userauthlist;
	struct hostent *host = gethostbyname(hostname);
	if (host == NULL) {
		std::cerr << "Could not resolve host name" << std::endl;
		return false;
	}
	const char *homedir;
	if ((homedir = getenv("HOME")) == NULL) {
		std::cerr << "Could not resolve user directory" << std::endl;
		return false;
	}
	std::string keyfile1 = std::string(homedir) + std::string("/.ssh/id_rsa.pub");
	std::string keyfile2 = std::string(homedir) + std::string("/.ssh/id_rsa");
	if (getlogin_r(username, BUF_SIZE) != 0)
		fatal("Getting username");
	/* getting the address information */
	memset(&addr_hints, 0, sizeof(struct addrinfo));
	addr_hints.ai_family = AF_INET; // IPv4
	addr_hints.ai_socktype = SOCK_STREAM;
	addr_hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(host->h_name, "22", &addr_hints, &addr_result) != 0)
		fatal("getaddrinfo");
	/* creating and connecting the socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0) {
		syserr("connect");
	}
	conn.sock = sock;
	conn.session = libssh2_session_init();
	if (libssh2_session_handshake(conn.session, conn.sock)) {
		disconnect_player(conn);
		fatal("establishing ssh connection");
	}
	
	userauthlist = libssh2_userauth_list(conn.session, username, strlen(username));
	std::cout << "Authentication methods: " << userauthlist << std::endl;
	if (strstr(userauthlist, "publickey") == NULL) {
		fatal("cannot authenticate by public key");
	}
	if (libssh2_userauth_publickey_fromfile(conn.session, username, keyfile1.c_str(),
								keyfile2.c_str(), NULL)) {
		std::cerr << "\tAuthentication by public key failed!\n";
		perror("Error: ");

		return false;
	} else {
		std::cerr << "\tAuthentication by public key succeeded.\n";
	}
	if (!(conn.channel = libssh2_channel_open_session(conn.session))) {
		fprintf(stderr, "Unable to open a session\n");
		disconnect_player(conn);
		return false;
	}

	if (libssh2_channel_request_pty(conn.channel, "vanilla")) {
		std::cerr << "Failed requesting pty\n";
		disconnect_player(conn);
		return false;
    }
	if (libssh2_channel_shell(conn.channel)) {
		std::cerr << "Unable to request shell on allocated pty\n";
		disconnect_player(conn);
		return false;
	}
	return true;
}


void run_player(PlayerConnection const &conn, std::string const &parameters) {
	std::string command = "player " + parameters;
	const char *ccommand = command.c_str();
	int to_write = command.size();
	if (libssh2_channel_write_ex(conn.channel, 0, ccommand, to_write) < 0) {
		std::cerr << "Problem writing into ssh channel" << std::endl;
		return;
	}
}
	

namespace master {
	void start(std::string const &addr, std::string const &player_parameters) {
		std::string dummy;
		short port;
		std::istringstream(player_parameters) 	>> dummy >> dummy
												>> dummy >> dummy
												>> port;
		PlayerConnection conn(++id, addr, port);
		players.push_back(conn);
		if (connect_player(conn)) {
			run_player(conn, player_parameters);
			std::cout << "OK " << id << std::endl;
		}
	}
	
	void player_command(std::string const &cmd, int id, int telnet_pos) {
		for (i = 0; i < (int) players.size(); i++) {
			if (players[i] == id)
				break;
		}
		if (i == (int) players.size()) {
			std::cerr << "Wrong id" << std::endl;
			return;
		}
		int sock;
		struct addrinfo addr_hints;
		struct addrinfo *addr_result;
		int flags, sflags, err;
		char buffer[BUF_SIZE];
		size_t len;
		ssize_t snd_len, rcv_len;
		struct sockaddr_in my_address;
		struct sockaddr_in srvr_address;
		socklen_t rcva_len;
		
		(void) memset(&addr_hints, 0, sizeof(struct addrinfo));
		addr_hints.ai_family = AF_INET; // IPv4
		addr_hints.ai_socktype = SOCK_DGRAM;
		addr_hints.ai_protocol = IPPROTO_UDP;
		addr_hints.ai_flags = 0;
		addr_hints.ai_addrlen = 0;
		addr_hints.ai_addr = NULL;
		addr_hints.ai_canonname = NULL;
		addr_hints.ai_next = NULL;
		err = getaddrinfo(players[i].addr.c_str(), NULL, &addr_hints, &addr_result);
		
		if (err == EAI_SYSTEM) { // system error
			syserr("getaddrinfo: %s", gai_strerror(err));
		} else if (err != 0) { // other error (host not found, etc.)
			fatal("getaddrinfo: %s", gai_strerror(err));
		}

		my_address.sin_family = AF_INET; // IPv4
		my_address.sin_addr.s_addr =
			((struct sockaddr_in*) (addr_result->ai_addr))->sin_addr.s_addr; // address IP
		my_address.sin_port = htons(players[i].port);

		freeaddrinfo(addr_result);

		sock = socket(PF_INET, SOCK_DGRAM, 0);
		if (sock < 0)
			syserr("socket");
		sflags = 0;
		rcva_len = (socklen_t) sizeof(my_address);
		const char *msg = cmd.c_str();
		len = strnlen(msg, BUF_SIZE);
		snd_len = sendto(sock, msg, len, sflags,
			(struct sockaddr *) &my_address, rcva_len);
		#ifdef DEBUG
		std::cout << "Writing: " << cmd << std::endl;
		std::cout << "Address: " << players[i].addr << std::endl;
		std::cout << "Port: " << players[i].port << std::endl;

		#endif

		if (snd_len != (ssize_t) len) {
			std::cout << "len = " << len << ", snd_len = " << snd_len << std::endl;
			syserr("partial / failed write");
		}
		if (cmd == "TITLE") {
			(void) memset(buffer, 0, sizeof(buffer));
			flags = 0;
			len = (size_t) sizeof(buffer) - 1;
			rcva_len = (socklen_t) sizeof(srvr_address);
			rcv_len = recvfrom(sock, buffer, len, flags,
				(struct sockaddr *) &srvr_address, &rcva_len);

			if (rcv_len < 0) {
				syserr("read");
			}
			int rval = write(telnet_clients[telnet_pos].fd, buffer, BUF_SIZE);
			if (rval < 0)
				syserr("write");
		}
	}
	
	void at(int at_h, int at_m, int how_long, std::string const &addr, std::string const &player_parameters) {
		std::cout << "At is not developed yet" << std::endl;
	}
}

void parse_parameters(int argc, char *argv[]) {
	if (argc == 1) {
		port = 0;
		show_port = true;
	} else if (argc == 2) {
		port = parse_port_num(argv[1]);
		if (port < 0)
			fatal("Wrong parameter");
		show_port = false;
	} else {
		usage_err();
	}
	#ifdef DEBUG
	std::cout << "Parsed port: " << port << std::endl;
	#endif
}

void initialize_telnet_array() {
	for (i = 0; i < _POSIX_OPEN_MAX; ++i) {
		telnet_clients[i].fd = -1;
		telnet_clients[i].events = POLLIN;
		telnet_clients[i].revents = 0;
	}
	active_clients = 0;
}
void manifest_port_number() {
	length = sizeof(telnet_server);
	if (getsockname(telnet_clients[0].fd, (struct sockaddr*) &telnet_server,
			(socklen_t*) &length) < 0) {
		fatal("Getting socket name");
		exit(EXIT_FAILURE);
	}
	std::cout 	<< "Listening for commands on port "
				<< (unsigned) ntohs(telnet_server.sin_port)
				<< std::endl;
}

void create_bind_listen_telnet() {
	telnet_clients[0].fd = socket(PF_INET, SOCK_STREAM, 0);
	if (telnet_clients[0].fd < 0)
		fatal("Opening stream socket");
	telnet_server.sin_family = AF_INET;
	telnet_server.sin_addr.s_addr = htonl(INADDR_ANY);
	telnet_server.sin_port = htons(port);
		
	if (bind(
			telnet_clients[0].fd,
			(struct sockaddr *) &telnet_server,
			(socklen_t) sizeof(telnet_server)
		) < 0)
		fatal("Binding central socket");
	if (show_port)	
		manifest_port_number();

	if (listen(telnet_clients[0].fd, MAX_QUEUE_LENGTH) < 0)
		fatal("Turning on listen mode");
}


void process_new_connections() {
	int msgsock;
	#ifdef DEBUG
	std::cout << "New connection acquired" << std::endl;
	#endif
	msgsock = accept(
			telnet_clients[0].fd,
			(struct sockaddr*) 0,
			(socklen_t*) 0
			);
	if (msgsock < 0) {
		fatal("accept");
	} else {
		for (i = 1; i < _POSIX_OPEN_MAX; i++) {
			if (telnet_clients[i].fd == -1) {
				#ifdef DEBUG
				std::cout << "Occupying spot " << i << std::endl;
				#endif
				telnet_clients[i].fd = msgsock;
				active_clients++;
				break;
			}
		}
		if (i == _POSIX_OPEN_MAX) {
			std::cerr << "Too many clients!" << std::endl;
			if (close(telnet_clients[0].fd) < 0)
				fatal("close");
		}
	}
}

void process_command(std::string const &line, int sender_position) {
	std::istringstream istr(line);
	std::string cmd, dummy;
	istr >> cmd;
	if (cmd == "START") {
		std::string addr, player_parameters;
		if (!(istr >> addr) || !getline(istr, player_parameters))
			std::cerr << "Unknown command" << std::endl;
		ltrim(player_parameters);
		master::start(addr, player_parameters);
		
	} else if (cmd == "AT") {
		std::string when, addr, player_parameters;
		int how_long;
		if (!(istr >> when >> how_long >> addr) || !getline(istr, player_parameters)) {
			std::cerr << "Unknown command" << std::endl;
			return;
		}
		ltrim(player_parameters);
		int when_h, when_m;
		std::replace(when.begin(), when.end(), '.', ' ');
		std::istringstream(when) >> when_h >> when_m >> dummy;
		if (dummy != "") {
			std::cerr << "Unknown command" << std::endl;
			return;
		}
		master::at(when_h, when_m, how_long, addr, player_parameters);
	} else if (cmd == "PAUSE" || cmd == "PLAY"
			|| cmd == "TITLE" || cmd == "QUIT") {
		int id;
		if (!(istr >> id) || istr >> dummy) {
			std::cerr << "Unknown command" << std::endl;
			return;
		}
		master::player_command(cmd, id, sender_position);
	} else {
		std::cerr << "Unknown command" << std::endl;
		return;
	}
}

void read_telnet_command(int client_position) {
	char buf[BUF_SIZE + 1];
	memset(buf, 0, sizeof(buf));
	int rval = read(telnet_clients[client_position].fd, buf, BUF_SIZE);
	if (rval <= 0) {
		if (rval < 0)
			perror("read");
		if (close(telnet_clients[client_position].fd) < 0)
			perror("close");
		active_clients--;
		telnet_clients[client_position].fd = -1;
		std::cout << "Zerwano połączenie" << std::endl;
	} else {
		process_command(std::string(buf), i);
	}
}

void close_central() {
	if (telnet_clients[0].fd >= 0)
		if (close(telnet_clients[0].fd) < 0)
			perror("close");
}
static void catch_int (int sig) {
  finished = true;
  fprintf(stderr,
          "Signal %d caught No new connections will be accepted.\n", sig);
}

int main(int argc, char *argv[]) {
	// setting a way to handle Ctrl+C
	if (signal(SIGINT, catch_int) == SIG_ERR)
		fatal("Unable to change signal handler");
		
    if (libssh2_init (0) != 0) {
        std::cerr << "libssh2 initialization failed";
        return 1;
    }

	parse_parameters(argc, argv);
	initialize_telnet_array();
	create_bind_listen_telnet();

	int ret;
	do {
		for (i = 0; i < _POSIX_OPEN_MAX; i++)
			telnet_clients[i].revents = 0;
		/* On Ctrl+C, we close the central socket
		 * (i.e. do not accept new connections */
		if (finished && telnet_clients[0].fd >= 0) {
			if (close(telnet_clients[0].fd) < 0)
				fatal("close");
			telnet_clients[0].fd = -1;
		}
		ret = poll(telnet_clients, _POSIX_OPEN_MAX, POLL_TIMEOUT);
		if (ret < 0) {
			fatal("poll");
		} else if (ret > 0) {
			if (!finished && (telnet_clients[0].revents & (POLLIN | POLLERR))) {
				/* Accepting (or declining) new connections */
				process_new_connections();
			}
			/* Processing the data from all our telnet friends */
			for (i = 1; i < _POSIX_OPEN_MAX; i++) {
				if (telnet_clients[i].fd != -1 &&
					(telnet_clients[i].revents & (POLLIN | POLLERR))) {
					read_telnet_command(i);
				}
			}
		}
		
	} while (!finished || active_clients > 0);

	close_central();
	libssh2_exit();
	return 0;
}
