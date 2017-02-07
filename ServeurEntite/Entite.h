#ifndef ENTITE_H
#define ENTITE_H
/*
	Entite Orthos
*/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <random>
#define _TIMESPEC_DEFINED
#include <pthread.h>

#include "Timer.h"


struct coordonnees {
	float _x;
	float _y;
};

struct Noeud {
	float _x;
	float _y;
	std::vector<Noeud*> voisins;
};

struct ToThreadArg2 {
	coordonnees *_coord;
	Noeud *origine;
	Noeud *destination;
	std::vector<Noeud> vecGraph;
	bool *cMove;
};

class Entite {
private:
	struct coordonnees coord;
	bool isMoving;
	bool canMove;
	std::vector<Noeud> Graph;
	Noeud *origine;
	Noeud *destination;
	ToThreadArg2 argToPass;
public :
	Entite();
	Entite(float x, float y);
	bool GetIsMoving();
	void SetIsMoving(bool b);
	bool GetCanMove();
	void SetCanMove(bool b);
	std::string GetSDCoord();
	std::string GetSDMood();
	void tmpInit();
	void ServCoord();
	void LoadGraph();
	void *ThreadMove(void *p_data);
	int Entite::LancerThreadEntite(ToThreadArg2 &tta);
private:
	float Distance(coordonnees &pos1, coordonnees &pos2);
	float Distance(coordonnees &pos1, Noeud *pos2);
	float Distance(coordonnees *pos1, Noeud *pos2);
	void split(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> split(const std::string &s, char delim);
public:
	static void* callThreadEntite(void *arg) { return ((Entite*)arg)->ThreadMove(arg); }
};

#endif
