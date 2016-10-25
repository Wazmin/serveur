#ifndef SERVEUR_H
#define SERVEUR_H
#define HAVE_STRUCT_TIMESPEC
#include <winsock2.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "Entite.h"

//struct thread_param {
//	Serveur * ser;
//	SOCKET soc;
//};

class Serveur {
private:
	WSAData WSAData;
	int numPort;
	bool enMarche;
	SOCKET mySocket;
	SOCKET clientSocket;
	SOCKADDR_IN sin;
	SOCKADDR_IN cin;
	// Threads
	pthread_t threadServeur;
	pthread_mutex_t mutex_coord;
	std::vector<SOCKET> vecSocketClient;
	std::vector<pthread_t> vecThreadClient;
	//partie entite
	Entite Orthos;
	//timer
	const int timerSendCoord = 1000;
	//format de communication
	const std::string MSG_COORD = "01-";
	const std::string MSG_MOOD = "02-";
	const std::string MSG_FILE_TRANSFERT = "03-";
public:
	Serveur();
	int Initialiser(int numPort = 1234);
	int Lancer();
	int LancerThreadServeurCoord();
	void ArreterServeur();
	static void*  callThreadServeur(void *arg) { return ((Serveur*)arg)->ThreadServeurCoord(); }
	void Receptionniste(std::string messageEntrant);
private:
	void * ThreadServeurCoord();
	void * LancerThreadClient(void * p_data);
};

#endif