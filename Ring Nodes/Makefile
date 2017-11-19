CC = g++ -std=c++11 -Wall -I/usr/local/include -L/usr/local/lib
LDFLAGS = -lzmq -lzmqpp -pthread

all: cs1 cs2 cs3

cs1:
	$(CC) cs1.cpp -o cs1 $(LDFLAGS)
cs2:
	$(CC) cs2.cpp -o cs2 $(LDFLAGS)	
cs3:
	$(CC) cs3.cpp -o cs3 $(LDFLAGS)
