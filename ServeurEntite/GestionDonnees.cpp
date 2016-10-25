#include "GestionDonnees.h"

//constructeur
GestionDonnees::GestionDonnees(){}

// chargement des donnees sauvegardées sur le serveur
void GestionDonnees::LoadRessources() {
	// vecteur UserData
	std::ifstream fileUserData(nomFichierUser);
	if (!fileUserData)
	{
		std::cerr << "Erreur de lecture UserData\n";
		return;
	}

	while (!fileUserData.eof())
	{
		struct UserData mUserData;
		fileUserData >> mUserData.IMEI;
		fileUserData >> mUserData.dateTimeInSecond;
		fileUserData >> mUserData.timerNextMeetInSecond;
		vecUserData.push_back(mUserData);
	}
	fileUserData.close();

	// vecteur souvenir
	std::ifstream fileSouvenirData(nomFichierSouvenir);
	if (!fileSouvenirData) {
		std::cerr << "Erreur de lecture SouvenirData\n";
		return;
	}
	int tmp;
	while (!fileSouvenirData.eof())
	{
		struct SouvenirData mSouvenirData;
		fileSouvenirData >> tmp;
		mSouvenirData.type = static_cast<TypeSouvenir>(tmp);
		fileSouvenirData >> mSouvenirData.nomDuFichier;
		fileSouvenirData >> mSouvenirData.dateTimeInSecond;
		vecSouvenirData.push_back(mSouvenirData);
	}
	fileSouvenirData.close();

	std::cout << "Ressources DataUser et Souvenirs chargees !" << std::endl;
}

// sauvegarde des ressources dans un fichier
void GestionDonnees::SaveRessources() {
	//sauvegarde des donnees user
	std::ofstream fileUserData(nomFichierUser,std::ofstream::trunc);
	if (!fileUserData)
	{
		std::cerr << "Erreur de creation fileUserData\n";
		return;
	}

	for (auto ud : vecUserData) {
		fileUserData << ud.IMEI << " " << ud.dateTimeInSecond << " " << ud.timerNextMeetInSecond << "\n";
	}
	fileUserData.close();

	//sauvegarde des donnes souvenir
	std::ofstream fileSouvenirData(nomFichierSouvenir, std::ofstream::trunc);
	if (!fileSouvenirData) {
		std::cerr << "Erreur de creation fileSouvenirData\n";
		return;
	}

	for (auto sd : vecSouvenirData) {
		fileSouvenirData << sd.type << " " << sd.nomDuFichier << " " << sd.dateTimeInSecond << "\n";
	}
	fileSouvenirData.close();
}

// renvoi une structure de type SouvenirData
SouvenirData GestionDonnees::GetSouvenir() {
	int size = vecSouvenirData.size();
	if (size == 0) {
		SouvenirData sderr;
		return sderr;
	}
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, size);
	int dice_roll = distribution(generator);
	dice_roll--;
	return vecSouvenirData[dice_roll];
}

// insertion d'un souvenir a une autre date