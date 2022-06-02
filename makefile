CC=gcc
CXX=clang++-9 -std=c++2a
FLAGS=-lpthread -fPIC
OBJECTS=active_object.o guard.o reactor.o queue.o
HEADERS=$(wildcard *.hpp)

all: main1

main1: main1.c
	$(CC) $(FLAGS) main1.c -o main1 -lpthread

client: client.c
	$(CC) client.c -o client
		./client "127.0.0.1"
