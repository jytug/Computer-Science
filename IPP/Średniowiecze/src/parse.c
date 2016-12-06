/** @file
	Implementation of the parser

 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 101			/**< The size of the buffer */
#define MAX_CMD_NAME_LEN 15		/**< Maximum length of a command 
									(must be smaller that BUF_SIZE)*/

/** Command container. */
typedef struct def_command {
	char name[16];
	int data[7];
} command;

/** Reads a line from stdin.
	Clears the buffer. On failure, leaves it cleared
	@param[in] buf Buffer for the line to be saved at
	@return -1 on failure, 1 on success.
*/
	
int read_line(char buf[BUF_SIZE]) {
	memset(buf, 0, BUF_SIZE);
	fgets(buf, BUF_SIZE, stdin);
	if (strnlen(buf, BUF_SIZE - 1) == BUF_SIZE - 1) {
		memset(buf, 0, BUF_SIZE);
		return -1;
	} else {
		return 1;
	}
}

/** Checks whitespace validity of a string.
	Checks if a line (including the end-of-line sign)
	contains leading or closing whitespaces, two
	consecutive whitespaces or a whitespace
	other than the space sign. Returns -1 if
	any of these conditions are met, 1 otherwise
	@param [in] buf Line to be validated
*/
int whitespace_valid(char buf[BUF_SIZE]) {
	const int len = strnlen(buf, BUF_SIZE) - 1;
 	/* Detect leading and ending whitespaces */
	if (len > 0 && (isspace(buf[0]) || isspace(buf[len - 1])))
		return -1;
	int i;
	char c;
	int after_whitespace;
	for (i = 0; i < len; i++) {
		c = buf[i];
		if (isspace(c)) {
			if (c != ' ' || after_whitespace)
				return -1;
			after_whitespace = 1;
		} else {
			after_whitespace = 0;
		}
	}
	return 1;
}

/** Parses a command with seven integer arguments.
	Returns -1 for invalid line, 1 otherwise.
	@param[in] line Line to be read from
	@param[in] cmd A pointer to the command to store the data parsed
	@return 1 on success, -1 on failure
*/
int parse_seven(char *line, command *cmd) {
	int data[7] = {0};
	char dummy[BUF_SIZE];
	if (sscanf(line, "%*s%d %d %d %d %d %d %d%s",
		&data[0],
		&data[1],
		&data[2],
		&data[3],
		&data[4],
		&data[5],
		&data[6],
		dummy) == 7) {
		int i;
		for (i = 0; i < 7; i++)
			cmd->data[i] = data[i];
		return 1;
	} else {
		return -1;
	}
}

/** Parses a command with four integer arguments.
	@param[in] line Line to be read from
	@param[in] cmd A pointer to the command to store the data parsed
	@return 1 on success, -1 on failure (invalid line)
*/
int parse_four(char *line, command *cmd) {
	int data[7] = {0};
	char dummy[BUF_SIZE];
	if (sscanf(line, "%*s %d %d %d %d%s",
		&data[0],
		&data[1],
		&data[2],
		&data[3],
		dummy) == 4) {
		int i;
		for (i = 0; i < 7; i++)
			cmd->data[i] = data[i];
		return 1;
	} else {
		return -1;
	}
}

/** Parses a command with no integer arguments.
	@param[in] line Line to be read from
	@param[in] cmd A pointer to the command to store the data parsed
	@return 1 on success, -1 on failure (invalid line)
*/
int parse_zero(char *line, command *cmd) {
	int data[7] = {0};
	char dummy[BUF_SIZE];
	if (sscanf(line, "%*s%s",
		dummy) < 1) {
		int i;
		for (i = 0; i < 7; i++)
			cmd->data[i] = data[i];
		return 1;
	} else {
		return -1;
	}
}

/** Parses a line read from stdin according to the specification.
	@param[in] line Line to be read from
	@param[in] cmd A pointer to the command to store the data parsed
	@return 1 on success, -1 on failure (invalid line)
*/

int parse_line(char *line, command *cmd) {
	char name[BUF_SIZE] = {0};
	sscanf(line, "%s", name);
	if (strncmp(name, "INIT", BUF_SIZE) == 0)
		if (parse_seven(line, cmd) == 1) {
			strncpy(cmd->name, name, sizeof(cmd->name));
			return 1;
		} else {
			return -1;
		}
	else if (strncmp(name, "MOVE", BUF_SIZE) == 0
			|| strncmp(name, "PRODUCE_KNIGHT", BUF_SIZE) == 0
			|| strncmp(name, "PRODUCE_PEASANT", BUF_SIZE) == 0)
		if (parse_four(line, cmd) == 1) {
			strncpy(cmd->name, name, sizeof(cmd->name));
			return 1;
		} else {
			return -1;
		}
	else if (strncmp(name, "END_TURN", BUF_SIZE) == 0)
		if (parse_zero(line, cmd) == 1) {
			strncpy(cmd->name, name, sizeof(cmd->name));
			return 1;
		} else {
			return -1;
		}
	else
		return -1;
}

/** Takes a line from stdin and parses it as a command.
	@return formatted, valid (from the parser point of view)
	command as a pointer to struct command
*/
command* parse_command() {
	static command my_command;
	static command none = { .name = "none", .data = {0}};
	char buf[BUF_SIZE];
	if (read_line(buf) == -1)
		return &none;
	if (whitespace_valid(buf) == -1)
		return &none;
	if (parse_line(buf, &my_command) == -1)
		return &none;
	return &my_command;
}
