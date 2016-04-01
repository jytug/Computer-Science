CC = gcc

all: helpers parse structure hospital

helpers: helpers.c helpers.h
	$(CC) -g -c helpers.c

parse: parse.c parse.h structure.h
	$(CC) -g -c parse.c 

structure: structure.h structure.c
	$(CC) -g -c structure.c 

hospital: parse.o structure.o hospital.c
	$(CC) -g -c hospital.c
	$(CC) -o hospital helpers.o parse.o structure.o hospital.o

clean: 
	rm *.o hospital
