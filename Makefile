CC = g++
CCFLAGS =  -std=c++17 -O0 -lm #-Wall -Wextra

.PHONY: build clean client

build: client

run: client
	./client

client:
	$(CC) $(CCFLAGS) -o client src/*.cpp

clean:
	rm -f client