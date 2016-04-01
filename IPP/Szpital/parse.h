#ifndef _PARSE_H
#define _PARSE_H

int read_line(char **);
int deduce_command(char *const );
void forward_command(int, char *const );
void parse_enter(char *const );
void parse_copy(char *const );
void parse_change(char *const );
void parse_print(char *const );
void parse_delete(char *const );
void do_the_reading();

#endif /*_PARSE_H*/
