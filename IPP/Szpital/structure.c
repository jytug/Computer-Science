#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "helpers.h"
#include "structure.h"

hospital *global_hospital() {
	static hospital instance;
	return &instance;
}

disease *new_disease(char *const desc, const size_t ref_count) {
	disease *res = malloc(sizeof(disease));
	res->desc = desc;
	res->ref_count = ref_count;
	return res;
}

void remove_disease(disease *d) {
	if (d->ref_count > 1) {
		d->ref_count -= 1;
	} else {
		free(d->desc);
		free(d);
	}
}

struct d_node *new_d_node(disease *d, size_t no, struct d_node *next) {
	struct d_node *res = malloc(sizeof(struct d_node));
	res->d = d;
	res->no = no;
	res->next = next;
	return res;
}

void insert_disease(struct d_node **list_ptr, disease *d) {
	struct d_node *list = *list_ptr;
	size_t no = (list == NULL ? 1 : list->no + 1);
	*list_ptr = new_d_node(d, no, list);
	d->ref_count++;
}

void delete_d_list(struct d_node *list) {
	if (list != NULL) {
		delete_d_list(list->next);
		remove_disease(list->d);
		free(list);
	}
}

struct p_node *new_p_node(char *const name, struct d_node *disease_list,
		struct p_node *next) {
	struct p_node *res = malloc(sizeof(struct p_node));
	res->name = name;
	res->disease_list = disease_list;
	res->next = next;
	return res;
}

struct p_node *find_patient(struct p_node *list, char *const name) {
	struct p_node *it = list;
	while (it != NULL && strcmp(it->name, name) != 0) // TODO strncmp?
		it = it->next;
	return it;
}

void insert_patient_disease(struct p_node **list_ptr,
		char *const name, disease *dis) {
	struct p_node *list = find_patient(*list_ptr, name);
	if (list == NULL) {
		struct d_node *disease_list = NULL;
		insert_disease(&disease_list, dis);
		*list_ptr = new_p_node(name, disease_list, *list_ptr);
	} else {
		insert_disease(&list->disease_list, dis);
	}
}

void delete_p_list(struct p_node *list) {
	if (list != NULL) {
		delete_p_list(list->next);
		delete_d_list(list->disease_list);
		free(list->name);
		free(list);
	}
}

void write_p_list(struct p_node *list) {
	while (list != NULL) {
		printf("Patient: %s\n", list->name);
		struct d_node *dis = list->disease_list;
		while (dis != NULL) {
			printf("\t%u. %s\n", dis->no, dis->d->desc);
			dis = dis->next;
		}
		list = list->next;
	}
}

void copy_description(struct p_node *list,
		char *const name1, char *const name2) {
	struct p_node *patient1, *patient2;
	patient1 = find_patient(list, name1);
	patient2 = find_patient(list, name2);
	if (patient1 == NULL || patient2 == NULL) {
		ignore();
	} else {
		if (patient1->disease_list == NULL || patient2->disease_list == NULL) {
			ignore();
		} else {
			insert_disease(&patient1->disease_list, patient2->disease_list->d);
		}
	}
}

void update_description(struct d_node *list, char *desc) {
	if (list == NULL) {
		ignore();
	} else {
		remove_disease(list->d);
		list->d = new_disease(desc, 1);
	}
}

struct d_node *find_description(struct d_node *list, size_t n) {
	struct d_node *it = list;
	if (it == NULL || n > it->no || n < 1)
		return NULL;
	else
		while (it != NULL && it->no != n)
			it = it->next;
	return it;
}

void change_description(struct p_node *list, char *name, size_t n, char *desc) {
	struct p_node *patient = find_patient(list, name);
	if (patient == NULL || patient->disease_list == NULL) {
		ignore();
	} else {
		struct d_node *it = find_description(patient->disease_list, n);  
		update_description(it, desc);
	}
}

void print_description(struct p_node *list, char *const name, const size_t n) {
	struct p_node *patient = find_patient(list, name);
	struct d_node *dis = find_description(
		patient == NULL ? NULL : patient->disease_list, n);
	if (dis == NULL)
		ignore();
	else
		puts(dis->d->desc);
}

void delete_patient(struct p_node *list, char *const name) {
	struct p_node *patient = find_patient(list, name);
	delete_d_list(patient->disease_list);
	patient->disease_list = NULL;
}

	/*
	* The following functions are propagated to the ,,parse'' module,
	* and don't require any knowledge of the data structure	
	*/
void enter(char *const name, char *const desc) {
	disease *d = new_disease(desc, 0);
	insert_patient_disease(&global_hospital()->patients, name, d);
}

void copy(char *const name1, char *const name2) {
	copy_description(global_hospital()->patients, name1, name2);
}

void change(char *const name, const size_t n, char* const desc) {
	change_description(global_hospital()->patients, name, n, desc);
}

void print(char *const name, const size_t n) {
	print_description(global_hospital()->patients, name, n);
}

void delete(char *const name) {
	delete_patient(global_hospital()->patients, name);
}

void destroy_structure() {
	delete_p_list(global_hospital()->patients);
}
