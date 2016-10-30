#include "Entite.h"

//constructeur
Entite::Entite(){}

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