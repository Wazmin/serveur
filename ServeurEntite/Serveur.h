#ifndef SERVEUR_H
#define SERVEUR_H
#define HAVE_STRUCT_TIMESPEC


#include <winsock2.h>
#include <iostream>
#include <cstring>
#include <chrono>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <cmath>
#include "Entite.h"
#include "GestionDonnees.h"

enum MessageType {
	CAN_I_MEET_ORTHOS,//04-
	REPONSE_MEETING,//05-
	CLIENT_SEND_SOUVENIR,//06-
	SERVER_SEND_SOUVENIR,//07-
	SERVER_SEND_NEW_NAME_SOUVENIR,//08-

};
static const char* MsgTypeString[] = {
	"04-",
	"05-",
	"06-",
	"07-",
	"08-"
};

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
	//partie gestion de donnees
	GestionDonnees mesDonnees;
public:
	Serveur();
	int Initialiser(int numPort = 1234);
	int Lancer();
	int LancerThreadServeurCoord();
	void ArreterServeur();
	bool Triage(const std::string &str, const MessageType &msgType);
	void Receptionniste(std::string &str, const SOCKET &theClientSocket, char * buffer);
	static void*  callThreadServeur(void *arg) { return ((Serveur*)arg)->ThreadServeurCoord(); }
private:
	void * ThreadServeurCoord();
	void * LancerThreadClient(void * p_data);
	void SendMessageToClient(const SOCKET &clientSocket, const std::string &msg, char * buffer);
	void RecvFileFromClient(const SOCKET &clientSocket, int tailleFichier, char type);
	bool ReadData(SOCKET sock, void *buf, int tailleFichier);
};

#endif