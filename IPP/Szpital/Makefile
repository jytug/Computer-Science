CC = gcc

all: parse structure hospital

parse: parse.c parse.h structure.h
	$(CC) -g -c parse.c 

structure: structure.h structure.c
	$(CC) -g -c structure.c 

hospital: parse.c structure.h hospital.c
	$(CC) -g -c hospital.c
	$(CC) -o hospital parse.o structure.o hospital.o

clean: parse.o structure.o hospital
	rm parse.o structure.o hospital.o hospital
