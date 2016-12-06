#ifndef _PARSE_H
#define _PARSE_H

int read_line(char **);
int deduce_command(char *const);
void forward_command(const int, char *const, const int);
void parse_enter(char *const, const int);
void parse_copy(char *const, const int);
void parse_change(char *const, const int);
void parse_print(char *const, const int);
void parse_delete(char *const, const int);
void do_the_reading(const int);

#endif /*_PARSE_H*/
