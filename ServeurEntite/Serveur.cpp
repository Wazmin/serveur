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
	//LancerThreadServeurCoord();
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

	//Protocole 1 == demande les coordonnées d'Orthos
	if (Triage(str, CLIENT_ASK_COORD)) {
		//std::cout << "Un client demande les coordonnees d'Orthos" << std::endl;
		Sleep(100);
		Orthos.SetCanMove(false);
		std::string reponse = MsgTypeString[SEND_COOR_TO_CLIENT];
		pthread_mutex_lock(&mutex_coord);
		reponse += serv->Orthos.GetSDCoord();
		pthread_mutex_unlock(&mutex_coord);
		Orthos.SetCanMove(true);

		SendMessageToClient(theClientSocket,reponse);
	}
	//Protocole 2 == demande de recontre à l'entité
	else if (Triage(str, CLIENT_ASK_MEET_ORTHOS)) {
		// msg like 05-ID
		std::cout << "Un client a demande s'il pouvait rencontrer Orthos" << std::endl;
		std::vector<std::string> subString = split(str, '-');
		unsigned int val = 1;
		if (subString.size() < val ) {
			return;
		}
		std::string reponse = MsgTypeString[REPONSE_MEETING_TO_CLIENT];
		if (serv->mesDonnees.CanMeetOrthos(subString[1])) {
			reponse += "y-";
			std::cout << "Je l'ai autorise" << std::endl;
		}
		else {
			reponse += "n-";
			std::cout << "J'ai rigole et je lui ai dit non" << std::endl;
		}	

		Sleep(100);
		SendMessageToClient(theClientSocket, reponse);
	}
	//Protocole 3 == reception d'un souvenir du Client
	else if (Triage(str, CLIENT_SENT_SOUVENIR)) {
		std::vector<std::string> subString = split(str, '-');
		if (subString.size() <3) {
			return;
		}
		bool isImage;
		SouvenirData sd;
		std::string nomFichierRetourne;

		if (subString[2] == "t") {
			isImage = false;
			sd.type = TYPE_SOUVENIR_TEXTE;
			sd.nomDuFichier = "TXT" + std::to_string(++serv->mesDonnees.nbTextes);
			sd.dateTimeInSecond = std::time(nullptr);
			sd.phrase = subString[3];
			nomFichierRetourne = sd.nomDuFichier;
			serv->mesDonnees.AddSouvenir(sd);
		}
		else {
			isImage = true;
			sd.type = TYPE_SOUVENIR_IMAGE;
			sd.nomDuFichier = "IMG" + std::to_string(++serv->mesDonnees.nbImages) + ".jpg";
			sd.dateTimeInSecond = std::time(nullptr);
			sd.phrase = " ";
			serv->mesDonnees.AddSouvenir(sd);
			int tailleMsg = std::stoi(subString[1]);
			RecvFileFromClient(theClientSocket, tailleMsg, serv, sd.nomDuFichier);
		}

		

		// pause avant envoi du nouveau nom et de la dateTime
		Sleep(100);
		std::string messageRetour = MsgTypeString[SEND_NEW_NAME_SOUVENIR_TO_CLIENT];
		messageRetour += sd.nomDuFichier + "-";
		messageRetour += std::to_string(sd.dateTimeInSecond) + "-";
		std::cout << "Envoi au client le nouveau nom : "<<messageRetour << std::endl;
		SendMessageToClient(theClientSocket, messageRetour);

		// on renvoie un souvenir et d'abors l'entete
		pthread_mutex_lock(&mutex_lectureEcritureFichier);
		SouvenirData sdToSend = serv->mesDonnees.GetSouvenir();
		Sleep(100);
		if(sdToSend.type == TYPE_SOUVENIR_TEXTE) {
			std::string msgNS = MsgTypeString[SEND_SOUVENIR_TO_CLIENT];
			msgNS += "t-";
			msgNS += std::to_string(sdToSend.dateTimeInSecond) + "-";
			msgNS += sdToSend.nomDuFichier + "-";
			msgNS += sdToSend.phrase + "-";
			std::cout << "Envoi au client la phrase: " << msgNS << std::endl;
			SendMessageToClient(theClientSocket, msgNS);
		}
		else {
			SendFileToClient(theClientSocket, serv, sdToSend);
		}

		pthread_mutex_unlock(&mutex_lectureEcritureFichier);
	}
}

//sous fonction pour l'envoi de donnée
void Serveur::SendMessageToClient(const SOCKET &clientSocket, const std::string &msg) {
	
	
	int tailleMsg= msg.length() + 1;
	char *buffer = new char[tailleMsg];
	memset(buffer,0,tailleMsg);
	strncpy_s(buffer, tailleMsg, msg.c_str(), msg.length());
	send(clientSocket, buffer, tailleMsg-1, 0);
	

	//send(clientSocket, msg.c_str(), msg.size(), 0);
	delete[] buffer;
}

// envoi d'un fichier au client ciblé
void Serveur::SendFileToClient(const SOCKET &clientSocket, Serveur * serv, SouvenirData &sd) {
	//recuperation d'un souvenir
	
	//essai de l'ouverture du fichier
	std::ifstream fichierSouvenir(serv->mesDonnees.nomRepSouvenirData +sd.nomDuFichier, std::ifstream::binary);
	bool erreur = false;
	if (fichierSouvenir) {
		fichierSouvenir.seekg(0, fichierSouvenir.end);
		int tailleFic = fichierSouvenir.tellg();
		fichierSouvenir.seekg(0, fichierSouvenir.beg);

		if (tailleFic == EOF) {
			erreur = true;
		}
		else {
			//envoi de l'entete
			std::string str = MsgTypeString[SEND_SOUVENIR_TO_CLIENT];
			str += "i-";
			str += std::to_string(sd.dateTimeInSecond) + "-";
			str += sd.nomDuFichier +"-";
			str += std::to_string(tailleFic) +"-";
			//07-[type]-[datetime]-[nomFichier]-[taille]-
			std::cout << "Envoi au client l'entete : " << str << std::endl;
			SendMessageToClient(clientSocket, str);

			Sleep(100);
			if (tailleFic > 0)
			{
				char buffer[1024];
				memset(buffer,0,1024);
				do
				{
					int num = min(tailleFic, sizeof(buffer));
					fichierSouvenir.read(buffer,num);
					num = fichierSouvenir.gcount();
					if (num < 1)
						erreur= false;
					if (!SendData(clientSocket, buffer, num))
						erreur= false;
					tailleFic -= num;
				} while (tailleFic > 0 && !erreur);


			}
		}
		fichierSouvenir.close();
		std::cout << "Fichier souvenir envoye" << std::endl;
	}
	else {
		int i = 0;
	}
}

//sous fonction
bool Serveur::SendData(SOCKET sock, void *buf, int buflen) {
	unsigned char *pbuf = (unsigned char *)buf;

	while (buflen > 0)
	{
		int num = send(sock, (char *)pbuf, buflen, 0);
		if (num == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// optional: use select() to check for timeout to fail the send
				continue;
			}
			return false;
		}

		pbuf += num;
		buflen -= num;
	}

	return true;
}


//Fonction de reception de fichier
void Serveur::RecvFileFromClient(const SOCKET &clientSocket, int tailleFichier, Serveur * serv, std::string &nomFichier) {

		//mutex pour ecriture lecture fichier
		pthread_mutex_lock(&mutex_lectureEcritureFichier);
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
			std::cout << "Reception terminee : "<<nomFichier << std::endl;
		}
		else {
			serv->mesDonnees.nbImages -= 1;
		}
		pthread_mutex_unlock(&mutex_lectureEcritureFichier);
	
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
	int cran = tp + 2;

	// besoin pour le message
	std::string message; 
	size_t size; 
	char *buffer;
	int len;

	while (enMarche) {
		tp = std::time(nullptr);

		if (tp >= cran) {
			cran = tp + 2;
			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			// mutex pour lire une bonne donnée
			pthread_mutex_lock(& mutex_coord);
			message = "";
			message += MsgTypeString[SEND_COOR_TO_CLIENT];
				message += Orthos.GetSDCoord();
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
				//std::cout << "envoi coord" << std::endl;
			}
			delete[] buffer;
			pthread_mutex_unlock(&mutex_vecSockCli);
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
			pthread_mutex_lock(&mutex_vecSockCli);
			std::cout << "Deconnexion d'un client" << std::endl;
			enFonction = false;
			std::vector<SOCKET>::iterator it;
			it = std::find(serv->vecSocketClient.begin(), serv->vecSocketClient.end(), thisClientSocket);
			serv->vecSocketClient.erase(it);
			pthread_mutex_unlock(&mutex_vecSockCli);
		}
		else {
			Receptionniste(buffer, thisClientSocket, serv);
		}

		
	}
	std::cout << "Fin du thread client" << std::endl;
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

void Serveur::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}


std::vector<std::string> Serveur::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}