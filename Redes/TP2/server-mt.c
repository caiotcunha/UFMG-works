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

int threadsCount = 0;
pthread_t *threads[10];

int idsClients[11];

typedef struct postTopic{
    char content[1000];
    struct postTopic *nextPost;
}postTopic;

typedef struct Topic{
    char title[100];
    char content[1000];
    int idsSubscribed[11];
    struct postTopic *posts;
    struct Topic *nextTopic;
}Topic;

Topic *topics = NULL;

void usage(int argc, char *argv[])
{
    exit(EXIT_FAILURE);
};

void printTopics(){
    Topic *aux = topics;
    while(aux != NULL){
        printf("Title: %s\n",aux->title);
        printf("Content: %s\n",aux->content);
        postTopic *auxPost = aux->posts;
        while(auxPost != NULL){
            printf("Post: %s\n",auxPost->content);
            auxPost = auxPost->nextPost;
        }
        for(int i = 1; i < 11;i++){
            if(aux->idsSubscribed[i] == 1){
                printf("Client %02d subscribed\n",i);
            }
        }
        aux = aux->nextTopic;
    }
};

Topic *findTopic(char *title){
    Topic *aux = topics;
    while(aux != NULL){
        if(strcmp(aux->title,title) == 0){
            return aux;
        }
        aux = aux->nextTopic;
    }
    return NULL;
};


struct clientData{
    int clientSock;
    int clientId;
    struct sockaddr_storage storage;
};

void * clientThread(void *data){
    struct clientData *clientData = (struct clientData *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&clientData->storage);
    int clientId = clientData->clientId;

        // loop do servidor
        while (1)
        {

            struct BlogOperation operation;
            Topic *topic = NULL;
            recv(clientData->clientSock, &operation, sizeof(struct BlogOperation), 0);

            if (operation.operation_type == DESCONNECT)
            {
                close(clientData->clientSock);
                printf("client disconnected\n");
                break;
            }
            // decide a ação a ser tomada
            switch (operation.operation_type)
            {
            case NEW_CONNECTION:
                printf("client %02d connected\n",clientId);
                operation.client_id = clientId;
                operation.operation_type = 1;
                operation.server_response = 1;
                strcpy(operation.topic, "");
                strcpy(operation.content, "");
                send(clientData->clientSock, &operation, sizeof(struct BlogOperation), 0);
                break;
            case SUBSCRIBE_TOPIC:
                topic = findTopic(operation.topic);

                if(topic == NULL){
                    Topic *aux = topics;
                    while(aux->nextTopic != NULL){
                        aux = aux->nextTopic;
                    }
                    
                    Topic *newTopic = malloc(sizeof(Topic));
                    strcpy(newTopic->title,operation.topic);
                    strcpy(newTopic->content,"");
                    newTopic->nextTopic = NULL;
                    newTopic->posts = NULL;
                    newTopic->idsSubscribed[clientId] = 1;
                    aux->nextTopic = newTopic;
                }
                else{
                    if(topic->idsSubscribed[clientId] == 0){
                        topic->idsSubscribed[clientId] = 1;
                    }
                    else{
                        printf("error: already subscribed");
                    }
                }

                printf("client %02d subscribed to %s\n",clientId,operation.topic);
                break;
            case UNSUBSCRIBE_TOPIC:
                topic = findTopic(operation.topic);

                if(topic == NULL){
                    printf("error: topic not found");
                    continue;
                }
                else{
                    if(topic->idsSubscribed[clientId] == 1){
                        topic->idsSubscribed[clientId] = 0;
                        printf("client %02d unsubscribed to %s\n",clientId,operation.topic);
                    }
                    else{
                        printf("error: not subscribed");
                    }
                }
                break;
            
            }
            //printTopics();
        }
    threadsCount--;
    close(clientData->clientSock);
    pthread_exit(EXIT_SUCCESS);

};



int main(int argc, char *argv[])
{
    topics = malloc(sizeof(Topic));
    strcpy(topics->title,"");
    strcpy(topics->content,"");
    topics->nextTopic = NULL;
    topics->posts = NULL;

    for(int i = 1; i < 11;i++){
        idsClients[i] = 0;
    }
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
        int clientId = 0;
        for(int i = 1; i < 11;i++){
            if(idsClients[i] == 0){
                idsClients[i] = 1;
                clientId = i;
                break;
            }
        };

        clientData->clientId = clientId;
        clientData->clientSock = clientSock;
        memcpy(&(clientData->storage),&storage,sizeof(storage));
        if ( threadsCount == 10){
            printf("Max clients reached\n");
            continue;
        }
        pthread_t *tid  = malloc(sizeof(pthread_t));;
        threads[threadsCount] = tid;
        threadsCount++;
        pthread_create(tid,NULL,clientThread,clientData);
    }

    exit(EXIT_SUCCESS);
    return 0;
}