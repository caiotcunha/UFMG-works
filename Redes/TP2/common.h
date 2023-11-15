#pragma once

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>

// actions
#define NEW_CONNECTION 1
#define NEW_TOPIC_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE_TOPIC 4
#define DESCONNECT 5
#define UNSUBSCRIBE_TOPIC 6

struct BlogOperation {
int client_id;
int operation_type;
int server_response;
char topic[50];
char content[2048];
};

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

int serverSockaddrInit(const char *proto, const char *portstr, struct sockaddr_storage *storage);

void logexit(const char *str);
