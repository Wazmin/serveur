#include "Serveur.h"

Serveur::Serveur(){}

// initialisation du serveur
int Serveur::Initialiser(int numPort) {
	// port du serveur
	this->numPort = numPort;
	this->enMarche = false;
	if (WSAStartup(MAKEWORD(2, 0), &this->WSAData) != 0 ) {
		std::cerr << "Erreur Initialiser() - WSAStartup" << std::endl;
		return 1;
	}
	// renseignement sur la connexion serveur
	this->mySocket = socket(AF_INET, SOCK_STREAM, 0);
	this->sin.sin_addr.s_addr = INADDR_ANY;
	this->sin.sin_family = AF_INET;
	this->sin.sin_port = htons(numPort);

	//pthread_mutex_init(&mutex_coord,NULL);
	mutex_coord = PTHREAD_MUTEX_INITIALIZER;
	Orthos.tmpInit();
	mesDonnees.LoadRessources();

	return 0;
}

// creation de la socket, bind puis ecoute
int Serveur::Lancer() {
	
	if ((mySocket = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET) {
		std::cerr << "Erreur Lancer() - socket() : "<< WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	if (bind(mySocket,(SOCKADDR *) &sin, sizeof(sin)) == SOCKET_ERROR) {
		std::cerr << "Erreur Lancer() - bind() : " << WSAGetLastError() << std::endl;
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}

	// Mise en ecoute du serveur
	if (listen(mySocket,5) == SOCKET_ERROR) {
		std::cerr << "listen a échoué avec l'erreur " << WSAGetLastError() << std::endl;
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Le serveur est lance, en ecoute sur le port :" << numPort << std::endl;
	enMarche = true;
	LancerThreadServeurCoord();
	cin = { 0 };
	int cinLen = sizeof(cin);

	//boucle principale du serveur
	while (enMarche) {
		// reception d'une connexion client
		if ((clientSocket = accept(mySocket, (SOCKADDR *)&cin, &cinLen)) == INVALID_SOCKET) {
			std::cerr << "accept a echoue avec l'erreur " << WSAGetLastError() << std::endl;
			closesocket(mySocket);
			WSACleanup();
			return 1;
		}
		else {
			std::cout << "arrivee d'un client " << std::endl;
			vecSocketClient.push_back(clientSocket);
			std::cout << "ajout de son socket aux clients du serveur " << std::endl;
			// creation du thread client
			ToThreadArg tta;
			tta.sock = clientSocket;
			tta.server = this;
			vecToThreadArg.push_back(tta);
			LancerThreadClient(vecToThreadArg[vecToThreadArg.size()-1]);//clientSocket
			std::cout << "lancement de son thread effectue " << std::endl;
		}
		
		
		
	}

}

// fonction qui lance le thread serveur pour l'envoi des coordonnees
int Serveur::LancerThreadServeurCoord() {

	if (pthread_create(&threadServeur, NULL, Serveur::callThreadServeur, this) != 0) {
		//impossible de lancer le thread serveur !
		std::cerr << "impossible de lancer le thread Serveur !" << std::endl;
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

// Arret du serveur
void Serveur::ArreterServeur() {

	//arret thread serveur 
	pthread_cancel(threadServeur);
	pthread_join(threadServeur, NULL);
	enMarche = false;
}

// recoit une data provenant d'un client et compare au type souhaitez
// renvoie vrai si cela correspond au type attendu
// passage par reference
bool Serveur::Triage(const std::string &str, const MessageType &msgType) {
	std::string typeMsgDetected;
	typeMsgDetected = str.substr(0, 3);
	return typeMsgDetected == MsgTypeString[msgType];
}

// tous les messages envoyé par le client passeront
// d'abord par ici
void Serveur::Receptionniste(char *incomingMsg, const SOCKET &theClientSocket, Serveur * serv) {
	std::string str = incomingMsg;

	//Protocole 1 == demande de recontre à l'entité
	if (Triage(str, CAN_I_MEET_ORTHOS)) {
		// msg like 05-IMEI
		std::string data1 = str.substr(3);//retrait entete
		if (serv->mesDonnees.CanMeetOrthos(_atoi64(data1.c_str()))) {
			data1 = MsgTypeString[REPONSE_MEETING] + 'y';
		}
		else {
			data1 = MsgTypeString[REPONSE_MEETING] + 'n';	
		}
		SendMessageToClient(theClientSocket, data1);
	}
	//Protocole 2 == reception d'un souvenir du Client
	else if (Triage(str, CLIENT_SEND_SOUVENIR)) {
		// msg like 06-1546852-i
		std::string infoTaille = str.substr(3);//retrait entete
		std::string typeFichier = infoTaille.substr(infoTaille.find('-')+1);
		infoTaille = infoTaille.substr(0,infoTaille.length()-2);
		// recuperation de la taille
		int tailleMsg = std::stoi(infoTaille);
		RecvFileFromClient(theClientSocket,tailleMsg,typeFichier[0], serv);
	}
}

void Serveur::SendMessageToClient(const SOCKET &clientSocket, const std::string &msg) {
	int tailleMsg= msg.length() + 1;
	char *buffer = new char[tailleMsg];
	strncpy_s(buffer, tailleMsg, msg.c_str(), tailleMsg);
	send(clientSocket, buffer, tailleMsg, 0);
	delete[] buffer;
}

void Serveur::RecvFileFromClient(const SOCKET &clientSocket, int tailleFichier, char type, Serveur * serv) {
	std::string nomFichier;
	SouvenirData sd;
	if (type == 'i') {
		serv->mesDonnees.nbImages += 1;
		nomFichier = "IMG";
		nomFichier += std::to_string(serv->mesDonnees.nbImages);
		nomFichier += ".jpg";
		sd.type = TYPE_SOUVENIR_IMAGE;
		sd.phrase = "n";
	}
	else if(type == 't'){
		serv->mesDonnees.nbTextes += 1;
		nomFichier = "TXT";
		nomFichier += std::to_string(serv->mesDonnees.nbTextes);
		nomFichier += ".txt";
		sd.type = TYPE_SOUVENIR_TEXTE;
	}
	sd.nomDuFichier = nomFichier;
	sd.dateTimeInSecond = std::time(nullptr);

	if (true) {//sd.type == TYPE_SOUVENIR_IMAGE
		// cas d'un fichier image
		std::cout << "Reception d'un fichier en cours" << std::endl;
		std::ofstream fichierSouvenir(serv->mesDonnees.nomRepSouvenirData + nomFichier, std::ofstream::binary);
		int actualFileSize = 0;
		bool error = false;
		if (fichierSouvenir) {
			if (tailleFichier > 0) {
				char buffer[1024];
				do {
					int num = min(tailleFichier, sizeof(buffer));
					if (!ReadData(clientSocket, buffer, num)) {
						error = true;
					}
					else {
						int offset = 0;
						do {
							try {
								fichierSouvenir.write(buffer, num - offset);
							}
							catch (const std::exception & e) {
								std::cerr << e.what();
								error = true;
								fichierSouvenir.close();
							}
							offset += num;
						} while (offset < num && !error);
						tailleFichier -= num;
					}
				} while (tailleFichier > 0 && !error);
			}
		}
		else {
			error = true;
		}
		fichierSouvenir.close();// fermeture du fichier
		if (!error) {
			serv->mesDonnees.AddSouvenir(sd);
			std::cout << "Reception terminee : "<<nomFichier << std::endl;
		}
		else {
			if (sd.type == TYPE_SOUVENIR_IMAGE)serv->mesDonnees.nbImages -= 1;
			else serv->mesDonnees.nbTextes -= 1;
		}
	}	
}

// sous fonction ReadData
bool Serveur::ReadData(SOCKET sock, void *buf, int buflen) {
	unsigned char *pbuf = (unsigned char *) buf;
	while (buflen > 0) {
		int num = recv(sock,(char *) pbuf, buflen, 0);
		if (num == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				continue;
			}
			return false;
		}
		else if (num == 0) {
			return false;
		}
		pbuf += num;
		buflen -= num;
	}
	return true;
}

// thread du serveur pour envois des coordonnées
// utilisation d'un timer pour la frequence d'envoi
void * Serveur::ThreadServeurCoord() {
	// creation d'un timer
	int tp = std::time(nullptr);
	int cran = tp + timerSendCoord;

	// besoin pour le message
	std::string message; 
	size_t size; 
	char *buffer;
	int len;

	while (enMarche) {
		tp = std::time(nullptr);

		if (tp >= cran) {
			cran = tp + timerSendCoord;
			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			// mutex pour lire une bonne donnée
			pthread_mutex_lock(& mutex_coord);
				message = Orthos.GetSDCoord();
				size = message.size() + 1;
				buffer = new char[size];
				strncpy_s(buffer, size, message.c_str(), size);
				len = strlen(buffer);
			pthread_mutex_unlock(&mutex_coord);
			pthread_mutex_lock(&mutex_vecSockCli);
			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
			for (const auto &sockClient : vecSocketClient) {
				//envoyer les coordonnees
				send(sockClient, buffer, len, 0);
			}
			pthread_mutex_unlock(&mutex_vecSockCli);
			delete[] buffer;
		}	
	}
	return NULL;
}

// Thread lancé pour chaque client des lors qu'ils se connectent
void * Serveur::ThreadClient(void *p_data) {
	ToThreadArg *tta = static_cast<ToThreadArg*>(p_data);
	SOCKET thisClientSocket = tta->sock;
	Serveur *serv = tta->server;
	char buffer[MAX_MSG_SIZE];
	bool enFonction = true;
	int num;
	// boucle principale
	while (enFonction) {
		num = recv(thisClientSocket, buffer, MAX_MSG_SIZE - 1, 0);
		if ( num == -1) {
			pthread_mutex_lock(&mutex_vecSockCli);
			std::cerr << "Erreur recv thread client" << std::endl;
			enFonction = false;
			std::vector<SOCKET>::iterator it;
			it = std::find(serv->vecSocketClient.begin(), serv->vecSocketClient.end(), thisClientSocket);
			serv->vecSocketClient.erase(it);
			pthread_mutex_unlock(&mutex_vecSockCli);
		}
		else if (num == 0) {
			std::cout << "Deconnexion d'un client" << std::endl;
			enFonction = false;
		}
		else {
			Receptionniste(buffer, thisClientSocket, serv);
		}

		
	}

	return NULL;
}
int Serveur::LancerThreadClient(ToThreadArg &tta) {
	pthread_t clientThread;
	if (pthread_create(&clientThread, NULL, callThreadClient, &tta) != 0) {
		//impossible de lancer le thread client !
		std::cerr << "impossible de lancer le thread Client !" << std::endl;
		closesocket(mySocket);
		WSACleanup();
		return 1;
	}
	vecThreadClient.push_back(clientThread);
	return 0;
}