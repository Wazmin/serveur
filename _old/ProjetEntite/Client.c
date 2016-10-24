#if defined (WIN32)
#include <winsock2.h>
typedef int socklen_t;
#elif defined (linux)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ClientFunctions.h"

typedef enum
{
	False = 0, True = 1
}
Bool;

int main(void)
{
	char adresseServeur[32], pseudo[32];
	fprintf(stdout, "saisir l'adresse IP du serveur : ");
	fgets(adresseServeur, 32, stdin);
	fprintf(stdout,"saisir votre pseudo :");
	fgets(pseudo, 32, stdin);

	init();
	app(adresseServeur, pseudo);
	end();

	return EXIT_SUCCESS;
}