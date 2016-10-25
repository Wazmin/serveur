#ifndef GESTIONDONNEES_H
#define GESTIONDONNEES_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>

struct UserData {
	__int64 IMEI;
	int dateTimeInSecond;
	int timerNextMeetInSecond;
};

enum TypeSouvenir {
	TYPE_SOUVENIR_IMAGE = 0,
	TYPE_SOUVENIR_TEXTE = 1
};

struct SouvenirData{
	TypeSouvenir type;
	std::string nomDuFichier;
	int dateTimeInSecond;
};

class GestionDonnees {
private:
	std::vector<UserData> vecUserData;
	std::vector<SouvenirData>  vecSouvenirData;
	const std::string nomFichierUser = "UserData";
	const std::string nomFichierSouvenir = "SouvenirData";
	const int timeBeforeNextMeeting = 300; //in seconds
public:
	GestionDonnees();
	void LoadRessources();
	void SaveRessources();
	SouvenirData GetSouvenir();
	void AddSouvenir(SouvenirData sd);
	void InsertSouvenir(SouvenirData sd);
	bool CanMeetOrthos(__int64 IMEI);
	void MeetOrthos(__int64 IMEI);
};

#endif
