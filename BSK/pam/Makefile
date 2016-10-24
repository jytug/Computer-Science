TARGET: bsk2
CC=g++
CPPFLAGS+=-Wall -std=c++11 -g -O2
LDFLAGS+=-lpam -lpam_misc
bsk2: bsk2.cpp
	$(CC) $(CPPFLAGS) bsk2.cpp -o $@ $(LDFLAGS)
