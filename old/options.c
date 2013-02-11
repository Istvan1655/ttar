#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>   
#include <sys/types.h>
#include <sys/stat.h> // pour la gestion des informations sur les fichiers / dossiers (date de dernière modification, isDir, etc...)
#include <dirent.h> // pour la gestion des dossiers
#include <time.h>
#include <errno.h>

#include "../includes/util.h"
#include "../includes/descripteur.h"
#include "../includes/options.h"

#define NB_TYPES_OPTIONS 2


//char** optionsLongues;
char** manuel;
bool affiche;
bool compresse;
char* nom_archive = NULL;
char* espacement = "    ";
char* valide[NB_TYPES_OPTIONS] = {"hS", "cxdrutm"};

/**************************************
 *   Fonctions utilitaires
 **************************************/

bool isExclusif(char option) {
    return strchr(valide[0], option) != NULL;
}

bool isAction(char option) {
    return strchr(valide[1], option) != NULL;
}

int optionCompatible(char *liste_options, char* pile_options) {
    int nb_traitees = 0;
    char* chaine = malloc(sizeof (char));
    bool repetition_v = false;
    bool repetition_z = false;
    bool repetition_f = false;

    int taille = strlen(liste_options); // la taille de la chaine de caractères indique le nombre d'options à vérifier
    
    if (strcmp(liste_options, "v") == 0) {
        printf("Le mode verbose n'est pas accepté seul !\n");
        nb_traitees = -1;
    } else if (strcmp(liste_options, "vf") == 0 || strcmp(liste_options, "fv") == 0) {
        printf("Il doit y avoir au moins une action pour utiliser le mode verbose et préciser le nom de l'archive !!\n");
        nb_traitees = -1;
    } else if (isExclusif(liste_options[0])) // on teste si la première option est exclusive ou pas
    {
        sprintf(chaine, "%c", liste_options[0]); // on empile cet option
        strcat(pile_options, chaine);
        nb_traitees = 1;
    } else if (isAction(liste_options[0])) // on teste si la première option est une action ou pas
    {
        if (taille > 1) {
                sprintf(chaine, "%c", liste_options[0]);
                strcat(pile_options, chaine);
                int i;
                for (i = 1; i < taille - 1; i++) // on parcourt le reste des options à tester
                {
                    if (isAction(liste_options[i]) || isExclusif(liste_options[i])) // si on se retrouve avec plusieurs actions ou options exclusives
                    {
                        printf("Deux options exclusives ou actions ne peuvent pas être utilisées en même temps\n%c est fautif !\n", liste_options[i]);
                        nb_traitees = -1; // on indique une erreur et on sort de la boucle !
                        break;
                    }
                    else {
                        sprintf(chaine, "%c", liste_options[i]);
                        if (strcmp(chaine, "f") == 0) {
                            if(repetition_f){
                            printf("L'option f ne peut pas être présente plusieurs fois ! Veuillez vérifier vos options\n");
                            nb_traitees = -1;
                            break;
                            }
                            repetition_f = true;
                        } else if (strcmp(chaine, "v") == 0) {
                            if (repetition_v) {
                                printf("Le mode verbose ne peut pas être changé plusieurs fois ! Veuillez vérifier vos options\n");
                                nb_traitees = -1;
                                break;
                            }
                            repetition_v = true;
                        } else if (strcmp(chaine, "z") == 0) {
                            if (repetition_z) {
                                printf("On ne peut pas compresser plusieurs fois ! Veuillez vérifier vos options\n");
                                nb_traitees = -1;
                                break;
                            }
                            repetition_z = true;
                        }

                        strcat(pile_options, chaine);
                        nb_traitees++;
                    }
                }
                if (nb_traitees > 0) {
                    sprintf(chaine, "%c", liste_options[taille - 1]);
                    strcat(pile_options, chaine);
                }
        } else {
            printf("Avec une action, vous devez au moins indiquer le nom de l'archive et décider si le mode verbose doit être activé\n");
            nb_traitees = -1;
        }
    } else {
        printf("Le premier caractère doit être une action ou une option exclusive !\n");
        nb_traitees = -1;
    }

    free(chaine);
    return nb_traitees;
}

void initialiserOptions() {
    optionsCourtes = "hvctruxf:zdSm"; // liste des options que l'on accepte
    verbose(false); // par défaut, on désactive le mode verbose ! 
    aCompresser(false); // par défaut, on ne compresse pas l'archive !
    int i;

    /* optionsLongues = malloc(sizeof(char*)*NB_OPTIONS);
       for(i=0 ; i<NB_OPTIONS ; i++)
       optionsLongues[i] = malloc(sizeof(char)*25);
       optionsLongues[0] = "--help";
       optionsLongues[1] = "--verbose";
       optionsLongues[2] = "--create";
       optionsLongues[3] = "--list";
       optionsLongues[4] = "--add";
       optionsLongues[5] = "--update";
       optionsLongues[6] = "--extract";
       optionsLongues[7] = "--file";
       optionsLongues[8] = "--zip";
       optionsLongues[9] = "--del";
       optionsLongues[10] = "--sparse";
       optionsLongues[11] = "--match";*/

    manuel = malloc(sizeof (char*) *NB_OPTIONS);

    for (i = 0; i <= NB_OPTIONS; i++)
        manuel[i] = malloc(sizeof (char) *255);

    manuel[0] = "Usage : tuxtar [OPTIONS] fichier(s)\n";
    manuel[1] = "help\n";
    manuel[2] = "verbose\n";
    manuel[3] = "create\n";
    manuel[4] = "list\n";
    manuel[5] = "add\n";
    manuel[6] = "update\n";
    manuel[7] = "extract\n";
    manuel[8] = "file\n";
    manuel[9] = "zip\n";
    manuel[10] = "del\n";
    manuel[11] = "sparse\n";
    manuel[12] = "match\n";
}

void detruireOptions() {
    int i;
    for (i = 0; i <= NB_OPTIONS; i++)
        free(manuel[i]);
    free(manuel);
    /*for(i=0 ; i<=NB_OPTIONS ; i++)
      free(optionsLongues[i]);
      free(optionsLongues);*/
}

/**************************************
 *   Fonction pour afficher le manuel du programme
 **************************************/

void printManuel(int i) // permet d'afficher une entrée du manuel en particulier
{
    if (i >= 0 && i < NB_OPTIONS)
        printf("%s", manuel[i]);
}

void help() // affiche tout le manuel
{
    int i;
    for (i = 0; i < NB_OPTIONS; i++)
        printf("%s", manuel[i]);
}

/**************************************
 *   Fonction pour afficher ce que fait une commande
 **************************************/

void verbose(bool mode) {
    if (mode)
        printf("Mode verbose activé\n");
    affiche = mode;
}

/****************************************
 *   Fonction pour compresser l'archive 
 *****************************************/

void aCompresser(bool compress) {
    if (compress && affiche)
        printf("Compression demandée\n");

    compresse = compress;
}

void compression(int archive) {
    if (affiche)
        printf("Compression de l'archive correspondante au descripteur de fichier : %d\n", archive);
}

/****************************************
 *   Fonction pour nommer l'archive 
 *****************************************/

void nomArchive(char* nom) {
    nom_archive = nom;
}

/**************************************
 *   Fonctions pour créer une archive 
 **************************************/

int createDossier(char* dir_name, ListeDescripteurs* descripteurs, int archive, uint* profondeur) {
    (*profondeur)++; // on incrémente la profondeur

    int fd; // descripteur de fichier pour chaque fichier
    DIR *dir; // structure pour gérer un dossier : utilisée pour les sous-dossiers
    struct dirent *entry; // pour manipuler chaque entrée (fichiers) d'un dossier
    int nb_sous_fichiers; // pour connaître le nombre de fichiers que contient un sous-dossier
    char ** sous_fichiers; // pour stocker la liste des fichiers d'un sous-dossier
    int taille_sous_fichier = 4; // taille par défaut de la liste de sous_fichiers

    int fichiers_traites; // nombre de fichiers traités

    char b_read[TAILLE_BUFFER]; // buffer pour écrire dans l'archive

    struct stat info; // infos des dossier et fichiers du dossier courant
    int s;
    int lus, ecrits;

    dir = opendir(dir_name);
    nb_sous_fichiers = 0; // on réinitialise le nombre de fichiers pour le sous-dossier

    while ((entry = readdir(dir)) != NULL) // on parcourt le dossier
    {
        if (strcmp(entry->d_name, ".") != 0 || strcmp(entry->d_name, "..") != 0) // permet d'éliminer les fichiers spéciaux : ".", ".."
        {
            int taille = strlen(entry->d_name) + strlen(entry->d_name);
            char * fic_arbo = malloc(taille * sizeof(char));
            strcpy(fic_arbo, dir_name);
            strcat(fic_arbo, "/");
            strcat(fic_arbo, entry->d_name);
            s = stat(fic_arbo, &info); // on récupère ses informations
            
            if (S_ISDIR(info.st_mode)) // si c'est un dossier
            {
                printf("%s est un dossier \n", fic_arbo);
                if (affiche)
                    printf("%s/\n", entry->d_name);
                Descripteur d = construireDescripteur(0, (uint) info.st_mtime, (uint) info.st_size, (uint) info.st_uid, (uint) info.st_gid,
                        (uint) info.st_mode, *profondeur, strlen(dir_name), dir_name);
                ajouter(*descripteurs, d); // On ajoute le descripteur du dossier parent à la liste
                fichiers_traites += createDossier(entry->d_name, descripteurs, archive, profondeur);
                if (affiche)
                    printf("\n\tDone\n");
            } else if (S_ISREG(info.st_mode)) // si c'est un fichier
            {
                printf("\t%s", fic_arbo);
                fd = open("src/main.c", O_RDONLY, NULL);

                if (fd == -1) // si on ne peut pas ouvrir le fichier en lecture seule
                {
                    printf("Erreur lors de l'ouverture du fichier %s\n", entry->d_name);
                    continue; // on passe au suivant
                }

                while ((lus = read(fd, &b_read, TAILLE_BUFFER)) > -1) // on lit tout le fichier
                {
                    ecrits = write(archive, &b_read, lus); // on écrit ce qui vient d'être lu
                    if (ecrits != lus) // si on a pas pu tout écrire
                    {
                        printf("Archive corrompue. Tout le contenu de %s n'a pas pu être copié dans l'archive !\n", entry->d_name);
                    }
                }

                Descripteur d = construireDescripteur(1, (uint) info.st_mtime, (uint) info.st_size, (uint) info.st_uid, (uint) info.st_gid,
                        (uint) info.st_mode, *profondeur, strlen(dir_name), dir_name);
                ajouter(*descripteurs, d);
                fichiers_traites += 1;

                if (close(fd) == -1) {
                    printf("Erreur lors de la fermeture de %s\n", entry->d_name);
                }

                if (affiche)
                    printf("\tDone\n");

            } else
            {
                printf("Type non géré pour %s car ", fic_arbo);
                /*if (errno == EBADF)
                    printf("bad descripteur\n");
                if (errno == ENOENT)
                    printf("n'existe pas\n");
                if (errno == ENOTDIR)
                    printf("pas dossier\n");
                if (errno == EFAULT)
                    printf("adressage\n");
                if (errno == ELOOP)
                    printf("liens\n");
                if (errno == ENOMEM)
                    printf("mémoire\n");
                if (errno == ENAMETOOLONG)
                    printf("nom trop long\n");
                if (errno == EACCES)
                    printf("accès\n");*/
            }
                

            nb_sous_fichiers++; // on augmente le nombre de fichiers à traiter
            if (nb_sous_fichiers >= taille_sous_fichier - 1) { // si le tableau n'est pas assez grand, on lui réalloue la mémoire
                sous_fichiers = (char**) realloc(&sous_fichiers, taille_sous_fichier * 2);
                taille_sous_fichier *= 2;
            }
            free(fic_arbo);
        }
    }
    free(sous_fichiers);

    closedir(dir);

    return fichiers_traites;
}

int createFiles(char** files, int nb_fichiers, ListeDescripteurs* descripteurs, int archive) {
    struct stat info; // pour stocker les informtations d'un fichier / répertoire
    int s; // pour vérifier que les informations soient bien accessibles
    int total_fichiers = 0;

    int fd;
    int lus, ecrits; // permettent de stocker le nombre d'octets lus et écrits dans un fichier

    char b_read[TAILLE_BUFFER]; // buffer pour écrire dans l'archive

    uint profondeur = 0;

    int i;
    for (i = 0; i < nb_fichiers; i++) // on parcourt chaque fichier
    {
        s = stat(files[i], &info); // on récupère ses informations
           
            if (S_ISDIR(info.st_mode)) // si c'est un dossier
            {

                
             if (affiche)
                    printf("%s/\n ", files[i]);
                Descripteur d = construireDescripteur(0, (uint) info.st_mtime, (uint) info.st_size, (uint) info.st_uid, (uint) info.st_gid,
                        (uint) info.st_mode, profondeur, (uint) sizeof (files[i]) / sizeof (char), files[i]);
                ajouter(*descripteurs, d); // On ajoute le descripteur du dossier parent à la liste
                total_fichiers += createDossier(files[i], descripteurs, archive, &profondeur);
                if (affiche)
                    printf("\n\tDone\n");
            } else if (S_ISREG(info.st_mode)) // si c'est un fichier
            {
                if (affiche)
                    printf("%s ", files[i]);
                fd = open(files[i], O_RDONLY, NULL);

                if (fd == -1) // si, on ne peut pas ouvrir le fichier en lecture seule
                {
                    printf("Erreur lors de l'ouverture du fichier %s\n", files[i]);
                    continue; // on passe au suivant
                }
                else
                {
                    while ((lus = read(fd, &b_read, TAILLE_BUFFER)) > 0) // on lit tout le fichier
                    {
                        ecrits = write(archive, &b_read, lus); // on écrit ce qui vient d'être lu
                        if (ecrits != lus) // si on a pas pu tout écrire
                        {
                            printf("Archive corrompue. Tout le contenu de %s n'a pas pu être copié dans l'archive !\n", files[i]);
                        }
                    }

                    Descripteur d = construireDescripteur(1, (uint) info.st_mtime, (uint) info.st_size, (uint) info.st_uid, (uint) info.st_gid,
                            (uint) info.st_mode, profondeur, (uint) strlen(files[i]), files[i]);
                    ajouter(*descripteurs, d);
                    suivant(*descripteurs);
                    
                    total_fichiers += 1;

                    if (close(fd) == -1) {
                        printf("Erreur lors de la fermeture de %s\n", files[i]);
                        return -1;
                    }
                    if (affiche)
                        printf("\tDone\n");    
                }
            }
            else
                printf("Type non géré pour %s car %s\n", files[i], info.st_mode);
        
   // 
    }
       
    return total_fichiers;
}

int createArchive(char** files, int nb_fichiers) {
    
    if (strcmp(nom_archive, "") == 0 || nom_archive == NULL)
        nom_archive = files[0];

    int archive = open(nom_archive, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (archive == -1) // si on a pas pu ouvrir le fichier
    {
        printf("L'archive n'a pas pu être ouverte\n");
        return EXIT_FAILURE;
    }

    printf("Création de l'archive %s\n", nom_archive);

    ListeDescripteurs descripteurs; // pour mémoriser les descripteurs de tous les fichiers / dossiers + archive (spécial)
    int nb_descripteurs = 0;

    descripteurs = construireListeVide();
    nb_descripteurs = createFiles(files, nb_fichiers, &descripteurs, archive); // on traite tous nos fichiers en paramètre

    tete(descripteurs);

    uint** all_d = getTousLesDescripteursToInt(descripteurs);
    
    int j;
    
    uint premierDescripteur = (uint) lseek(archive, 0, SEEK_CUR);
    int i;

    if (affiche)
        printf("Ecriture de la configuration de l'archive...... ");

    for (i = 0; i < nb_descripteurs; i++) {
        int nb_int = taille(all_d[i]);
        int dernier_int = taille(all_d[0]);
        all_d[0] = (uint*) realloc(all_d[0], nb_int * sizeof(int) * sizeof(all_d[0]));
        for(j=0 ; j<nb_int ; j++){
          all_d[0][j+dernier_int] = all_d[i][j];
        }
         

        int ecrit = write(archive, &all_d[i], nb_int);

        if ((ecrit < 0) || (ecrit != nb_int))
            printf("Erreur lors l'écriture de la configuration de l'archive !\n");
    }
    
    detruireListeInt(all_d, nb_descripteurs);
    detruireListeDescripteurs(descripteurs);

    write(archive, &premierDescripteur, sizeof(uint));

    if (affiche)
        printf("Done\n");

    if (compresse)
        compression(archive);

    if (close(archive) == -1) {
        printf("Attention, l'archive n'a pas pu être fermée... Possibilité de corruption du fichier !");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/******************************************************
 *   Fonction pour extraire les fichiers de l'archive 
 *******************************************************/

void extract() {

}

/**************************************
 *   Fonction pour lister une archive 
 **************************************/

ListeDescripteurs liste(char* nom_fichier, bool affichage) {
    int fd = open(nom_fichier, O_RDONLY);
    if (fd == -1) {
        printf("%s n'a pas pu être ouvert... Merci de vérifier son chemin\n", nom_fichier);
        return NULL;
    }
    //printf("Avant lseek\n");
    off_t finDescripteurs = lseek(fd, ((-1)  * sizeof(uint)), SEEK_END);             //on se met au descripteur global d'archive
    uint archive;
    //printf("avant read\n");
    read(fd, &archive, 32);
    //printf("avant premierDescripteur\n");
    off_t premierDescripteur = getPremierDescripteur(archive);      //on construit le 1° descripteur
    
    lseek(fd, premierDescripteur, SEEK_SET);

    uint bufferDescripteursBruts;
    uint* descripteursBruts = malloc(255 * sizeof(uint)); 
    int lus;
    int i=0;
    while((lus = read(fd, &bufferDescripteursBruts, sizeof(uint))) > 0){
        descripteursBruts[i] = bufferDescripteursBruts;
    }
    // On a récupérer les descripteurs en "brut" (que des int qui se suivent)
    if(descripteursBruts == NULL){
        printf("le buffer est vide\n");
    }
    // Cette fonction nous créé une liste de descripteurs
    ListeDescripteurs liste =  getIntToDescripteurs(descripteursBruts);

    // partie affichage
    if (affichage) {
        int j;
        printf("type:date:nom");
        if (affiche)
            printf(":taille(octets):uid:gid:droits");
        printf("\n");

        int profTab = 0;
        if(courant(liste) == NULL){
            printf("Erreur : impossible de récupérer la configuration de l'archive\n");
            detruireListeDescripteurs(liste);
            return NULL;
        }
        do{
            char* espacement_actuel = "";
            profTab = getProfondeur(courant(liste));
            for (j = 0; j < profTab; j++) { //tabulation par rapport a la profondeur du fichier
                strcat(espacement_actuel, espacement);
            }
            char* nameS = getFilename(courant(liste));
            printf("nom de fichier%s\n", getNbCarac(courant(liste)));
    	    printf("%s->%u:%u:%s", espacement_actuel, getType(courant(liste)), getDate(courant(liste)), nameS), getFilename(courant(liste));

            if (affiche) { //le reste des parametres de l'archive si demandé
                printf(":%u:%u:%u:%u", getTailleFichier(courant(liste)), getUid(courant(liste)), getGid(courant(liste)), getDroits(courant(liste)));
            }
            printf("\n");
        }while(suivant(liste));
    }
    close(fd);
    return liste;
}



/****************************************************
 *   Fonction pour supprimer un fichier de l'archive 
 ****************************************************/

int delete(char* archive) {
    int fd = open(archive, O_RDONLY);
    if (fd == -1) {
        printf("%s n'a pas pu être ouvert... Merci de vérifier son chemin\n", archive);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}


