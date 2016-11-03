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
	std::cout <<str << std::endl;
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
	this->coord._x = 45.65103366566195;
	this->coord._y = 0.15183281153440475;
	LoadGraph();
	origine = &Graph[0];
	destination = &Graph[1];

	argToPass._coord = &this->coord;
	argToPass.origine = this->origine;
	argToPass.destination = this->destination;
	argToPass.vecGraph = this->Graph;
	argToPass.cMove = &canMove;

	LancerThreadEntite(argToPass);
}

// recuperation is moving
bool Entite::GetIsMoving() {
	return isMoving;
}
// set is moving
void Entite::SetIsMoving(bool b) {
	isMoving = b;
}

// recuperation canMove
bool Entite::GetCanMove() {
	return canMove;
}
// set canMove
void Entite::SetCanMove(bool b) {
	canMove = b;
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

	std::string line;
	char delimitateur = ';';
	int nbNoeuds =37;
	//fileUserData >> nbNoeuds;

	//creation des noeuds dans le vecteur
	for (int i = 0;i < nbNoeuds;i++) {
		Noeud n;
		Graph.push_back(n);
	}

	int j = 0;
	while (std::getline(fileUserData, line) && j < nbNoeuds) {
		std::vector<std::string> subString = split(line, delimitateur);
		
		if (subString.size() > 1) {
			Graph[j]._x = std::stof(subString[0]);
			Graph[j]._y = std::stof(subString[1]);
		}
		int i = 2;
		while (subString.size() > i) {
			Graph[j].voisins.push_back(&Graph[std::stoi(subString[i])-1]);
			i++;
		}
		j++;
	}

	fileUserData.close();
}

void * Entite::ThreadMove(void *p_data) {
	ToThreadArg2 *tta = static_cast<ToThreadArg2*>(p_data);

	NYTimer timerUpdatePos;
	NYTimer timerDeplacement;
	float distanceD = 0.01;
	coordonnees *coordD = tta->_coord;
	Noeud *orig = tta->origine;
	Noeud *dest = tta->destination;
	std::vector<Noeud> leGraph = tta->vecGraph;
	bool _canMove = tta->cMove;

	while (true) {
		if (timerUpdatePos.getElapsedSeconds() > 0.1)
		{
			timerUpdatePos.getElapsedSeconds(true);
			if (_canMove) {
				//est-ce qu'on est arrivé à destination
				if (coordD->_x == dest->_x && coordD->_y == dest->_y) {
					// selection du prochain point
					int randomMax = 0;
					randomMax = dest->voisins.size();
					std::default_random_engine generator;
					std::uniform_int_distribution<int> distribution(0, randomMax - 1);
					int indiceRand = distribution(generator);

					// on boucle tant que qu'on retombe sur l'origine
					while (dest->voisins[indiceRand] == orig) {
						indiceRand = distribution(generator);
					}

					// on renseigne la prochaine destination
					orig = dest;
					dest = orig->voisins[indiceRand];
				}
				else {
					float distanceRestante = Distance(*coordD, dest);
					coordonnees vecDir;
					vecDir._x = dest->_x - orig->_x;
					vecDir._y = dest->_y - orig->_y;
					float normeVecDir = std::sqrtf(std::powf(vecDir._x, 2) + std::powf(vecDir._y, 2));
					//on normalise le vecteur
					vecDir._x /= normeVecDir;
					vecDir._y /= normeVecDir;

					//creation du point temporaire
					coordonnees ptTmp;
					float tempsEcoule = timerDeplacement.getElapsedSeconds(true);
					ptTmp._x = coordD->_x + vecDir._x * distanceD* tempsEcoule;
					ptTmp._y = coordD->_y + vecDir._y * distanceD* tempsEcoule;

					//on verifie qu'on ne depasse pas la distance
					if (distanceRestante < Distance(*coordD, ptTmp)) {
						coordD->_x = dest->_x;
						coordD->_y = dest->_y;
						//std::cout << "coordD x: " << coordD._x << " y: " << coordD._y << std::endl;
					}
					else {
						coordD->_x = ptTmp._x;
						coordD->_y = ptTmp._y;
						//std::cout << "coordD x: " << coordD._x << " y: " << coordD._y << std::endl;
					}

					//std::cout << "coord serv x: " << coord._x << " y: " << coord._y << std::endl;
				}
			}
			
		}
	}



	return NULL;
}

int Entite::LancerThreadEntite(ToThreadArg2 &tta) {
	pthread_t clientThread;
	if (pthread_create(&clientThread, NULL, callThreadEntite, &tta) != 0) {
		//impossible de lancer le thread client !
		std::cerr << "impossible de lancer le thread Entite !" << std::endl;
		return 1;
	}
	return 0;
}

float Entite::Distance(coordonnees &pos1, coordonnees &pos2) {
	return std::sqrtf(std::pow((pos1._x - pos2._x),2)+pow((pos1._y - pos2._y),2));
}

float Entite::Distance(coordonnees &pos1, Noeud *pos2) {
	return std::sqrtf(std::pow((pos1._x - pos2->_x), 2) + pow((pos1._y - pos2->_y), 2));
}

void Entite::split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

std::vector<std::string> Entite::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}