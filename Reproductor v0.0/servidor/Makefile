CC	= g++ -std=c++11 -Wall -L/usr/local/lib -I/usr/local/include
LDFLAGS = -lzmqpp -lzmq -lsfml-audio -pthread

all: servidor

servidor: servidor.cpp
	$(CC) -o servidor servidor.cpp $(LDFLAGS)

