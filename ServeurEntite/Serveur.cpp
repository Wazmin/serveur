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
		std::cerr << "listen a �chou� avec l'erreur " << WSAGetLastError() << std::endl;
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

// Receptionniste pour trier les messages entrants
void Serveur::Receptionniste(std::string messageEntrant) {
	std::string enteteMessage = messageEntrant.substr(0, 3);
	
	//tri
	if (enteteMessage.compare(MSG_COORD) != 0) {

	}
	else if (enteteMessage.compare(MSG_MOOD) != 0) {

	}
	else if (enteteMessage.compare(MSG_FILE_TRANSFERT) != 0) {

	}
	else {

	}
}

// thread du serveur pour envois des coordonn�es
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
			// mutex pour lire une bonne donn�e
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

// Thread lanc� pour chaque client des lors qu'ils se connectent
void * Serveur::LancerThreadClient(void * p_data) {

	// boucle principale
	while (enMarche) {

	}
	return NULL;
}