#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

struct action
{
    int type;
    int coordinates[2];
    int board[4][4];
};

void usage(int argc, char *argv[])
{
    printf("Usage: %s <port>\n", argv[0]);
    exit(EXIT_FAILURE);
}

// void printBoard(char board[MAX_ROWS][MAX_COLS])
// {
//     for (int i = 0; i < MAX_ROWS; i++)
//     {
//         for (int j = 0; j < MAX_COLS; j++)
//         {
//             printf("%c ", board[i][j]);
//         }
//         printf("\n");
//     }
// }

int main(int argc, char *argv[])
{
    int s;

    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (addrparse(argv[1], argv[2], &storage) != 0)
    {
        usage(argc, argv);
    }
    s = socket(storage.ss_family, SOCK_STREAM, 0);

    if (s == -1)
    {
        logexit("socket");
    }
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (connect(s, addr, sizeof(storage)) != 0)
    {
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];
    int coordX;
    int coordY;
    struct action msg;

    while (1)
    {
        memset(buf, 0, BUFSZ);
        scanf("%s%d%d", buf, &coordX, &coordY);
        int type = transformActionStringInInt(buf);
        msg.type = type;
        msg.coordinates[0] = coordX;
        msg.coordinates[1] = coordY;

        send(s, &msg, sizeof(struct action), 0);

        msg.type = 25;
        msg.coordinates[0] = 25;
        msg.coordinates[1] = 25;

        recv(s, &msg, sizeof(struct action), 0);

        printf("type: %d\n", msg.type);
        printf("coordinates: %d %d\n", msg.coordinates[0], msg.coordinates[1]);

        printClientBoard(msg.board);
    }

    puts(buf);
    exit(EXIT_SUCCESS);
    return 0;
}