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

	/*
	* Returns -1 if if a disease was freed from memory, 0 otherwise
	*/
int remove_disease(disease *d) {
	if (d->ref_count > 1) {
		d->ref_count -= 1;
		return 0;
	} else {
		free(d->desc);
		free(d);
		return -1;
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

	/*
	* Returns the number of diseases
	* freed during deleting the list
	*/
int delete_d_list(struct d_node *list) {
	if (list == NULL) {
		return 0;
	} else {
		int res = delete_d_list(list->next);
		res += remove_disease(list->d);
		free(list);
		return res;
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

	/*
	* Returns:
	*	0 if the patient was not on the list 
	*	1 otherwise
	*/
int insert_patient_disease(struct p_node **list_ptr,
		char *const name, disease *dis) {
	struct p_node *list = find_patient(*list_ptr, name);
	if (list == NULL) {
		struct d_node *disease_list = NULL;
		insert_disease(&disease_list, dis);
		*list_ptr = new_p_node(name, disease_list, *list_ptr);
		return 0;
	} else {
		insert_disease(&list->disease_list, dis);
		return 1;
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

	/*
	* Returns:
	*	-1 on failure
	*	0 if the patient named name1 was not on the list
	*	1 otherwise 
	*/
int copy_description(struct p_node **list_ptr,
		char *const name1, char *const name2) {
	struct p_node *patient2;
	patient2 = find_patient(*list_ptr, name2);
	if (patient2 == NULL || patient2->disease_list == NULL) {
		return -1;
	} else {
		return insert_patient_disease(list_ptr, name1, patient2->disease_list->d);
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
	/*
	* Returns:
	* 	-1 on failure
	*	1 otherwise
	*/
int change_description(struct p_node *list, char *const name,
		const size_t n, char *const desc) {
	struct p_node *patient = find_patient(list, name);
	struct d_node *d;
	if (patient == NULL || patient->disease_list == NULL ||
			(d = find_description(patient->disease_list, n)) == NULL) {
		return -1;
	} else {
		//struct d_node *it = find_description(patient->disease_list, n);  
		update_description(d, desc);
		return 1;
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

	/*
	* Returns the number of diseases freed from the 
	* memory during deletion (the number of diseases
	* that belonged only to the specified patient
	*/
int delete_patient(struct p_node *list, char *const name) {
	int res = 0;
	struct p_node *patient = find_patient(list, name);
	if (patient != NULL) {
		res = delete_d_list(patient->disease_list);
		patient->disease_list = NULL;
	}
	return res;
}

	/*
	* The following functions are propagated to the ,,parse'' module,
	* and don't require any knowledge of the data structure. They allocate 
	* the memory for the data stored inside the structure and free it if
	* not needed
	*/
void enter(char *const name, char *const desc) {
	char *name_copy = copy_string(name),
		*desc_copy = copy_string(desc);
	disease *d = new_disease(desc_copy, 0);
	int err = insert_patient_disease(&global_hospital()->patients,
			name_copy, d);
	if (err == 1)
		free(name_copy);
}

void copy(char *const name1, char *const name2) {
	char *name1_copy = copy_string(name1);
	int err = copy_description(&global_hospital()->patients, name1_copy, name2);
	if (err == -1) {
		ignore();
		free(name1_copy);
	} else if (err == 1) {
		free(name1_copy);
	}
}

void change(char *const name, const size_t n, char* const desc) {
	char *desc_copy = copy_string(desc);
	int err = change_description(global_hospital()->patients,
			name, n, desc_copy);
	if (err == -1)
		free(desc_copy);
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

void write() {
	write_p_list(global_hospital()->patients);
	printf("\n");
}
