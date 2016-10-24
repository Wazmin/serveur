#ifndef SERVEUR_H
#define SERVEUR_H

#include <WinSock2.h>
#include <pthread.h>
#include <vector>

struct thread_param {
	Serveur* ser;
	SOCKET soc;
};

class Serveur {
private:
	WSADATA WSAData;
	int numPort;
	bool enMarche;
	SOCKET mySocket;
	SOCKET clientSocket;
	SOCKADDR_IN sin;
	SOCKADDR_IN cin;
	int cinLen;
	pthread_t threadServeur;
	std::vector<int> vecSocketClient;
	std::vector<pthread_t> vecThreadClient;

public:
	Serveur();
	int Initialiser(int numPort = 1234);
	int Lancer();

private:
	void * LancerThreadServeurCoord();
	void * LancerThreadClient();
};

#endif