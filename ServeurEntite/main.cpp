#include <iostream>
#include "Serveur.h"

int main(int argc, char **argv) {
	Serveur leServeur;
	leServeur.Initialiser(1234);
	leServeur.Lancer();

	while(1){}

	return EXIT_SUCCESS;
}