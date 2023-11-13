#pragma once

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>

#define MAX_ROWS 4
#define MAX_COLS 4

// actions
#define START 0
#define REVEAL 1
#define FLAG 2
#define STATE 3
#define REMOVE_FLAG 4
#define RESET 5
#define WIN 6
#define EXIT 7
#define GAME_OVER 8

// actions
#define NEW_CONNECTION 1
#define NEW_TOPIC_POST 2
#define LIST_TOPICS 3
#define SUBSCRIBE_TOPIC 4
#define DESCONNECT 5
#define UNSUBSCRIBE_TOPIC 6


// board
#define BOMB -1
#define HIDDEN_CELL -2
#define FLAGGED -3

struct action
{
    int type;
    int coordinates[2];
    int board[4][4];
};

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

int transformActionStringInInt(char *type);

char transformIntInChar(int intRepresentation);

void printClientBoard(int board[MAX_ROWS][MAX_COLS]);