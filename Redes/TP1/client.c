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
    memset(buf, 0, BUFSZ);
    scanf("%s%d%d",buf,&coordX,&coordY);
    struct action msg;
    if(strcmp("start",buf) == 0 ){
        msg.type = 0;
    }
    msg.coordinates[0] = coordX;
    msg.coordinates[1] = coordY;
    printf("coordenada 0:%d\n",msg.coordinates[0]);
    printf("coordenada 1:%d\n",msg.coordinates[1]);

    size_t count = send(s, &msg, sizeof(struct action), 0);
    // if (count != strlen(buf) + 1)
    // {
    //     logexit("send");
    // }

    memset(buf, 0, BUFSZ);
    unsigned total = 0;
    while (1)
    {
        count = recv(s, buf + total, BUFSZ - total, 0);
        if (count == 0)
        {
            break;
        }
        total += count;
        if (count < 0)
        {
            logexit("recv");
        }
        memset(buf, 0, BUFSZ);
    }

    printf("received %u bytes\n", total);
    puts(buf);
    exit(EXIT_SUCCESS);
    return 0;
}