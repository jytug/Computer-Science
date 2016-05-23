/** @file
	Interface of game engine.
*/

#ifndef ENGINE_H
#define ENGINE_H

#define END_TURN	10		/**< Returned on success by end_turn*/

/**
	Initializes a game. Needed before first INIT.
*/
void start_game();

/**
	Frees memory. Needed after finishing game.
*/
void end_game();

/**
	Initializes a game with size of a board, number of rounds and positions of kings.
*/
int init(int n, int k, int p, int x1, int y1, int x2, int y2);

/**
	Makes a move.
	@param[in] x1 Column number before a move.
	@param[in] y1 Row number before a move.
	@param[in] x2 Column number after a move.
	@param[in] y2 Row number before a move.
	@return 1 if the move is valid, -1 otherwise.
*/
int move(int x1, int y1, int x2, int y2);

/**
	Produces a knight
	@param[in] x1 Column number of the producer.
	@param[in] y1 Row number before of the producer.
	@param[in] x2 Column number of the new knight.
	@param[in] y2 Row number of the new knight.
	@return 1 if the production is valid, -1 otherwise.
*/
int produce_knight(int x1, int y1, int x2, int y2);

/**
	Produces a peasant
	@param[in] x1 Column number of the producer.
	@param[in] y1 Row number before of the producer.
	@param[in] x2 Column number of the new peasant.
	@param[in] y2 Row number of the new peasant.
	@return 1 if the production is valid, -1 otherwise.
*/
int produce_peasant(int x1, int y1, int x2, int y2);

/**
	Ends the turn.
	Switches the current player, increments current round if
	necessary.
	@return 1
*/
int end_turn();

/**
	Prints (into stdout) top-left corner of the board of size m x m where m = min(n,10).
*/
void print_topleft();

/**
	Returns non-zero if game is over, 0 otherwise.
	@return non-zero value if game is over, zero otherwise
*/
int game_over();

/**
	Prints (into stdout) the information about the winner.
*/
void print_result();
#endif /* ENGINE_H */
