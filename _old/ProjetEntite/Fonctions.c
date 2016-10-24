//#include "Fonctions.h"

#include <stdio.h>


int copierFichier(char const * const, char const * const);

int TmpMain(int argc, char **argv) {
	char fichierS[] = "dataTest";
	char fichierD[] = "dataTest2.";
	copierFichier(fichierS, fichierD);
	

	getchar();

}

int copierFichier(char const * const nomfichierSource, char const * const nomfichierDestination) {
	//création des pointeurs fichiers
	FILE* fSource = NULL;
	FILE* fDest = NULL;
	char buffer[512] = { 0 };
	int nbLus = 0;

	// ouverture du fichier source en lecture binaire
	if ((fSource = fopen(nomfichierSource,"rb")) == NULL) {
		fprintf(stderr,"impossible d'ouvrir le fichier source");
		return 1;
	}

	 //ouverture du fichier destination en ecriture binaire
	if ((fDest = fopen(nomfichierDestination,"wb")) == NULL) {
		fclose(fSource);
		fprintf(stderr, "impossible d'ouvrir un fichier destination\n");
		return 2;
	}

	while ((nbLus = fread(buffer, 1, 512, fSource)) != 0)
		fwrite(buffer, 1, nbLus, fDest);

	fclose(fDest);
	fclose(fSource);
	fprintf(stdout, "copie terminée\n");
	return 0;

}