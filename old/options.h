#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "descripteur.h"

char* optionsCourtes; // utilisées dans le main !


void help();
void verbose(bool mode);
void aCompresser(bool compress);
void nomArchive(char* nom);
int createArchive(char** files, int nb_fichiers);
void initialiserOptions();
int optionCompatible(char *liste_options, char* pile_options);
void printManuel(int i);
ListeDescripteurs liste(char* nom_fichier,bool affichage);
#endif