/** @file
	Implementation of the game engine
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>

#include "lists.h"
#include "helpers.h"
#include "engine.h"
#include "piece.h"

#define MAX_BOARD_SIZE 		2147483647 	/**< Maximum board size*/
#define MIN_BOARD_SIZE 		9			/**< Maximum board size*/
#define MAX_TOPLEFT_SIZE	10			/**< Maximum topleft corner size*/
#define MIN_MAX_ROUNDS 		1			/**< Maximum maximum rounds number*/
#define MAX_MAX_ROUNDS 		2147483647	/**< Maximum maximum rounds number*/
#define MIN_KING_DISTANCE 	8			/**< Minimum distance to separate
											the kings initially*/
#define PROD_WAIT			3			/**< Time to wait before
											creating knight or peasant*/

/** A structure to hold all information about the game.
*/
typedef struct def_game {
	int board_size;					/**< Size of the game board*/	
	int max_rounds;					/**< Maximum number of rounds to be played*/
	int current_player;				/**< Number of the current player*/
	int current_round;				/**< Number of the current round*/
	struct node *pieces;			/**< A list of all the pieces on board*/
	char topleft[MAX_TOPLEFT_SIZE][MAX_TOPLEFT_SIZE];
									/**< An array to hold the top-left corner of
										the board to be displayed*/
	int winner;						/**< Number of the player who won*/
	int over;						/**< 1 if game is over, 0 otherwise*/
} game;

/** Returns a pointer to the container of all game information
*/
game *global_game() {
	static game instance;
	return &instance;
}

/** Returns a character representation of a piece.
*/
char piece_repr(enum piece p, int player) {
	char res;
	switch(p) {
		case king:
			res = 'K';
			break;
		case knight:
			res = 'R';
			break;
		case peasant:
			res = 'C';
			break;
		case none:
			res = '.';
			break;
		default:
			assert(0);
	}
	if (player == 1)
		return res;
	else if (player == 2)
		return tolower(res);
	else
		assert(0);
	return res;
}

/** Adds a piece to the topleft corner of the board.
	@param[in] p The piece type to be added
	@param[in] x The horizontal position of this piece
	@param[in] y The vertical position of this piece
	@param[in] player Number of the player who owns this piece
*/
void topleft_add(enum piece p, int x, int y, int player) {
	if (x <= MAX_TOPLEFT_SIZE && y <= MAX_TOPLEFT_SIZE)
		global_game()->topleft[x - 1][y - 1] = piece_repr(p, player);
}

void start_game() {
	int i;
	global_game()->board_size = 0;
	global_game()->max_rounds = 0;
	global_game()->current_player = 0;
	global_game()->current_round = 0;
	init_list(&(global_game()->pieces));
	for (i = 0; i < 100; i++) {
		global_game()->topleft[i / MAX_TOPLEFT_SIZE][i % MAX_TOPLEFT_SIZE] =
			piece_repr(none, 1);
	}
	global_game()->winner = 0;
	global_game()->over = 0;
}

void end_game() {
	destroy(global_game()->pieces);
}

void print_topleft() {
	int i, j;
	const int topleft_size = min(MAX_TOPLEFT_SIZE, global_game()->board_size);
	for (j = 0; j < topleft_size; j++) {
		for (i = 0; i < topleft_size; i++)
			putchar(global_game()->topleft[i][j]);
		putchar('\n');
	}
	putchar('\n');
}

/** Checks if the input describes a valid init.
	Checks if the input describes a valid init, i.e.
	that the board size, maximum number of rounds
	is not too large nor too small, if the
	initial positions of the kings is large
	enough etc.
	@return 1 if input describes a valid init, -1 otherwise
*/
int validate_init(int n, int k, int p, int x1, int y1, int x2, int y2) {
	if ((n < MIN_BOARD_SIZE || n > MAX_BOARD_SIZE)
		|| (k < MIN_MAX_ROUNDS || k > MAX_MAX_ROUNDS)
		|| (p != 1 && p != 2)
		|| (x1 < 1 || x1 > n - 3) || (y1 < 1 || y1 > n)
		|| (x2 < 1 || x2 > n - 3) || (y2 < 1 || y2 > n)
		|| max_metric(x1, y1, x2, y2) < MIN_KING_DISTANCE)
		return -1;
	return 1;
}


/** Checks if the input describes a valid move or action.
	Checks if the input describes a valid move or action, i.e.
	that the destination is in the neighbourhood of
	the starting position.
	Note: it doesn't check whether a player has a piece
	on coordinates (x1, y1) or whether the position
	(x2, y2) is occupied (and by whom)
	@return -1 for bad input, 1 otherwise
*/
int validate_move_action(int x1, int y1, int x2, int y2) {
	const int n = global_game()->board_size;
	if ((max_metric(x1, y1, x2, y2) != 1)
		|| x1 < 1 || x1 > n || y1 < 1 || y1 > n
		|| x2 < 1 || x2 > n || y2 < 1 || y2 > n)
		return -1;
	return 1;
}

/** Compares two pieces.
	@param[in] p1 One piece to compare
	@param[in] p2 Other piece to compare
	@return -1 if p2 beats p1, 0 on draw, 1 if p1 beats p2
*/
int compare(enum piece p1, enum piece p2) {
	if (p1 == p2)
		return 0;
	if (p1 == knight || (p1 == king && p2 == peasant))
		return 1;
	return -1;
}

/** Conducts a fight between two pieces.
	Assumes p1 is the attacker. On draw, deletes
	both nodes and updates the topleft corner.
	On victory of p1, replaces p2 with p1. On
	victory of p2, deletes p1
	@param[in] p1 The attacker
	@param[in] p2 The attacked
*/
void fight(struct node *p1, struct node *p2) {
	assert(p1 != NULL && p2 != NULL);
	int result = compare(p1->piece, p2->piece);
	if (result == -1) {
		if (p1->piece == king) {
			global_game()->over = 1;
			global_game()->winner = p2->player;
		}
		topleft_add(none, p1->x, p1->y, p1->player);
		remove_node(&global_game()->pieces, p1->x, p1->y);
	} else if (result == 0) {
		if (p1->piece == king) {
			global_game()->over = 1;
		}
		topleft_add(none, p1->x, p1->y, 1);
		topleft_add(none, p2->x, p2->y, 1);
		remove_node(&global_game()->pieces, p1->x, p1->y);
		remove_node(&global_game()->pieces, p2->x, p2->y);
	} else {
		if (p2->piece == king) {
			global_game()->over = 1;
			global_game()->winner = p1->player;
		}
		topleft_add(none, p1->x, p1->y, 1);
		topleft_add(p1->piece, p2->x, p2->y, p1->player);
		int x, y;
		x = p2->x;
		y = p2->y;
		remove_node(&global_game()->pieces, p2->x, p2->y);
		p1->x = x;
		p1->y = y;
	}
}

int init(int n, int k, int p, int x1, int y1, int x2, int y2) { 
	static int already_called;
	if (validate_init(n, k, p, x1, y1, x2, y2) == -1)
		return -1;
	if (already_called && (n != global_game()->board_size ||
			k != global_game()->max_rounds))
		return -1;
	global_game()->board_size = n;
	global_game()->max_rounds = k;
	global_game()->current_round = 1;
	global_game()->current_player = 1;

	if (!already_called) {
		insert(&global_game()->pieces, king, x1, y1, 0, 1);
		insert(&global_game()->pieces, peasant, x1 + 1, y1, 0, 1);
		insert(&global_game()->pieces, knight, x1 + 2, y1, 0, 1);
		insert(&global_game()->pieces, knight, x1 + 3, y1, 0, 1);
		topleft_add(king, x1, y1, 1);
		topleft_add(peasant, x1 + 1, y1, 1);
		topleft_add(knight, x1 + 2, y1, 1);
		topleft_add(knight, x1 + 3, y1, 1);
	
		insert(&global_game()->pieces, king, x2, y2, 0, 2);
		insert(&global_game()->pieces, peasant, x2 + 1, y2, 0, 2);
		insert(&global_game()->pieces, knight, x2 + 2, y2, 0, 2);
		insert(&global_game()->pieces, knight, x2 + 3, y2, 0, 2);
		topleft_add(king, x2, y2, 2);
		topleft_add(peasant, x2 + 1, y2, 2);
		topleft_add(knight, x2 + 2, y2, 2);
		topleft_add(knight, x2 + 3, y2, 2);
	}
	already_called = 1;
	return 1;
}

int move(int x1, int y1, int x2, int y2) {
	if (validate_move_action(x1, y1, x2, y2) == -1)
		return -1;
	struct node *start, *dest;
	start = find(global_game()->pieces, x1, y1);
	if (start == NULL || start->player != global_game()->current_player
		|| start->last_move >= global_game()->current_round)
		return -1;
	dest = find(global_game()->pieces, x2, y2);
	if (dest != NULL) {
		if (dest->player == start->player) {
			return -1;
		} else {
			fight(start, dest);
			return 1;
		}
	}
	start->x = x2;
	start->y = y2;
	start->last_move = global_game()->current_round;
	topleft_add(none, x1, y1, 1);
	topleft_add(start->piece, x2, y2, start->player);
	return 1;
}

/** A helper function for piece production.
	@param[in] p A type of piece to be produced
	@param[in] x1 The column of the producer
	@param[in] y1 The row of the producer
	@param[in] x2 The column of the new piece
	@param[in] y2 The row of the new piece
	@return 1 if the production is valid, -1 otherwise
*/
int produce(enum piece p, int x1, int y1, int x2, int y2) {
	if (validate_move_action(x1, y1, x2, y2) == -1)
		return -1;
	struct node *source, *dest;
	source = find(global_game()->pieces, x1, y1);
	if (source == NULL || source->player != global_game()->current_player
				|| source->piece != peasant
				|| source->last_move > (global_game()->current_round - PROD_WAIT))
		return -1;
	dest = find(global_game()->pieces, x2, y2);
	if (dest != NULL)
		return -1;
	source->last_move = global_game()->current_round;
	insert(&global_game()->pieces,
		p,
		x2,
		y2,
		global_game()->current_round - 1,
		source->player);
	topleft_add(p, x2, y2, source->player);
	return 1;
}

int produce_knight(int x1, int y1, int x2, int y2) {
	return produce(knight, x1, y1, x2, y2);
}

int produce_peasant(int x1, int y1, int x2, int y2) {
	return produce(peasant, x1, y1, x2, y2);
}

int end_turn() {
	global_game()->current_player = 3 - global_game()->current_player;
	if (global_game()->current_player == 1)
		global_game()->current_round++;
	return END_TURN;
}

int game_over() {
	return (global_game()->max_rounds < global_game()->current_round ||
			global_game()->over);
}

void print_result() {
	assert(game_over());
	if (global_game()->winner == 0)
		fprintf(stderr, "draw\n");
	else
		fprintf(stderr, "player %d won\n", global_game()->winner);
}
