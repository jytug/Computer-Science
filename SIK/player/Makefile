TARGET: player master

debug: CPPFLAGS += -DDEBUG
debug: LDFLAGS += -DDEBUG
debug: player master

CC = g++
CPPFLAGS = -std=c++11 -Wall -O2
LDFLAGS = -std=c++11 -Wall

player.o err.o master.o: err.h
player.o master.o: helpers.h

player: player.o err.o helpers.o
	$(CC) $(CXXFLAGS) $^ -o $@
	
master: master.o err.o helpers.o
	$(CC) $(LDFLAGS) $^ -o $@ -lssh2


.PHONY: clean TARGET
clean:
	rm -rf player master *.o *.bak *~ *.mp3
