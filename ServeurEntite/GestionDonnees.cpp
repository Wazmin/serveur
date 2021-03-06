#include "GestionDonnees.h"

//constructeur
GestionDonnees::GestionDonnees(){}

// chargement des donnees sauvegard�es sur le serveur
void GestionDonnees::LoadRessources() {
	nbImages = 0;
	nbTextes = 0;
	// vecteur UserData
	std::ifstream fileUserData(nomFichierUser);
	if (!fileUserData)
	{
		std::cerr << "Erreur de lecture UserData\n";
		return;
	}
	fileUserData.seekg(0, fileUserData.end);
	int tailleFic = fileUserData.tellg();
	fileUserData.seekg(0, fileUserData.beg);
	while (!fileUserData.eof() && tailleFic>0)
	{
		struct UserData mUserData;
		fileUserData >> mUserData.ID;
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

	fileSouvenirData.seekg(0, fileSouvenirData.end);
	tailleFic = fileSouvenirData.tellg();
	fileSouvenirData.seekg(0, fileSouvenirData.beg);
	while (!fileSouvenirData.eof() && tailleFic>0)
	{
		struct SouvenirData mSouvenirData;
		fileSouvenirData >> tmp;
		mSouvenirData.type = static_cast<TypeSouvenir>(tmp);
		fileSouvenirData >> mSouvenirData.nomDuFichier;
		fileSouvenirData >> mSouvenirData.dateTimeInSecond;
		if (mSouvenirData.type == TYPE_SOUVENIR_TEXTE) {
			fileSouvenirData >> mSouvenirData.phrase;
			nbTextes++;
		}
		else {
			nbImages++;
		}
		vecSouvenirData.push_back(mSouvenirData);
	}
	fileSouvenirData.close();

	std::cout << "Ressources DataUser et Souvenirs chargees !" << std::endl;
}

// sauvegarde des ressources dans un fichier
void GestionDonnees::SaveRessources() {
	//sauvegarde des donnees user
	std::ofstream fileUserData(nomFichierUser, std::ofstream::trunc);
	if (!fileUserData)
	{
		std::cerr << "Erreur de creation fileUserData\n";
		return;
	}

	for (auto ud : vecUserData) {
		fileUserData << ud.ID << " " << ud.dateTimeInSecond << " " << ud.timerNextMeetInSecond << "\n";
	}
	fileUserData.close();

	//sauvegarde des donnes souvenir
	std::ofstream fileSouvenirData(nomFichierSouvenir, std::ofstream::trunc);
	if (!fileSouvenirData) {
		std::cerr << "Erreur de creation fileSouvenirData\n";
		return;
	}

	for (auto sd : vecSouvenirData) {
		//prevision du fichier image avec phrase vide
		fileSouvenirData << sd.type << " " << sd.nomDuFichier << " " << sd.dateTimeInSecond;
		if (sd.type == TYPE_SOUVENIR_TEXTE) {
			fileSouvenirData << " " << sd.phrase;
		}
		fileSouvenirData << "\n";
	}
	fileSouvenirData.close();
}

//Ajout d'un utilisateur � la bonne place
void GestionDonnees::AddUser(std::string numID) {
	struct UserData ud;
	ud.ID = numID;
	ud.dateTimeInSecond = std::time(nullptr);
	ud.timerNextMeetInSecond = timeBeforeNextMeeting;
	int indiceToInsert;
	FindUserIndice(numID, indiceToInsert);
	std::vector<UserData>::iterator it;
	it = vecUserData.begin();
	it += indiceToInsert;
	vecUserData.insert(it, ud);

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
	std::cout << "tirage souvenir = " << dice_roll << std::endl;
	return vecSouvenirData[dice_roll];
}

// ajout d'un souvenir, on insere au debut
// retourne le nouveau nom du fichier
void GestionDonnees::AddSouvenir(SouvenirData sd) {
	std::vector<SouvenirData>::iterator it;
	it = vecSouvenirData.begin();
	vecSouvenirData.insert(it, sd);
}

// ajout d'un souvenir a une date differee
// recherche dichotomique, retourne le nouveau nom du fichier
void GestionDonnees::InsertSouvenir(SouvenirData sd, int dateTimeSouvenirSuivant) {
	//recherche et insertion du souvenir
	int bas(0), milieu(0), haut(vecSouvenirData.size()-1);
	std::vector<SouvenirData>::iterator it;
	it = vecSouvenirData.begin();
	do {
		milieu = (bas + haut) / 2;
		if (dateTimeSouvenirSuivant == vecSouvenirData[milieu].dateTimeInSecond) {
			it += milieu;
			vecSouvenirData.insert(it, sd);
		}
		else if (vecSouvenirData[milieu].dateTimeInSecond > dateTimeSouvenirSuivant) {
			bas = milieu + 1;
		}
		else {
			haut = milieu - 1;
		}
	} while ((dateTimeSouvenirSuivant != vecSouvenirData[milieu].dateTimeInSecond)||(bas>haut));
}

// CanMeetOrthos : est-ce que le joueur peut rencontrer Orthos
bool GestionDonnees::CanMeetOrthos(std::string &numID) {
	int indiceUserData;
	
	if (!FindUserIndice(numID, indiceUserData)) {// non trouv�, on l'ajoute et renvoie vrai
		AddUser(numID);
		return true;
	}
	else {// on l'a trouv�, verifions si il peut la rencontrer
		if ( (vecUserData[indiceUserData].dateTimeInSecond
			+ vecUserData[indiceUserData].timerNextMeetInSecond)
			<= std::time(nullptr)) {
			vecUserData[indiceUserData].dateTimeInSecond = std::time(nullptr);
			return true;
		}
		else {
			return false;
		}
	}
}

//recherche de l'indice d'un utilisateur dans le vector DataUser
// retourne si il a trouv� l'utilisateur et met a jour l'indice
bool GestionDonnees::FindUserIndice(std::string &numID, int &indice) {
	if (vecUserData.size() <= 0) {
		indice = 0;
		return 0;
	}
	int bas(0), milieu(0), haut(vecUserData.size() - 1);

	do {
		milieu = (bas + haut) / 2;
		if (numID == vecUserData[milieu].ID) {
			indice = milieu;
			return true;
		}
		else if (numID < vecUserData[milieu].ID) {
			haut = milieu - 1;
		}
		else {
			bas = milieu + 1;
		}

	} while ((numID != vecUserData[milieu].ID) || (bas>haut));

	//cas o� l'on a pas trouv� l'utilisateur
	indice = milieu;
	return false;
}
