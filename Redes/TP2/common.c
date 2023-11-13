#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "common.h"

void logexit(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

// inicializa o sockaddr_storage com o endereço e porta passados dependendo do protocolo
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage)
{
    if (addrstr == NULL || portstr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0)
    {
        return -1;
    }
    // para converter do dispositivo para rede: host -> network short
    port = htons(port);

    struct in_addr inaddr4; // ipv4
    if (inet_pton(AF_INET, addrstr, &inaddr4))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6; // ipv6
    if (inet_pton(AF_INET6, addrstr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        //&inaddr6?
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

// função para inicializar o socket do servidor dependendo do protocolo
int serverSockaddrInit(const char *proto, const char *portstr, struct sockaddr_storage *storage)
{
    uint16_t port = (uint16_t)atoi(portstr);
    if (port == 0)
    {
        return -1;
    }
    port = htons(port); // host to network short
    memset(storage, 0, sizeof(*storage));
    if (strcmp(proto, "v4") == 0)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        // qualquer endereço
        addr4->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    else if (strcmp(proto, "v6") == 0)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else
    {
        return -1;
    }
}

int transformActionStringInInt(char *type)
{
    if (strcmp(type, "start") == 0)
    {
        return START;
    }
    else if (strcmp(type, "reveal") == 0)
    {
        return REVEAL;
    }
    else if (strcmp(type, "flag") == 0)
    {
        return FLAG;
    }
    else if (strcmp(type, "state") == 0)
    {
        return STATE;
    }
    else if (strcmp(type, "remove_flag") == 0)
    {
        return REMOVE_FLAG;
    }
    else if (strcmp(type, "reset") == 0)
    {
        return RESET;
    }
    else if (strcmp(type, "win") == 0)
    {
        return WIN;
    }
    else if (strcmp(type, "exit") == 0)
    {
        return EXIT;
    }
    else if (strcmp(type, "game_over") == 0)
    {
        return GAME_OVER;
    }
    else
    {
        return -1;
    }
}

char transformIntInChar(int intRepresentation)
{
    if (intRepresentation == BOMB)
    {
        return '*';
    }
    else if (intRepresentation == HIDDEN_CELL)
    {
        return '-';
    }
    else if (intRepresentation == 0)
    {
        return '0';
    }
    else if (intRepresentation == FLAGGED)
    {
        return '>';
    }
    else
    {
        return intRepresentation + '0';
    }
}

void printClientBoard(int board[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            printf("%c\t\t", transformIntInChar(board[i][j]));
        }
        printf("\n");
    }
}
