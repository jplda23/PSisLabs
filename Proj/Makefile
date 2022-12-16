all: chase-server chase-client chase-bot


chase-client: ./human-client/chase-client.c ./header/chase.h
	gcc -Wall -pedantic ./human-client/chase-client.c -g -o ./exe/client -lncurses

chase-server: ./server/chase-server.c ./header/chase.h
	gcc -Wall -pedantic ./server/chase-server.c -g -o ./exe/server -lncurses

chase-bot: ./bot-client/chase-client-bots.c ./header/chase.h
	gcc -Wall -pedantic ./bot-client/chase-client-bots.c -g -o ./exe/bots -lncurses
