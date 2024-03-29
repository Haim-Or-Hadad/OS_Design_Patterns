CC=gcc
Cpp=g++
OBJECTS=active_object.o guard.o reactor.o queue.o 


Cpp_HEADERS=$(wildcard *.hpp)
C_HEADERS=$(wildcard *.h)

all: main1 client pollclient Pollserver singleton guard 

main1: main1.o queue.o active_object.o
	$(CC) main1.o active_object.o queue.o -o main1 -lpthread

client: client.o
	$(CC) client.o -o client

pollclient: PollClient.o
	$(Cpp) PollClient.o -o pollclient -lpthread -fPIC


library.so: queue.o guard.o singleton.o reactor.o  main1.o
	$(Cpp) -shared -fPIC -o library.so guard.o singleton.o reactor.o queue.o main1.o

Pollserver: reactor.o pollserver.o 
	$(Cpp) reactor.o pollserver.o -o Pollserver -lpthread 

main1.o: $(C_HEADERS) main1.c
	$(CC) -c main1.c -lpthread -fPIC

active_object.o: $(C_HEADERS) active_object.c
	$(CC) -c active_object.c -lpthread -fPIC

queue.o: $(C_HEADERS) queue.c
	$(CC) -c queue.c -lpthread -fPIC


client.o: $(C_HEADERS) client.c
	$(CC) -lpthread -fPIC  -c client.c 

guard:guard.o
	$(Cpp) guard.o -o guard -lpthread

guard.o:guard.cpp 
	$(Cpp)  -c -fPIC  guard.cpp -lpthread

singleton:singleton.o
	$(Cpp) singleton.o -o singleton
	
singleton.o:   singleton.cpp 
	$(Cpp) -c -fPIC singleton.cpp 

reactor.o: $(Cpp_HEADERS) reactor.cpp
	$(Cpp) -c reactor.cpp -lpthread -fPIC

# g++ pollserver.cpp Reactor.cpp -o server -lpthread
pollserver.o: $(Cpp_HEADERS) pollserver.cpp
	$(Cpp) -c pollserver.cpp -lpthread -fPIC

PollClient.o: PollClient.cpp
	$(Cpp) -c PollClient.cpp -lpthread -fPIC	


clean: 
	rm -f *.o main1 client  pollclient Pollserver guard singleton  *.so 		
