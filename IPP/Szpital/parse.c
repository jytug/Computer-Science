#include "parse.h"
#include "structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN "100000"

#define MAX_CMD_LEN 29
#define ENTER "NEW_DISEASE_ENTER_DESCRIPTION"
#define COPY "NEW_DISEASE_COPY_DESCRIPTION"
#define CHANGE "CHANGE_DESCRIPTION"
#define PRINT "PRINT_DESCRIPTION"
#define DELETE "DELETE_PATIENT_DATA"


void ignore() {
	puts("ignored");
}

int equal(const char *s1, const char *s2, int prec) {
	return strncmp(s1, s2, prec) == 0;
}

int read_line(char **line_ptr) {
	static size_t nbytes = 0;
	return getline(line_ptr, &nbytes, stdin);
}

void check_and_free(char *ptr) {
	if (ptr != 0)
		free(ptr);
}

int deduce_command(const char *line) {
	/* return value is the following:
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

void forward_command(const int cmd, const char *line) {
	switch (cmd) {
	case 0:
		ignore();
		break;
	case 1:
		parse_enter(line);
		break;
	case 2:
		parse_copy(line);
		break;
	case 3:
		parse_change(line);	
		break;
	case 4:
		parse_print(line);	
		break;
	case 5:
		parse_delete(line);
		break;
	}
}

void parse_enter(const char *line) {
	char *name = 0, *desc = 0;
	const int to_fill = 2;
	int err;
	err = sscanf(line, "%*s %ms %m[^\n]", &name, &desc);
	if (name == 0 || desc == 0 || err != to_fill)
		ignore();
	else 
		printf("insert(%s, %s)\n", name, desc);
	check_and_free(name);
	check_and_free(desc);
}

void parse_copy(const char *line) {
	char *name_to = 0, *name_from = 0, *dummy = 0;
	const int to_fill = 2;
	int err;
	err = sscanf(line, "%*s %ms %ms%ms", &name_to, &name_from, &dummy);
	if (name_to == 0 || name_from == 0 || dummy != 0 || err != to_fill)
		ignore();
	else
		printf("copy(%s, %s)\n", name_to, name_from);
	check_and_free(name_to);
	check_and_free(name_from);
	check_and_free(dummy);
}

void parse_change(const char *line) {
	char *name = 0, *desc = 0; 
	const int to_fill = 3;
	int err;
	int disease_n;
	err = sscanf(line, "%*s %ms %d %m[^\n]", &name, &disease_n, &desc);
	if (name == 0 || desc == 0 || err != to_fill)
		ignore();
	else
		printf("change(%s, %d, %s)\n", name, disease_n, desc);
	check_and_free(name);
	check_and_free(desc);
}

void parse_print(const char *line) {
	char *name = 0, *dummy = 0;
	const int to_fill = 2;
	int err;
	int disease_n;
	err = sscanf(line, "%*s %ms %d%ms", &name, &disease_n, &dummy);
	if (name == 0 || dummy != 0 || err != to_fill)
		ignore();
	else
		printf("print(%s, %d)\n", name, disease_n);
	check_and_free(name);
	check_and_free(dummy);
}

void parse_delete(const char *line) {
	char *name = 0, *dummy = 0;
	const int to_fill = 1;
	int err;
	err = sscanf(line, "%*s %ms%ms", &name, &dummy);
	if (name == 0 || dummy != 0 || err != to_fill)
		ignore();
	else
		printf("delete(%s)\n", name);
	check_and_free(name);
	check_and_free(dummy);
}

void do_the_reading() {
	char *line = 0;
	while (read_line(&line) != -1) {
		int cmd = deduce_command(line);		
		forward_command(cmd, line);
	}
	check_and_free(line);
}
