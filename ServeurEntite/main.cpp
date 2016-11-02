#include <iostream>
#include "Serveur.h"
#include <Windows.h>

int main(int argc, char **argv) {
	Serveur leServeur;
	leServeur.Initialiser(42137);
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