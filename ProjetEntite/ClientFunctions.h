#pragma once
#ifndef CLIENTFUNCTIONS_H
#define CLIENTFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32

#include <winsock2.h>
#ifdef _WIN32
#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#endif
#endif

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF	 "\r\n"
#define PORT	 1977
#define BUF_SIZE 1024

void init(void);
void end(void);
void app(const char *address, const char *name);
int init_connection(const char *address);
void end_connection(int sock);
int read_server(SOCKET sock, char *buffer);
void write_server(SOCKET sock, const char *buffer);

#endif /* guard */
