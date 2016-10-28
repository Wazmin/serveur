#ifndef GESTIONDONNEES_H
#define GESTIONDONNEES_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>

struct UserData {
	__int64 IMEI;
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
	void AddUser(__int64 IMEI);
	SouvenirData GetSouvenir();
	void AddSouvenir(SouvenirData sd);
	void InsertSouvenir(SouvenirData sd, int dateTimeSouvenirSuivant);
	bool CanMeetOrthos(__int64 IMEI);
private:
	bool FindUserIndice(__int64 numIMEI, int &indice);
};

#endif
