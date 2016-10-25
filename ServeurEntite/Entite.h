#ifndef ENTITE_H
#define ENTITE_H
/*
	Entite Orthos
*/
#include <iostream>
#include <string>

struct coordonnees {
	float _x;
	float _y;
};

class Entite {
private:
	struct coordonnees coord;
public :
	Entite();
	Entite(float x, float y);
	std::string GetSDCoord();
	std::string GetSDMood();
	void tmpInit();
};

#endif
