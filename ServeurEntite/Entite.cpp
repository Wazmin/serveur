#include "Entite.h"

//constructeur
Entite::Entite(){
	isMoving = false;
}

Entite::Entite(float x, float y) {
	this->coord._x = x;
	this->coord._y = y;
}

// construction d'un string pour envoi des coordonnees
std::string Entite::GetSDCoord() {
	std::string str = std::to_string(coord._x);
	str += '-';
	str += std::to_string(coord._y);
	str +='-';
	return str;
}

// construction d'un string pour envoi de l'humeur
std::string Entite::GetSDMood() {
	std::string str;
	str += "1";
	return str;
}

// fonction temporaire pour test
void Entite::tmpInit() {
	this->coord._x = 42.4242;
	this->coord._y = 13.3713;
}

// recuperation is moving
bool Entite::GetIsMoving() {
	return isMoving;
}
// set is moving
void Entite::SetIsMoving(bool b) {
	isMoving = b;
}

//charge le graph pour les deplacements de l'entité
void Entite::LoadGraph() {
	std::ifstream fileUserData("PointEntiteGPS.csv");

	if (!fileUserData)
	{
		std::cerr << "Erreur de lecture PointEntiteGPS.csv\n";
		return;
	}
	fileUserData.seekg(0, fileUserData.end);
	int tailleFic = fileUserData.tellg();
	fileUserData.seekg(0, fileUserData.beg);

	while (!fileUserData.eof() && tailleFic>0) {

	}

	fileUserData.close();
}