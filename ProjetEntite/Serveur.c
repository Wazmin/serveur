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
#include "Fonctions.h"
#define PORT 23
#define TAILLE_BUFFER 32

typedef enum
{
	False = 0, True = 1
}
Bool;



int main(void)
{

#if defined (WIN32)
	WSADATA WSAData;
	int erreur = WSAStartup(MAKEWORD(2, 2), &WSAData);
#else
	int erreur = 0;
#endif

	/* Socket et contexte d'adressage du serveur */
	SOCKADDR_IN sin;
	SOCKET sock;
	socklen_t recsize = sizeof(sin);

	/* Socket et contexte d'adressage du client */
	SOCKADDR_IN csin;
	SOCKET csock;
	socklen_t crecsize = sizeof(csin);

	int sock_err;
	char buffer[TAILLE_BUFFER] = "Bonjour !";


	if (!erreur)
	{
		/* Cr�ation d'une socket */
		sock = socket(AF_INET, SOCK_STREAM, 0);

		/* Si la socket est valide */
		if (sock != INVALID_SOCKET)
		{
			printf("La socket %d est maintenant ouverte en mode TCP/IP\n", sock);

			/* Configuration */
			sin.sin_addr.s_addr = htonl(INADDR_ANY);  /* Adresse IP automatique */
			sin.sin_family = AF_INET;                 /* Protocole familial (IP) */
			sin.sin_port = htons(PORT);               /* Listage du port */
			sock_err = bind(sock, (SOCKADDR*)&sin, recsize);

			/* Si la socket fonctionne */
			if (sock_err != SOCKET_ERROR)
			{
				/* D�marrage du listage (mode server) */
				sock_err = listen(sock, 5);
				printf("Listage du port %d...\n", PORT);

				/* Si la socket fonctionne */
				if (sock_err != SOCKET_ERROR)
				{
					/* Attente pendant laquelle le client se connecte */
					Bool tchatActive = True;
					printf("Patientez pendant que le client se connecte sur le port %d...\n", PORT);
					csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
					printf("Un client se connecte avec la socket %d de %s:%d\n", csock, inet_ntoa(csin.sin_addr), htons(csin.sin_port));
					if (csock != SOCKET_ERROR) {
						int sock_err = 1;
						while (tchatActive && sock_err != SOCKET_ERROR) {
							sock_err = send(csock, buffer, TAILLE_BUFFER, 0);
							printf("saisir texte : \n");
							fgets(buffer, TAILLE_BUFFER, stdin);
						}
					}

				}
				else
					perror("listen");
			}
			else
				perror("bind");

			/* Fermeture de la socket client et de la socket serveur */
			printf("Fermeture de la socket client\n");
			closesocket(csock);
			printf("Fermeture de la socket serveur\n");
			closesocket(sock);
			printf("Fermeture du serveur termin�e\n");
		}
		else
			perror("socket");

#if defined (WIN32)
		WSACleanup();
#endif
	}

	return EXIT_SUCCESS;
}