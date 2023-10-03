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
    int board[MAX_ROWS][MAX_COLS];
};

void usage(int argc, char *argv[])
{
    printf("Usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("Example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void readFile(char *filename, int board[MAX_ROWS][MAX_COLS])
{
    FILE *file;
    file = fopen(filename, "r");
    char line[100];
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    for (int i = 0; i < MAX_ROWS; i++)
    {
        if (fgets(line, sizeof(line), file) == NULL)
        {
            break; // End of file or error
        }
        char *token = strtok(line, ",");
        int j = 0;
        while (token != NULL && j < MAX_COLS)
        {
            board[i][j] = atoi(token);
            j++;
            token = strtok(NULL, ",");
        }
    }

    fclose(file);
}

void printBoard(int board[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

void inicializeBoardClientUnrevealed(int boardClientUnrevealed[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            boardClientUnrevealed[i][j] = -2;
        }
    }
}

void copyMatrix(int board[MAX_ROWS][MAX_COLS], int msgBoard[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            msgBoard[i][j] = board[i][j];
        }
    }
}

int main(int argc, char *argv[])
{
    int s;
    int board[MAX_ROWS][MAX_COLS];
    int boardClientUnrevealed[MAX_ROWS][MAX_COLS];
    inicializeBoardClientUnrevealed(boardClientUnrevealed);

    if (argc < 3)
    {
        usage(argc, argv);
    }

    char *filename = argv[4];
    readFile(filename, board);
    printBoard(board);

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

    // char addrstr[BUFSZ];
    // addrtostr(addr, addrstr, BUFSZ);

    // printf("bound to %s, waiting connections\n", addrstr);

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
        // char clientAddrStr[BUFSZ];
        // addrtostr(clientAddr, clientAddrStr, BUFSZ);

        // printf("[log] connection from %s\n", clientAddrStr);
        while(1){

            struct action msg;
            recv(clientSock, &msg, sizeof(struct action), 0);
            // count = send(clientSock, &msg, sizeof(struct action), 0);
            // if (count != 0)
            // {
            //     logexit("send");
            // }
            switch (msg.type)
            {
            case 0:
                copyMatrix(boardClientUnrevealed, msg.board);
                send(clientSock, &msg, sizeof(struct action), 0);
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 8:
                break;
            }
        }
    }

    exit(EXIT_SUCCESS);
    return 0;
}