#ifndef _PARSE_H
#define _PARSE_H

int read_line(char **);
int deduce_command(const char *);
void forward_command(int, const char *);
void parse_enter(const char *);
void parse_copy(const char *);
void parse_change(const char *);
void parse_print(const char *);
void parse_delete(const char *);
void do_the_reading();

#endif /*_PARSE_H*/
