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
			std::cout << "arrivee d'un client " << WSAGetLastError() << std::endl;
		}
		vecSocketClient.push_back(clientSocket);

		//preparation d'envoi au thread
		
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
	return typeMsgDetected.compare(MsgTypeString[msgType]);
}

// tous les messages envoyé par le client passeront
// d'abord par ici
void Serveur::Receptionniste(std::string &str, const SOCKET &theClientSocket, char * buffer) {


	//Protocole 1 == demande de recontre à l'entité
	if (Triage(str, CAN_I_MEET_ORTHOS)) {
		// msg like 05-IMEI
		std::string data1 = str.substr(3);//retrait entete
		if (mesDonnees.CanMeetOrthos(_atoi64(data1.c_str()))) {
			data1 = MsgTypeString[REPONSE_MEETING] + 'y';
		}
		else {
			data1 = MsgTypeString[REPONSE_MEETING] + 'n';	
		}
		SendMessageToClient(theClientSocket, data1, buffer);
	}
	//Protocole 2 == reception d'un souvenir du Client
	else if (Triage(str, CLIENT_SEND_SOUVENIR)) {
		// msg like 06-1546852-i
		std::string infoTaille = str.substr(3);//retrait entete
		std::string typeFichier = infoTaille.substr(infoTaille.find('-')+1);
		infoTaille = infoTaille.substr(0,infoTaille.length()-2);
		// recuperation de la taille
		int tailleMsg = std::stoi(infoTaille);
		RecvFileFromClient(theClientSocket,tailleMsg,typeFichier[0]);
	}
}

void Serveur::SendMessageToClient(const SOCKET &clientSocket, const std::string &msg, char * buffer) {
	int tailleMsg= msg.length() + 1;
	buffer = new char[tailleMsg];
	strncpy_s(buffer, tailleMsg, msg.c_str(), tailleMsg);
	send(clientSocket, buffer, tailleMsg, 0);
	delete[] buffer;
}

void Serveur::RecvFileFromClient(const SOCKET &clientSocket, int tailleFichier, char type) {
	std::string nomFichier;
	SouvenirData sd;
	if (type == 'i') {
		mesDonnees.nbImages++;
		nomFichier = "IMG" + mesDonnees.nbImages;
		nomFichier += ".jpg";
		sd.type = TYPE_SOUVENIR_IMAGE;
		sd.phrase = "n";
	}
	else if(type == 't'){
		mesDonnees.nbTextes++;
		nomFichier = "TXT" + mesDonnees.nbTextes;
		nomFichier += ".txt";
		sd.type = TYPE_SOUVENIR_TEXTE;
	}
	sd.nomDuFichier = nomFichier;
	sd.dateTimeInSecond = std::time(nullptr);

	if (true) {//sd.type == TYPE_SOUVENIR_IMAGE
		// cas d'un fichier image
		std::ofstream fichierSouvenir(mesDonnees.nomRepSouvenirData + nomFichier, std::ofstream::binary);
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
		fichierSouvenir.close();// fermeture du fichier
		if (!error) {
			mesDonnees.AddSouvenir(sd);
		}
		else {
			if (sd.type == TYPE_SOUVENIR_IMAGE) mesDonnees.nbImages--;
			else mesDonnees.nbTextes--;
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
}

// thread du serveur pour envois des coordonnées
// utilisation d'un timer pour la frequence d'envoi
void * Serveur::ThreadServeurCoord() {
	// creation d'un timer
	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point cran = tp + std::chrono::milliseconds(timerSendCoord);

	// besoin pour le message
	std::string message; 
	size_t size; 
	char *buffer;
	int len;

	while (enMarche) {
		tp = std::chrono::system_clock::now();

		if (tp >= cran) {
			cran = tp + std::chrono::milliseconds(timerSendCoord);
			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			// mutex pour lire une bonne donnée
			pthread_mutex_lock(& mutex_coord);
				message = Orthos.GetSDCoord();
				size = message.size() + 1;
				buffer = new char[size];
				strncpy_s(buffer, size, message.c_str(), size);
				len = strlen(buffer);
			pthread_mutex_unlock(&mutex_coord);
			pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
			for (const auto &sockClient : vecSocketClient) {
				//envoyer les coordonnees
				send(sockClient, buffer, len, 0);
			}
			delete[] buffer;
		}	
	}
	return NULL;
}

// Thread lancé pour chaque client des lors qu'ils se connectent
void * Serveur::LancerThreadClient(void * p_data) {
	SOCKET thisClientSocket;
	*
	// boucle principale
	while (enMarche) {

	}
	return NULL;
}