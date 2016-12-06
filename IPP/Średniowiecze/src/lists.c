/** @file
	Implementation of the list structure
*/
#include <stdlib.h>
#include <stdio.h>

#include "lists.h"

void init_list(struct node **lp) {
	*lp = NULL;
}

void insert(struct node **lp,
		enum piece p,
		int x,
		int y,
		int last_move,
		int player) {
	struct node *new_node = malloc(sizeof(struct node));
	new_node->piece = p;
	new_node->x = x;
	new_node->y = y;
	new_node->last_move = last_move;
	new_node->player = player;
	new_node->next = *lp;
	*lp = new_node;
}

struct node *find(struct node *list, int x, int y) {
	if (list == NULL)
		return NULL;
	else if (list->x == x && list->y == y)
		return list;
	else
		return (find(list->next, x, y));
}

void remove_node(struct node **list, int x, int y) {
	struct node guard;
	struct node *prev, *curr;
	guard.next = *list;
	prev = &guard;
	curr = prev->next;
	while (curr != NULL) {
		if (curr->x == x && curr->y == y) {
			prev->next = curr->next;
			free(curr);
			break;
		}
		prev = prev->next;
		curr = curr->next;
	}
	*list = guard.next;
}

void destroy(struct node *list) {
	if (list != NULL) {
		destroy(list->next);
		free(list);
	}
}
