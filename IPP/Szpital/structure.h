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

hospital *global_hospital();
disease *new_disease(char *const , size_t);
void remove_disease(disease *);
struct d_node *new_d_node(disease *, size_t, struct d_node *);
void insert_disease(struct d_node **, disease *);
void delete_d_list(struct d_node *);

void insert_patient_disease(struct p_node **, char *const , disease *);
void delete_p_list(struct p_node *);
void write_p_list(struct p_node *);

void copy_description(struct p_node *, char *const, char *const);

void change_description(struct p_node *, char *, size_t, char *);

void print_description(struct p_node *, char *const , const size_t);

void delete_patient(struct p_node *, char *const);

void enter(char *const , char *const);
void copy(char *const , char *const);
void change(char *const, const size_t, char *const);
void print(char *const, const size_t);
void destroy_structure();
void delete(char *const);
#endif /*_STRUCTURE_H*/
