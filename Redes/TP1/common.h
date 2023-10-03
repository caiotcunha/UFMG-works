#pragma once

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>

#define MAX_ROWS 4
#define MAX_COLS 4

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int serverSockaddrInit(const char *proto, const char *portstr, struct sockaddr_storage *storage);

void logexit(const char *str);

int transformActionStringInInt(char *type);

char transformIntInChar(int intRepresentation);

void printClientBoard(int board[MAX_ROWS][MAX_COLS]);