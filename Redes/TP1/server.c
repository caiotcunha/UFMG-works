#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSZ 1024

struct action
{
    int type;
    int coordinates[2];
    int board[4][4];
};

// tipos de mensagem
// start 0
// reveal 1
// flag 2
// state 3
// remove_flag 4
// reset 5
// win 6
// exit 7
// game_over 8

void usage(int argc, char *argv[])
{
    printf("Usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("Example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int s;

    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (serverSockaddrInit(argv[1], argv[2], &storage) != 0)
    {
        usage(argc, argv);
    }
    s = socket(storage.ss_family, SOCK_STREAM, 0);

    if (s == -1)
    {
        logexit("socket");
    }
    // reutilizar o port
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (bind(s, addr, sizeof(storage)) != 0)
    {
        logexit("bind");
    }

    // num conexoes deve ser um ?
    if (listen(s, 10) != 0)
    {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("bound to %s, waiting connections\n", addrstr);

    while (1)
    {
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddr = (struct sockaddr *)(&clientStorage);
        // accept retorna um socket para a conexao
        socklen_t clientAddrLen = sizeof(clientStorage);
        int clientSock = accept(s, clientAddr, &clientAddrLen);
        if (clientSock == -1)
        {
            logexit("accept");
        }
        char clientAddrStr[BUFSZ];
        addrtostr(clientAddr, clientAddrStr, BUFSZ);

        printf("[log] connection from %s\n", clientAddrStr);

        char buf[BUFSZ];
        memset(buf, 0, BUFSZ);
        size_t count = recv(clientSock, buf, BUFSZ - 1, 0);
        printf("[msg] %s, %d bytes: %s\n", clientAddrStr, (int)count, buf);

        sprintf(buf, "remote endpoint: %.1000s", clientAddrStr);
        count = send(clientSock, buf, strlen(buf) + 1, 0);
        if (count != strlen(buf) + 1)
        {
            logexit("send");
        }
        close(clientSock);
    }

    exit(EXIT_SUCCESS);
    return 0;
}