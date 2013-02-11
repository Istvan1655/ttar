#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>   
#include <sys/types.h>
#include <getopt.h>

#include "../includes/util.h"
#include "../includes/options.h"


int main(int argc, char** argv) 
{
    initialiserOptions();                                       // initialise les options ET le manuel !

    char* pile = malloc(NB_OPTIONS * sizeof(char));             // stocke les options valides sous forme de chaine de caractères
    char* liste_options = malloc(NB_OPTIONS * sizeof(char));    // stocke les options passées en paramètre au programme sous forme de chaine de caractères
    char* chaine = malloc(sizeof(char));                        // permet de parser un char en char*

    if(argc < 2)                                                // il faut au moins deux arguments (nom du programme et options)
    {
        printf("Erreur : pas assez d'arguments\n");
        printManuel(0);
        return EXIT_FAILURE;
    }
   

    int optch;                  // permet de stocker le code ASCII de l'option
    extern int opterr;          // permet de définir une varibale globale pour gérer l'affichage automatique des erreurs
      
    opterr = 0;                 // on demande l'affichage automatique, 0 pour le désactiver

    char* nom_archive = malloc(TAILLE_NOM * sizeof(char));       // permet de stocker le nom de l'archive

    printf("Traitement des options passées en paramètres.......... ");

    while ((optch = getopt(argc, argv, optionsCourtes)) != -1)   // on demande le prochain caractère qui correpond au masque optionsCourtes
    {
        sprintf(chaine, "%c", optch);                            // on parse l'option de char vers char *
        strcat(liste_options, chaine);                           // on concatène l'option à la liste pour la traiter

        if (strcmp(chaine, "f") == 0)                           // si l'option est f
           strcpy(nom_archive, optarg);                         // on sauvegarde le nom de l'archive présent dans optarg car écrasé à chaque tour de boucle
    }

    sprintf(chaine, "%c", optopt);                              // on convertit les options invalides en chaine de caractère

    printf("Done\n");

    if (strcmp(chaine, "") != 0)                                // si on a rencontré une option invalide
    {
        if (!opterr)                                            // si l'affichage automatique des erreurs n'est pas activé
            printf("\nLe nom de l'archive n'est pas indiqué. Veuillez relancer le programme en ajoutant ce paramètre.\n\n");

        // on libère tout ce qui a été utilisé
        free(nom_archive);
        free(liste_options);
        free(pile);
        return EXIT_FAILURE;
    }

    printf("Vérification de la liste des options : %s .........", liste_options);
    int traitees = optionCompatible(liste_options, pile);           // on traite les options pour ne garder que celles qui sont valides
    printf("Done\n");
    if (traitees >= 0)                                              // < 0 : erreur, = 0 : action sans paramètre, > 0 : action avec paramètres (v, z)
    {
        printf("Traitement des options validées : %s\n", pile);
        switch(pile[0])                                             // on exécute les options (exclusives ou action)
        {
            case 'h':
                help();
            break;
            case 'S':
                printf("L'option Sparse n'est pas disponible pour le moment\n");
            break;
            case 'm':
                printf("L'option match n'est pas disponible pour le moment\n");
            break;
            case 'c':
                if (strchr(pile, 'v') != NULL)                  // si l'option v est présente
                    verbose(true);                              // on active le mode verbose
                if (strchr(pile, 'z') != NULL)                  // si l'option z est présente
                    aCompresser(true);                          // on active la compression
                sprintf(nom_archive, "%s.ttar", nom_archive);
                nomArchive(nom_archive);                        // on fixe le nom de l'archive

                int nb_fichiers = argc - optind;                // optind est l'indice du premier argument exploitable par le programme => premier fichier à traiter
                char ** fichiers_traites = malloc(nb_fichiers * sizeof(char*));     // on devra traiter tous les fichiers au maximum
                int nb_traites = fichiersValides(argv + optind, nb_fichiers, fichiers_traites); // on récupère le nombre de fichiers valides

                if (createArchive(fichiers_traites, nb_traites) == EXIT_SUCCESS)
                    printf("%s a bien été créée\n", nom_archive);
                else
                    printf("Une erreur s'est produite lors de la création de %s\n", nom_archive);

                int i;
                for(i = 0; i < nb_traites; i++)
                {
                    free(fichiers_traites[i]);
                }
                free(fichiers_traites);

                break;
            case 'x':
                printf("L'option extract n'est pas disponible pour le moment\n");
            break;
                case 't':
    	        nomArchive(nom_archive);
    	        if (strchr(pile, 'v') != NULL) //en attendant de faire deux types de liste: complete et partielle 
    		      verbose(true); 	      
    	        liste(nom_archive , true);
                break;
            case 'r':
                printf("L'option append n'est pas disponible pour le moment\n");
            break;
            case 'u':
                printf("L'option update n'est pas disponible pour le moment\n");
            break;
            case 'd':
                printf("L'option delete n'est pas disponible pour le moment\n");
            break;
            default:
                printf("Option non reconnue !\n");
            break;
        }
    }
    else
    {
        printf("Une erreur s'est produite lors de la vérification des options !\n");
    }
    free(nom_archive);
    free(liste_options);
    free(pile);

    printf("Fin de l'utilitaire Tuxtar\n");

    return traitees != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}
