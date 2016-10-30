#ifndef GESTIONDONNEES_H
#define GESTIONDONNEES_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <thread>

struct UserData {
	std::string ID;
	int dateTimeInSecond;//time of the lastest meeting
	int timerNextMeetInSecond;//time in second before the next meeting
};

enum TypeSouvenir {
	TYPE_SOUVENIR_IMAGE = 0,
	TYPE_SOUVENIR_TEXTE = 1
};

struct SouvenirData{
	TypeSouvenir type;
	std::string nomDuFichier;
	int dateTimeInSecond;
	std::string phrase;
};

class GestionDonnees {
private:
	std::vector<UserData> vecUserData;
	std::vector<SouvenirData>  vecSouvenirData;
	const int timeBeforeNextMeeting = 300; //in seconds
public:
	const std::string nomFichierUser = "UserData";
	const std::string nomFichierSouvenir = "SouvenirData";
	const std::string nomRepSouvenirData = "Souvenirs/";
	int nbImages;
	int nbTextes;
	GestionDonnees();
	void LoadRessources();
	void SaveRessources();
	void AddUser(std::string numID);
	SouvenirData GetSouvenir();
	void AddSouvenir(SouvenirData sd);
	void InsertSouvenir(SouvenirData sd, int dateTimeSouvenirSuivant);
	bool CanMeetOrthos(std::string &numID);
private:
	bool FindUserIndice(std::string &numID, int &indice);
};

#endif
