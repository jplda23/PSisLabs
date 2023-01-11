all: chase-server chase-client


chase-client: ./client/chase-client.c ./header/chase.h
	gcc -Wall -pedantic ./client/chase-client.c -g -o ./exe/client -lncurses -lpthread

chase-server: ./server/chase-server.c ./header/chase.h
	gcc -Wall -pedantic ./server/chase-server.c -g -o ./exe/server -lncurses -lpthread
