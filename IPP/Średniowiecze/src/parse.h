 /** @file
	Interface of parser.
 */

#ifndef PARSE_H
#define PARSE_H

/** Command container. */
typedef struct def_command {
	char name[16];			/**< Command name*/
	int data[7];			/**< Up to seven command parameters */
} command;


/** Reads a command.
  Returns command with data points using 'command' structure.
  */
command* parse_command();

#endif /* PARSE_H */
