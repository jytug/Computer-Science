#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "engine.h"
#include "lists.h"
#include "err.h"

int initialise_twice() {
	int i;
	int first_player;		// first player to initialize
	int x1, y1, x2, y2;
	command *init1, *init2;
	init1 = parse_command();
	first_player = init1->data[2];
	x1 = init1->data[3];
	y1 = init1->data[4];
	x2 = init1->data[5];
	y2 = init1->data[6];
	if (strcmp(init1->name, "INIT") != 0)
		return -1;
	if (init(init1->data[0],
			init1->data[1],
			init1->data[2],
			init1->data[3],
			init1->data[4],
			init1->data[5],
			init1->data[6]) == -1)
		return -1;
	print_topleft();
	init2 = parse_command();
	if (strcmp(init2->name, "INIT") != 0)
		return -1;
	if (init2->data[2] == first_player ||
			(x1 != init2->data[3] || y1 != init2->data[4] ||
			x2 != init2->data[5] || y2 != init2->data[6]))
		return -1;
	if (init(init2->data[0],
			init2->data[1],
			init2->data[2],
			init2->data[3],
			init2->data[4],
			init2->data[5],
			init2->data[6]) == -1) 
		return -1;
	for (i = 0; i < 7; i++)
		if (i != 2 && init1->data[i] != init2->data[i])
			return -1;
	print_topleft();
	return 1;
}

int distribute_commands(command *cmd) {
	if (strcmp(cmd->name, "MOVE") == 0)
		return move(cmd->data[0],
					cmd->data[1],
					cmd->data[2],
					cmd->data[3]);
	else if (strcmp(cmd->name, "PRODUCE_KNIGHT") == 0)
		return produce_knight(cmd->data[0],
					cmd->data[1],
					cmd->data[2],
					cmd->data[3]);
	else if (strcmp(cmd->name, "PRODUCE_PEASANT") == 0)
		return produce_peasant(cmd->data[0],
					cmd->data[1],
					cmd->data[2],
					cmd->data[3]);
	else if (strcmp(cmd->name, "END_TURN") == 0)
		return end_turn();
	return -1;
}

int main() {
	start_game();

	command *new_command;
	int err;
	err = initialise_twice();
	if (err == -1) {
		input_error();
		return 42;
	}
	while (!game_over()) {
		new_command = parse_command();
		if ((err = distribute_commands(new_command)) == -1) {
			input_error();
			return 42;
		}
		if (err != END_TURN)
			print_topleft();

	}

	print_result();
	end_game();

	return 0;
}
