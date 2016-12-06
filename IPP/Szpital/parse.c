#include "helpers.h"
#include "parse.h"
#include "structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 29
#define ENTER "NEW_DISEASE_ENTER_DESCRIPTION"
#define COPY "NEW_DISEASE_COPY_DESCRIPTION"
#define CHANGE "CHANGE_DESCRIPTION"
#define PRINT "PRINT_DESCRIPTION"
#define DELETE "DELETE_PATIENT_DATA"

int read_line(char **line_ptr) {
	static size_t nbytes = 0;
	return getline(line_ptr, &nbytes, stdin);
}

void check_and_free(char *ptr) {
	if (ptr != 0)
		free(ptr);
}

int deduce_command(char *const line) {
	/* return value is the following:
		0 - invalid command
		1 - enter disease description,
		2 - copy disease description,
		3 - change description,
		4 - print description,
		5 - delete patient
	 */
	char *command;
	sscanf(line, "%ms", &command);
	int res = 0;
	if (equal(command, ENTER, MAX_CMD_LEN))
		res = 1;
	if (equal(command, COPY, MAX_CMD_LEN))
		res = 2;
	if (equal(command, CHANGE, MAX_CMD_LEN)) 
		res = 3;
	if (equal(command, PRINT, MAX_CMD_LEN))
		res = 4;
	if (equal(command, DELETE, MAX_CMD_LEN))
		res = 5;
	check_and_free(command);
	return res;
}

void forward_command(const int cmd, char *const line, const int err_info) {
	switch (cmd) {
	case 0:
		ignore();
		break;
	case 1:
		parse_enter(line, err_info);
		break;
	case 2:
		parse_copy(line, err_info);
		break;
	case 3:
		parse_change(line, err_info);	
		break;
	case 4:
		parse_print(line, err_info);	
		break;
	case 5:
		parse_delete(line, err_info);
		break;
	}
}

void parse_enter(char *const line, const int err_info) {
	char *name = 0, *desc = 0;
	const int to_fill = 2;
	int err;
	err = sscanf(line, "%*s %ms %m[^\n]", &name, &desc);
	if (name == 0 || desc == 0 || err != to_fill) {
		ignore();
	} else {
		enter(name, desc, err_info);
	}
	check_and_free(name);
	check_and_free(desc);
}

void parse_copy(char *const line, const int err_info) {
	char *name_to = 0, *name_from = 0, *dummy = 0;
	const int to_fill = 2;
	int err;
	err = sscanf(line, "%*s %ms %ms%ms", &name_to, &name_from, &dummy);
	if (name_to == 0 || name_from == 0 || dummy != 0 || err != to_fill)
		ignore();
	else
		copy(name_to, name_from, err_info);
	check_and_free(name_to);
	check_and_free(name_from);
	check_and_free(dummy);
}

void parse_change(char *const line, const int err_info) {
	char *name = 0, *desc = 0; 
	const int to_fill = 3;
	int err;
	size_t disease_n = 0;
	err = sscanf(line, "%*s %ms %lu %m[^\n]", &name, &disease_n, &desc);
	if (name == 0 || desc == 0 || err != to_fill) {
		ignore();
	} else {
		change(name, disease_n, desc, err_info);
	}
	check_and_free(desc);
	check_and_free(name);
}

void parse_print(char *const line, const int err_info) {
	char *name = 0, *dummy = 0;
	const int to_fill = 2;
	int err;
	size_t disease_n = 0;
	err = sscanf(line, "%*s %ms %lu%ms", &name, &disease_n, &dummy);
	if (name == 0 || dummy != 0 || err != to_fill)
		ignore();
	else
		print(name, disease_n, err_info);
	check_and_free(name);
	check_and_free(dummy);
}

void parse_delete(char *const line, const int err_info) {
	char *name = 0, *dummy = 0;
	const int to_fill = 1;
	int err;
	err = sscanf(line, "%*s %ms%ms", &name, &dummy);
	if (name == 0 || dummy != 0 || err != to_fill)
		ignore();
	else
		delete(name, err_info);
	check_and_free(name);
	check_and_free(dummy);
}

void do_the_reading(const int err_info) {
	char *line = 0;
	while (read_line(&line) != -1) {
		if (!equal(line, "\n", 1)) {
			int cmd = deduce_command(line);		
			forward_command(cmd, line, err_info);
		} else {
			ignore();
		}
	}
	check_and_free(line);
	destroy_structure();
}
