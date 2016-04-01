#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include <stdlib.h>

typedef struct {
	char *desc;
	size_t ref_count;
} disease;

struct d_node {
	disease *d;
	size_t no;
	struct d_node *next;
};

struct p_node {
	char *name;
	struct d_node *disease_list;
	struct p_node *next;
};

typedef struct {
	struct p_node *patients;
	size_t descriptions;
} hospital;

void enter(char *const , char *const);
void copy(char *const , char *const);
void change(char *const, const size_t, char *const);
void print(char *const, const size_t);
void destroy_structure();
void delete(char *const);
void write();

#endif /*_STRUCTURE_H*/
