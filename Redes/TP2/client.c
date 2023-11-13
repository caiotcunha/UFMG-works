#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

void usage(int argc, char *argv[])
{
    exit(EXIT_FAILURE);
}

// função que checa erros
int checkError(struct action msg)
{
    if (msg.type == 0 || msg.type == 7)
    {
        return 0;
    }
    if (msg.coordinates[0] < 0 || msg.coordinates[0] >= MAX_ROWS || msg.coordinates[1] < 0 || msg.coordinates[1] >= MAX_COLS)
    {
        printf("error: invalid cell\n");
        return 1;
    }
    if (msg.type == -1)
    {
        printf("error: command not found\n");
        return 1;
    }
    if (msg.board[msg.coordinates[0]][msg.coordinates[1]] != HIDDEN_CELL && msg.type == 1)
    {
        printf("error: cell already revealed\n");
        return 1;
    }
    if (msg.board[msg.coordinates[0]][msg.coordinates[1]] == FLAGGED && msg.type == 2)
    {
        printf("error: cell already has a flag\n");
        return 1;
    }
    if (msg.board[msg.coordinates[0]][msg.coordinates[1]] != HIDDEN_CELL && msg.type == 2)
    {
        printf("error: cannot insert flag in revealed cell\n");
        return 1;
    }
    if (msg.board[msg.coordinates[0]][msg.coordinates[1]] != FLAGGED && msg.type == 4)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    // criação do socket
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
    // criando a conexão
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (connect(s, addr, sizeof(storage)) != 0)
    {
        logexit("connect");
    }
    int id = 0;
    struct BlogOperation operation;
    operation.client_id = 0;
    operation.operation_type = 1;
    operation.server_response = 0;
    strcpy(operation.topic, "");
    strcpy(operation.content, "");

    // envia a mensagem com a struct BlogOperation para a criação do user
    send(s, &operation, sizeof(struct BlogOperation), 0);

    recv(s, &operation, sizeof(struct BlogOperation), 0);

    if( operation.server_response == 1 ){
        id = operation.client_id;
    }

    printf("id: %d\n", id);


    //  estruturas para auxiliar a leitura e enviar a mensagem
    char buf[BUFSZ];
    char keyword[BUFSZ];
    char topic[BUFSZ];
    char post[BUFSZ];


    while (1)
    {
        memset(buf, 0, BUFSZ);
        fgets(buf, sizeof(buf), stdin);
        int result = sscanf(buf, "%s %s %s", keyword, topic, post);

        if( result == 1 ){
            break;
        }


        if( result == 3 ){
            printf("3");
        }

        if( result == 2 ){
            printf("2");
        }

        // envia a mensagem com a struct action
        //send(s, &operation, sizeof(struct action), 0);

        //recv(s, &operation, sizeof(struct action), 0);

        memset(buf, 0, BUFSZ);
    }

    exit(EXIT_SUCCESS);
    return 0;
}