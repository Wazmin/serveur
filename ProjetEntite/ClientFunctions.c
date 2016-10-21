#include "ClientFunctions.h"

//initialisation des socket sous systeme windows
void init(void)
{
#ifdef WIN32
	WSADATA wsa;
	int err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err < 0)
	{
		puts("WSAStartup failed !");
		exit(EXIT_FAILURE);
	}
#endif
}


//fin d'utilisation des sockets
void end(void) {
	#ifdef WIN32
		WSACleanup();
	#endif
}

// entree de l'adresse de connexion au serveur
// avec setup du pseudo sur le tchat
void app(const char *address, const char *name) {
	SOCKET sock = init_connection(address);
	char buffer[BUF_SIZE];
	char entreeClavier[256];

	fd_set rdfs;

	/* send our name */
	write_server(sock, name);

	while (1)
	{
		FD_ZERO(&rdfs);

		/* add STDIN_FILENO */
		//FD_SET(entreeClavier, &rdfs);

		/* add the socket */
		FD_SET(sock, &rdfs);
		
		if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
		{
			perror("select()");
			exit(errno);
		}

		/* something from standard input : i.e keyboard */
		if (strlen(buffer)<=0)
		{
			fgets(buffer, BUF_SIZE - 1, stdin);
			{
				char *p = NULL;
				p = strstr(buffer, "\n");
				if (p != NULL)
				{
					*p = 0;
				}
				else
				{
					/* fclean */
					buffer[BUF_SIZE - 1] = 0;
				}
			}
			write_server(sock, buffer);
		}
		else if (FD_ISSET(sock, &rdfs))
		{
			int n = read_server(sock, buffer);
			/* server down */
			if (n == 0)
			{
				printf("Deconnexion serveur !\n");
				break;
			}
			puts(buffer);
		}
	}

	end_connection(sock);
}

//ouverture du socket + connection
// recuperation par host name
int init_connection(const char *address) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sin = { 0 };
	struct hostent *hostinfo;

	if (sock == INVALID_SOCKET)
	{
		perror("socket()");
		exit(errno);
	}

	//hostinfo = gethostbyname(address);
	//if (hostinfo == NULL)
	//{
	//	fprintf(stderr, "host inconnu %s.\n", address);
	//	exit(EXIT_FAILURE);
	//}

	//sin.sin_addr = *(IN_ADDR *)hostinfo->h_addr;
	//sin.sin_port = htons(PORT);
	//sin.sin_family = AF_INET;

	sin.sin_addr.s_addr = inet_addr(address);
	sin.sin_port = htons(PORT);
	sin.sin_family = AF_INET;

	if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		perror("connect()");
		exit(errno);
	}

	return sock;
}

//fermeture de la socket
void end_connection(int sock) {
	closesocket(sock);
}

// lecture message provenant du serveur
int read_server(SOCKET sock, char *buffer) {
	int n = 0;

	if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
	{
		perror("recv()");
		exit(errno);
	}

	buffer[n] = 0;

	return n;
}

//ecrire au serveur
void write_server(SOCKET sock, const char *buffer) {
	if (send(sock, buffer, strlen(buffer), 0) < 0)
	{
		perror("send()");
		exit(errno);
	}
}