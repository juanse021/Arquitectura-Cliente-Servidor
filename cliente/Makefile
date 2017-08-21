CC	= g++ -std=c++11 -Wall -L/usr/local/lib -I/usr/local/include
LDFLAGS = -lzmqpp -lzmq -lsfml-audio -pthread

all: cliente

cliente: cliente.cpp
	$(CC) -o cliente cliente.cpp $(LDFLAGS)

