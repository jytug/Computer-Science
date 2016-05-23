/** @file
	Interface of a list structure
*/

#ifndef _MIDDLE_AGES_LISTS_H
#define _MIDDLE_AGES_LISTS_H

#include "piece.h"

/** A struct which represents all pieces on board.
	Each node represents one piece
*/
struct node {
	enum piece piece;		/**< Type of piece*/
	int x;					/**< Horizontal position of piece*/
	int y;					/**< Vertical position of piece*/
	int last_move;			/**< Last round in which this moved or acted*/
	int player;				/**< Player which owns this piece*/
	struct node *next;		/**< A pointer to the next piece on a list*/
};

/** Initializes an empty list*/
void init_list(struct node **);

/** Inserts a value into a list. */
void insert(struct node **, enum piece, int, int, int, int);

/** Finds a value in a list.
	@return A pointer to the seeked value, if found, NULL otherwise*/
struct node *find(struct node *, int, int);

/** Removes a node from a list. */
void remove_node(struct node **, int, int);

/** Destroys a list. Frees all nodes from memory. */
void destroy(struct node *);

#endif
