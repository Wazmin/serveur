#ifndef ENTITE_H
#define ENTITE_H
/*
	Entite Orthos
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <pthread.h>



struct coordonnees {
	float _x;
	float _y;
};

struct noeud{
	float _x;
	float _y;
	std::vector<noeud> noeudsVoisins;
};

class Entite {
private:
	struct coordonnees coord;
	bool isMoving;
public :
	Entite();
	Entite(float x, float y);
	bool GetIsMoving();
	void SetIsMoving(bool b);
	std::string GetSDCoord();
	std::string GetSDMood();
	void tmpInit();
	void LoadGraph();
};

#endif
