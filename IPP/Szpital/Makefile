CC = gcc
CCFLAGS = -Wall

all: hospital

helpers.o: helpers.c helpers.h
	$(CC) $(CCFLAGS) -c helpers.c -o $@

parse.o: parse.c parse.h structure.h helpers.h
	$(CC) $(CCFLAGS) -c parse.c -o $@

structure.o: structure.h structure.c helpers.h
	$(CC) $(CCFLAGS) -c structure.c -o $@

hospital: helpers.o parse.o structure.o hospital.o

clean: 
	rm -rf *.o *.dbg hospital

hospital.dbg: helpers.o parse.o structure.o
	$(CC) $(CCFLAGS) -o hospital.dbg hospital.c helpers.o parse.o structure.o
	
debug: CCFLAGS += -g
debug: hospital.dbg
