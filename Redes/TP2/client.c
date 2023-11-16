#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

#include <pthread.h>

// typedef struct readInputClientData{
//     int s;
//     struct BlogOperation *operation;
// }readInputClientData;

// void * readInputClientThread(void *data){
//     struct readInputClientData *clientData = (struct readInputClientData *)data;

//         // loop do servidor
//         while (1)
//         {
//         }
//     pthread_exit(EXIT_SUCCESS);

// };

void usage(int argc, char *argv[])
{
    exit(EXIT_FAILURE);
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
    else{
        printf("error creating user\n");
        exit(EXIT_FAILURE);
    }
    // pthread_t *tid  = malloc(sizeof(pthread_t));
    // pthread_create(tid,NULL,clientThread,clientData);
    // cria a thread de leitura
    // struct clientData *clientData = malloc(sizeof (*clientData));
    // clientData->s = s;
    // clientData->operation = &operation;
    // pthread_t *readThread  = malloc(sizeof(pthread_t));
    // pthread_create(readThread,NULL,readThread,clientData);

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
            if(strcmp(keyword,"exit") == 0){
                operation.client_id = id;
                operation.operation_type = DESCONNECT;
                operation.server_response = 0;
                strcpy(operation.topic, "");
                strcpy(operation.content, "");
                send(s, &operation, sizeof(struct BlogOperation), 0);
                break;
            }
            else{
                printf("invalid command\n");
            }
            break;
        }

        if( result == 2 ){
            if(strcmp(keyword,"list") == 0 && strcmp(topic,"topics") == 0 ){
                operation.client_id = id;
                operation.operation_type = LIST_TOPICS;
                operation.server_response = 0;
                strcpy(operation.topic, "");
                strcpy(operation.content, "");
                send(s, &operation, sizeof(struct BlogOperation), 0);
                recv(s, &operation, sizeof(struct BlogOperation), 0);
                printf("%s\n", operation.content);
            }
            else if(strcmp(keyword,"subscribe") == 0){
                operation.client_id = id;
                operation.operation_type = SUBSCRIBE_TOPIC;
                operation.server_response = 0;
                strcpy(operation.topic, topic);
                strcpy(operation.content, "");
                send(s, &operation, sizeof(struct BlogOperation), 0);
            }
            else if(strcmp(keyword,"unsubscribe") == 0){
                operation.client_id = id;
                operation.operation_type = UNSUBSCRIBE_TOPIC;
                operation.server_response = 0;
                strcpy(operation.topic, topic);
                strcpy(operation.content, "");
                send(s, &operation, sizeof(struct BlogOperation), 0);
            }
            else{
                printf("invalid command\n");
            }
        }

        if( result == 3 ){
            if(strcmp(keyword,"publish") == 0 && strcmp(topic,"in")){
                operation.client_id = id;
                operation.operation_type = NEW_TOPIC_POST;
                operation.server_response = 0;
                strcpy(operation.topic, post);
                memset(buf, 0, BUFSZ);
                fgets(buf, sizeof(buf), stdin);
                strcpy(operation.content, buf);
                send(s, &operation, sizeof(struct BlogOperation), 0);
            }
            else{
                printf("invalid command\n");
            }
        }

        memset(buf, 0, BUFSZ);
    }

    exit(EXIT_SUCCESS);
    return 0;
}