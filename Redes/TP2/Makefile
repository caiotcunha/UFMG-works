all:
	gcc -Wall client.c common.c -o ./bin/client
	gcc -Wall server.c common.c -o ./bin/server
	gcc -Wall -lpthread server-mt.c common.c -o ./bin/server-mt