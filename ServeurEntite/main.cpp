#include <iostream>
#include "Serveur.h"
#include <Windows.h>

int main(int argc, char **argv) {
	Serveur leServeur;
	if (argc < 2) {
		std::cerr << "argument port attendu " <<std::endl;
		return 2;
	}

	leServeur.Initialiser(argv[1]);
	//leServeur.Initialiser("42137");
	leServeur.Lancer();

	//while (true)
	//{
	//	if (GetAsyncKeyState(VK_ESCAPE))
	//	{
	//		std::cout << "Arret serveur";
	//		leServeur.ArreterServeur();
	//	}
	//}

	return EXIT_SUCCESS;
}