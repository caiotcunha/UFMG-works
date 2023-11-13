#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>

#define BUFSZ 1024
#define WIN_COUNT 13



void usage(int argc, char *argv[])
{
    exit(EXIT_FAILURE);
};

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
            break;
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
};

void printBoard(int board[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            printf("%d\t\t", board[i][j]);
        }
        printf("\n");
    }
};

void inicializeBoardClientUnrevealed(int boardClientUnrevealed[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            boardClientUnrevealed[i][j] = HIDDEN_CELL;
        }
    }
};

void copyMatrix(int board[MAX_ROWS][MAX_COLS], int msgBoard[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            msgBoard[i][j] = board[i][j];
        }
    }
};

struct clientData{
    int clientSock;
    int idCount;
    int board[MAX_ROWS][MAX_COLS];
    struct sockaddr_storage storage;
};

void * clientThread(void *data){
    struct clientData *clientData = (struct clientData *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&clientData->storage);

        // loop do servidor
        while (1)
        {

            struct BlogOperation operation;
            recv(clientData->clientSock, &operation, sizeof(struct action), 0);

            if (operation.operation_type == EXIT)
            {
                close(clientData->clientSock);
                printf("client disconnected\n");
                break;
            }
            // decide a ação a ser tomada
            switch (operation.operation_type)
            {
            case NEW_CONNECTION:
                printf("client %02d connected\n",clientData->idCount);
                operation.client_id = clientData->idCount;
                operation.operation_type = 1;
                operation.server_response = 1;
                strcpy(operation.topic, "");
                strcpy(operation.content, "");
                send(clientData->clientSock, &operation, sizeof(struct BlogOperation), 0);
                break;
            }
        }
    close(clientData->clientSock);
    pthread_exit(EXIT_SUCCESS);

};



int main(int argc, char *argv[])
{
    int idCount = 0;
    int s;
    int board[MAX_ROWS][MAX_COLS];
    int boardClientUnrevealed[MAX_ROWS][MAX_COLS];

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

    if (listen(s, 10) != 0)
    {
        logexit("listen");
    }

    while (1)
    {
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddr = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddrLen = sizeof(clientStorage);
        int clientSock = accept(s, clientAddr, &clientAddrLen); // accept retorna um socket para a conexão
        if (clientSock == -1)
        {
            logexit("accept");
        }

        struct clientData *clientData = malloc(sizeof (*clientData));
        if(!clientData){
            logexit("malloc");
        }
        idCount++;
        clientData->idCount = idCount;
        clientData->clientSock = clientSock;
        memcpy(&(clientData->storage),&storage,sizeof(storage));
        copyMatrix(boardClientUnrevealed, clientData->board);

        pthread_t tid;
        pthread_create(&tid,NULL,clientThread,clientData);

    }

    exit(EXIT_SUCCESS);
    return 0;
}