#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSZ 1024
#define WIN_COUNT 13

void usage(int argc, char *argv[])
{
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
}

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
}

void inicializeBoardClientUnrevealed(int boardClientUnrevealed[MAX_ROWS][MAX_COLS])
{
    for (int i = 0; i < MAX_ROWS; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            boardClientUnrevealed[i][j] = HIDDEN_CELL;
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
        int count = 0;                                          // usado para verificar se o jogador ganhou
        inicializeBoardClientUnrevealed(boardClientUnrevealed); // inicializa o tabuleiro não revelado
        struct sockaddr_storage clientStorage;
        struct sockaddr *clientAddr = (struct sockaddr *)(&clientStorage);
        socklen_t clientAddrLen = sizeof(clientStorage);
        int clientSock = accept(s, clientAddr, &clientAddrLen); // accept retorna um socket para a conexão
        printf("client connected\n");
        if (clientSock == -1)
        {
            logexit("accept");
        }
        // loop do jogo
        while (1)
        {

            struct action msg;
            recv(clientSock, &msg, sizeof(struct action), 0);

            if (msg.type == EXIT)
            {
                close(clientSock);
                printf("client disconnected\n");
                break;
            }
            // decide a ação a ser tomada
            switch (msg.type)
            {
            case START:
                copyMatrix(boardClientUnrevealed, msg.board);
                send(clientSock, &msg, sizeof(struct action), 0);
                break;
            case REVEAL:
                if (board[msg.coordinates[0]][msg.coordinates[1]] != BOMB)
                {
                    count++;
                    if (count == WIN_COUNT)
                    {
                        msg.type = WIN;
                        copyMatrix(board, msg.board);
                        send(clientSock, &msg, sizeof(struct action), 0);
                    }
                    msg.type = STATE;
                    boardClientUnrevealed[msg.coordinates[0]][msg.coordinates[1]] = board[msg.coordinates[0]][msg.coordinates[1]];
                    msg.board[msg.coordinates[0]][msg.coordinates[1]] = board[msg.coordinates[0]][msg.coordinates[1]];
                    send(clientSock, &msg, sizeof(struct action), 0);
                }
                else
                {
                    msg.type = GAME_OVER;
                    copyMatrix(board, msg.board);
                    send(clientSock, &msg, sizeof(struct action), 0);
                }
                break;
            case FLAG:
                msg.type = STATE;
                boardClientUnrevealed[msg.coordinates[0]][msg.coordinates[1]] = FLAGGED;
                copyMatrix(boardClientUnrevealed, msg.board);
                send(clientSock, &msg, sizeof(struct action), 0);
                break;
            case REMOVE_FLAG:
                msg.type = STATE;
                boardClientUnrevealed[msg.coordinates[0]][msg.coordinates[1]] = HIDDEN_CELL;
                copyMatrix(boardClientUnrevealed, msg.board);
                send(clientSock, &msg, sizeof(struct action), 0);
                break;
            case RESET:
                printf("starting new game\n");
                inicializeBoardClientUnrevealed(boardClientUnrevealed);
                copyMatrix(boardClientUnrevealed, msg.board);
                send(clientSock, &msg, sizeof(struct action), 0);
                break;
            }
        }
    }

    exit(EXIT_SUCCESS);
    return 0;
}