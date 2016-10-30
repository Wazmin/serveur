#ifndef SERVEUR_H
#define SERVEUR_H
#define HAVE_STRUCT_TIMESPEC


#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <ctime>
#include <pthread.h>
#include <vector>
#include <cmath>
#include <mutex>

#include "Entite.h"
#include "GestionDonnees.h"

#define MAX_MSG_SIZE 1024

static pthread_mutex_t mutex_vecSockCli = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_coord = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_lectureEcritureFichier = PTHREAD_MUTEX_INITIALIZER;
class Serveur;
enum MessageType {
	CLIENT_ASK_COORD,
	STRUCT_SOUVENIR,
	SEND_COOR_TO_CLIENT,//03-
	CLIENT_ASK_MEET_ORTHOS,//04-
	REPONSE_MEETING_TO_CLIENT,//05-
	CLIENT_SENT_SOUVENIR,//06-
	SEND_SOUVENIR_TO_CLIENT,//07-
	SEND_NEW_NAME_SOUVENIR_TO_CLIENT,//08-

};
static const char* MsgTypeString[] = {
	"01-",
	"02-",
	"03-",
	"04-",
	"05-",
	"06-",
	"07-",
	"08-"
};

struct ToThreadArg {
	SOCKET sock;
	Serveur *server;
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
	//partie entite
	Entite Orthos;
	//timer
	const int timerSendCoord = 1;
	//partie gestion de donnees
	GestionDonnees mesDonnees;
public:
	std::vector<SOCKET> vecSocketClient;
	std::vector<pthread_t> vecThreadClient;
	std::vector<ToThreadArg> vecToThreadArg;
	
public:
	Serveur();
	int Initialiser(int numPort = 1234);
	int Lancer();
	int LancerThreadServeurCoord();
	int LancerThreadClient(ToThreadArg &tta);
	void ArreterServeur();
	bool Triage(const std::string &str, const MessageType &msgType);
	void Receptionniste(char *incomingMsg, const SOCKET &theClientSocket,Serveur * serv);
public:
	static void*  callThreadServeur(void *arg) { return ((Serveur*)arg)->ThreadServeurCoord(); }
	static void*  callThreadClient(void *arg) { return ((Serveur*)arg)->ThreadClient(arg); }
private:
	void * ThreadServeurCoord();
	void * ThreadClient(void * p_data);
	void SendMessageToClient(const SOCKET &clientSocket, const std::string &msg);
	void SendFileToClient(const SOCKET &clientSocket, Serveur * serv, SouvenirData &sd);
	bool SendData(SOCKET sock, void *buf, int buflen);
	void RecvFileFromClient(const SOCKET &clientSocket, int tailleFichier, Serveur * serv, std::string &nomFichier);
	bool ReadData(SOCKET sock, void *buf, int tailleFichier);
	void split(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> split(const std::string &s, char delim);
};

#endif