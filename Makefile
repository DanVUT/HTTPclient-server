CC = g++ 
CFLAGS = -std=c++11

all: ftrestd ftrest

ftrestd: server.cpp
		$(CC) $(CFLAGS) -o ftrestd server.cpp

ftrest: client.cpp
		$(CC) $(CFLAGS) -o ftrest client.cpp 


