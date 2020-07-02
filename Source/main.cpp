//-----------------------------------INCLUDE----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <numeric>
#include <time.h>
#include <pthread.h>
#include <cstdlib>

using namespace std;

#ifdef _WIN32 
#include <Windows.h>
#elif  _WIN64
#include <Windows.h>
#else
#include <unistd.h>
#endif


#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
     
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

//-----------------------------------DATA----------------------------------------------------------------

#include "data.h"
//#include "data_1.h"

//-----------------------------------VARIABLE GLOBALE AJOUTE----------------------------------------------------------------

#include "variable.h"

//-----------------------------------MATRICE----------------------------------------------------------------

//Calcul la matrice des distances
/*
    On va calculer la distance d'un noeud i jusqu'a tous les autres noeud
*/
void GetDistance(){
    //pour chaque noeud
    for(int i = 0; i < NBR_NODES; i++){
        //pour chaque noeud
        for(int j = 0; j < NBR_NODES; j++){
            //si on calcul la distance d'un noueud jusqu'a lui meme
            if(i == j){
                distance[i][j] = -1; //le -1 est comme un infini
            }else{
                //calcul de la distance entre les deux coordoné
                distance[i][j] = sqrt(pow(coord[i][0] - coord[j][0],2) + pow(coord[i][1] - coord[j][1],2));
            }
        }
    }
}

//Affiche une matrice
/*
*/
void print_matrix(double d[NBR_NODES][NBR_NODES])
{
    //pour chaque valeur de la matrice on l'affiche
    int i, j ;
    for (i=0; i<NBR_NODES; i++)
    {
        printf ("%d:", i) ;
        for (j=0; j<NBR_NODES; j++)
        {
            printf ("%6.1f ", d[i][j]) ;
        }
        printf ("\n") ;
    }
}

//-----------------------------------CALCUL CLASSIQUE----------------------------------------------------------------

//Calcul la moyenne
int moyenne(int* data, int size_arr)
{
    int sum = 0, mean;

    int i;

    for(i = 0; i < size_arr; i++){
        sum += data[i];
    }

    mean = sum/size_arr;
    return mean;
}

//calcul de l'ecart type
int calculateSD(int* data, int size_arr)
{
    int mean, standardDeviation = 0.0;

    mean = moyenne(data, size_arr);

    for(int i = 0; i < size_arr; i++){
        standardDeviation += pow(data[i] - mean, 2);
    }

    return sqrt(standardDeviation / size_arr);
}

//-----------------------------------TEMPS TRAJET----------------------------------------------------------------

//evalue le jour reel de depart pour une mission
int jour_reel_depart(int jour_base, int heure_prevu, int numero_lieu_formation, int num_interface, int num_apprenant){
    //Si on n'a pas le lieu de formation (aucune specialite)
    if (numero_lieu_formation == -1){
        //Le calcul est ici l'heure prevu de formation - la distance entre le domicile de l'interface et celui de l'apprenant multiplier le temps par unite de distance diviser par 60 le tout arrondi a l'entier inferieur
        int heure = floor(heure_prevu - ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60)); //calcul l'heure
        return (jour_base+floor((heure-24)/24)); //regarde si c'est negatif (donc le jour precedent)
    }
    int heure = (floor(heure_prevu - ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60) - ((distance[numero_lieu_formation][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60)));
    return (jour_base+floor((heure-24)/24));
}

//evalue le jour reel d'arriver apres une mission
int jour_reel_arrive(int jour_base, int heure_prevu, int numero_lieu_formation, int num_interface, int num_apprenant){
    if (numero_lieu_formation == -1){
        int heure = ceil(heure_prevu + ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60));
        return (jour_base+floor(heure/24));
    }
    int heure = (ceil(heure_prevu + ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60) + ((distance[numero_lieu_formation][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60)));
    return (jour_base+floor(heure/24));
}

//evalue l'heure a laquel l'interface doit partir de son domicile pour se rendre au domicile de l'apprenant puis arriver a l'heure au lieu de formation (On arrondi a l'heure supperieur)
int calcul_heure_depart(int heure_prevu, int numero_lieu_formation, int num_interface, int num_apprenant){
    if (numero_lieu_formation == -1){
        //heure calculer (meme calcul que plus haut)
        int heure = floor(heure_prevu - ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60));
        //On remet l'heure en modulo 24
        return heure%24;
    }
    int heure = (floor(heure_prevu - ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60) - ((distance[numero_lieu_formation][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60)));
    return heure%24;
}

//evalue l'heure a laquel l'interface arrive a son domicile apres avoir raccompagner l'apprenant a son domicile en partant a l'heure de la fin de formation du lieu de formation (On arrondi a l'heure supperieur)
int calcul_heure_arrive(int heure_prevu, int numero_lieu_formation, int num_interface, int num_apprenant){
    if (numero_lieu_formation == -1){
        int heure = (ceil(heure_prevu + ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60)));
        return heure%24;
    }
    int heure = (ceil(heure_prevu + ((distance[num_interface+NBR_CENTRES_FORMATION][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60) + ((distance[numero_lieu_formation][num_apprenant+NBR_CENTRES_FORMATION+NBR_INTERFACES] * Temps_par_unite_distance)/60)));
    return heure_prevu%24;
}

//-----------------------------------FONCTION D'EVALUATION----------------------------------------------------------------

//test si la solution respecte les contrainte (0 si non, 1 si oui)
/*
    On va venir tester toutes les contrainte du probleme,
    si une contrainte s'avere non resppecter on renvoie 0,
    si on arrive a la fin de la fonction, ca signifie que les contrainte sont respecter
    on renvoie donc 1 a la fin pour dire que la solution est valide
*/
int isValid(int tempsTravail[NBR_JOURS][NBR_INTERFACES], int num_formation, int num_interface, int Tps){

    //Bonne competence
    if(competences_interfaces[num_interface][formation[num_formation][2]] != 1){
        return -1; //renvoie solution non valide
    }

    /*
    Temps de travail legal maximum en France :
        Quotidien : 10h,
        Hebdomadaire : 48h
    Amplitude maximum d'une journee en France est de 13h
    */

    //temps de travail dans une journé inf a 10h
    if(tempsTravail[formation[num_formation][3]][num_interface] + Tps > 10){
        return -2; //renvoie solution non valide
    }

    //Total de travail inf a 44h par semaine
    int sum = 0; //Somme du temps de travail dans la semaine
    int jour_init_semaine = (formation[num_formation][3]/7)*7; //Jour debut de la semaine
    for(int i = jour_init_semaine; i < min(jour_init_semaine+7, NBR_JOURS); i++){ //pour chaque jour de la semaine
        //Incremente le temps de travail de la semaine avec le temps de travail dans la journee
        sum += tempsTravail[i][num_interface];
    }
    //Test si le temps de travail est strictement superieur a 48h
    if( sum > 48){
        return -3; //renvoie solution non valide
    }

    //Aucun autre rdv en meme temps et bonne emplitude
    for (int i = 0; i < NBR_FORMATION; i++){ //pour chaque formation
        if(Attribution[i] == num_interface && i != num_formation){ //Si ce n'est pas la solution tester

            //jour de depart et d'arriver avec les temps de trajet
            int jour_arr_act, jour_arr_tester, jour_dep_act, jour_dep_tester;
            jour_arr_act = jour_reel_arrive(formation[num_formation][3],formation[num_formation][5], formation[num_formation][1],Attribution[num_formation],formation[num_formation][0]);
            jour_arr_tester = jour_reel_arrive(formation[i][3],formation[i][5], formation[i][1],Attribution[i],formation[i][0]);
            jour_dep_act = jour_reel_depart(formation[num_formation][3],formation[num_formation][4], formation[num_formation][1],Attribution[num_formation],formation[num_formation][0]);
            jour_dep_tester = jour_reel_depart(formation[i][3],formation[i][4], formation[i][1],Attribution[i],formation[i][0]);

            if( ((jour_dep_act >= jour_dep_tester) && (jour_dep_act <= jour_arr_tester)) || ((jour_arr_act >= jour_dep_tester) && (jour_arr_act <= jour_arr_tester)) ){ //Si c'est le meme jour

                /*
                Possibbilite de commencer une mission juste apres la fin d'une autre
                ex : un mission fini a 14h une autre peut recommencer a 14h
                si on ne veut pas ca, penser a mettre le = dans
                    formation[num_formation][4] < formation[i][5] et formation[num_formation][5] > formation[i][4]
                */

                //heure de depart et d'arriver avec le temps de trajet pris en compte
                int heure_depart_act, heure_depart_tester, heure_arriver_act, heure_arriver_tester;
                heure_depart_act = calcul_heure_depart(formation[num_formation][4], formation[num_formation][1],Attribution[num_formation],formation[num_formation][0]);
                heure_depart_tester = calcul_heure_depart(formation[i][4], formation[i][1],Attribution[i],formation[i][0]);
                heure_arriver_act = calcul_heure_arrive(formation[num_formation][5], formation[num_formation][1],Attribution[num_formation],formation[num_formation][0]);
                heure_arriver_tester = calcul_heure_arrive(formation[i][5], formation[i][1],Attribution[i],formation[i][0]);

                //Test s'il n'y a aucun horraire a la meme heure et le meme jour
                if( (heure_depart_act >= heure_depart_tester) && (heure_depart_act < heure_arriver_tester) ){
                    return -4; //renvoie solution non valide
                }
                //Test s'il n'y a aucun horraire a la meme heure et le meme jour
                if( (heure_arriver_act > heure_depart_tester) && (heure_arriver_act <= heure_arriver_tester) ){
                    return -5; //renvoie solution non valide
                }
                //test si l'emplitude de 13h est respecter
                if( ((formation[num_formation][5] - formation[i][4]) > 13 ) || ((formation[i][5] - formation[num_formation][4]) > 13 ) ){
                    return -6; //renvoie solution non valide
                }
            }
        }
    }

    return 1; //Solution valide
}

//Evalue une solution
/*
    Renvoie la distance + penalite
*/
int evaluation_solution(int sol[NBR_FORMATION]){

    double distance_temporaire = 0.0; //Distance sur 1 trajet
    int distance_tot_interface = 0; //Somme des distannce
    //temps de travail des interfaces, temps de pause le midi (est initialiser a 3 car pause possible entre 11h et 14h)
    int tempsTravail[NBR_JOURS][NBR_INTERFACES] = {{0}}, tempsPauseMidi[NBR_JOURS][NBR_INTERFACES] = {{0}};
    double penal = 0.0; //penalite pour un trajet
    int limit[NBR_COMPETENCE]; //nombre limite de trajet par interface (pour une meilleur repartition)

    //si la penalite pour la repartition doit etre applique et sur qui
    //si l'inteface est trouver, nombre d'interface non troouver, nombre d'apparition de chaque interface
    int findit, nonfind = 0, apparition[NBR_INTERFACES] = {0}, distance_parcour_interface[NBR_INTERFACES] = {0}, temps_travail[NBR_INTERFACES] = {0};
    for(int i = 0; i < NBR_INTERFACES; i++){ //pour chaque interface
        findit = 0; //interface non trouver
        for(int j = 0; j < NBR_FORMATION; j++){ //pour chaque formation
            if(sol[j] == i){ //si l'inteface viser
                apparition[i] += 1; //on augmente le nombre d'apparition de l'interface
                distance_parcour_interface[i] += distance[i+NBR_CENTRES_FORMATION][formation[j][0] + NBR_CENTRES_FORMATION + NBR_INTERFACES]; //somme des distances parcouru
                temps_travail[i] += formation[j][5] - formation[j][4];

                findit = 1; // Interface trouver
            }
        }
        //Si l'interface n'est pas trouver
        if(findit == 0){
            nonfind++; //On incremente le nombre d'interface non trouver
        }
    }

    int moyenne_distance = moyenne(distance_parcour_interface, NBR_INTERFACES); //moyenne des distances parcouru par les interfaces
    int stdev_distance = calculateSD(distance_parcour_interface, NBR_INTERFACES); //ecart type
    int moyenne_temps = moyenne(temps_travail, NBR_INTERFACES); //moyenne du temps de travail
    int stdev_temps = calculateSD(temps_travail, NBR_INTERFACES); //ecart type

    int moy_abs_distance, moy_abs_temps; //difference absolue entre la moyenne et la distance et le temps

    //pour chaque interface
    for(int inter = 0; inter < NBR_INTERFACES; inter++){

        moy_abs_distance = abs(distance_parcour_interface[inter] - moyenne_distance); //difference absolue entre la moyenne et la distance
        moy_abs_temps = abs(temps_travail[inter] - moyenne_temps); //difference absolue entre la moyenne et le temps

        //si la distance n'etait pas dans l'intervale entre moyenne+ecart type et moyenne- ecart type
        if( (moy_abs_distance-stdev_distance) < 0 ){
            distance_tot_interface += abs(moy_abs_distance-stdev_distance) * penalite_distance_dif; //penalise la distance
        }

        //si le temps n'etait pas dans l'intervale entre moyenne+ecart type et moyenne- ecart type
        if( (moy_abs_distance-stdev_distance) < 0 ){
            distance_tot_interface += abs(moy_abs_temps-stdev_temps) * penalite_temps_dif; //penalise l'interface
        }
    }

    // Si il y'a plus d'interface que de formation
    if(NBR_INTERFACES > NBR_FORMATION){
        //On retire l'ecart entre le nombre d'interface et de formation au nombre d'interface non trouver
        nonfind = nonfind - (NBR_INTERFACES - NBR_FORMATION);

        //La limite de chaque competence est 1
        for(int comp = 0; comp < NBR_COMPETENCE; comp++){
            limit[comp] = 1;
        }

    }else{//Cherche la personne avec le moins de mission par competence
        float mini[NBR_COMPETENCE];
        int nbInterface[NBR_COMPETENCE];

        //Pour chaque competence
        for(int i = 0; i < NBR_COMPETENCE; i++){
            mini[i] = 999999.0;

            //pour chaque formation
            for(int j = 0; j < NBR_INTERFACES; j++){
                if(competences_interfaces[j][i] == 1){
                    mini[i] = min(mini[i], apparition[j]);
                }
            }
            if(mini[i] == 0){
                limit[i] = 1; //une interface n'a pas de mission, on force pour lui en attribuer une
            }else{
                limit[i] = mini[i] * 2; //pas plus du double de travail que la personne travaillant le moins sinon penalité
            }
        }
    }

    //printf("Limite competence 1 : %d, Limite competence 2 : %d\n", limit[0], limit[1]);

    //pour chaque formation
    for(int i = 0; i < NBR_FORMATION; i++){

        //remet la penalite a 0
        penal = 0.0;

        //Duree de la formation
        int Tps = formation[i][5] - formation[i][4];//temps de la mission

        //test si la solution est valide
        if(isValid(tempsTravail, i, sol[i], Tps) != 1){
            //printf("Num formation problematique : %d\n", i);
            return -1; //retourne -1 si non valide
        }

        //distance entre le domicile de l'interface et celui de l'apprenant
        distance_temporaire = distance[sol[i] + NBR_CENTRES_FORMATION][formation[i][0] + NBR_CENTRES_FORMATION+NBR_INTERFACES];

        //penalite pour la spe
        if(formation[i][1] != -1 && specialite_interfaces[sol[i]][formation[i][1]] == 0){
            penal += (double) ( distance_temporaire * penalite_spe ); //ajoute a la penalite
        }

        //Penalite pour le temps de travail quotidien
        if(tempsTravail[formation[i][3]][sol[i]] + Tps > 8){
            penal += (double) ( distance_temporaire * penalite_heure_sup ); //ajoute a la penalite
        }

        //Total de travail inf a 44h par semaine
        int sum = 0; //Somme du temps de travail dans la semaine
        int jour_init_semaine = (formation[i][3]/7)*7; //Jour debut de la semaine
        for(int j = jour_init_semaine; j < min(jour_init_semaine+7, NBR_JOURS); j++){ //pour chaque jour de la semaine
            //Incremente le temps de travail de la semaine avec le temps de travail dans la journee
            sum += tempsTravail[j][sol[i]];
        }
        //Test si le temps de travail est strictement superieur a 44h
        if( sum > 35){
            penal += (double) ( distance_temporaire * penalite_heure_sup ); //ajoute a la penalite
        }

        //penalite pour le midi (heure de pause midi : 11h-14h)
        //regarde le temps de formation entre 11h et 14h, on retire cette duree au temps de pause le midi
        if( (11 <= formation[i][4]) && (14 > formation[i][4]) || (11 < formation[i][5]) && (14 >= formation[i][5]) || (11 >= formation[i][4]) && (14 <= formation[i][5]) ){

            tempsPauseMidi[formation[i][3]][sol[i]] -= min(min(3, formation[i][5]-11), min(formation[i][5]-formation[i][4], 14 - formation[i][4]));

            if(tempsPauseMidi[formation[i][3]][sol[i]] <= -3){ //Si le temps de pause est -3 ==> aucune pause le midi
                penal += (double) ( distance_temporaire * penalite_midi ); //ajoute a la penalite
            }

        }

        //penalite pour la repartition : On penalise les distances faite par des interface avec plus du double en travail que la personne en ayant le moins
        if( (nonfind > 0) ){
            for(int comp = 0; comp < NBR_COMPETENCE; comp++){
                if(competences_interfaces[sol[i]][comp] == 1 && (apparition[sol[i]] > limit[comp])){
                    penal += (double) ( distance_temporaire * penalite_repartition * (apparition[sol[i]] - limit[comp])); //ajoute a la penalite
                    //printf("Penalite de %f\n", ( distance_temporaire * penalite_repartition * (apparition[sol[i]] - limit[comp])));
                }
            }
        }

        //printf("Interface %d pour la formation %d, penalite de %f\n",sol[i], i, (penal) );

        //Distance parcouru + penalite
        distance_tot_interface += (int) (distance_temporaire + penal);

        //printf("%d += %f + %f\n", distance_tot_interface, distance_temporaire, penal);

        //incremente le temps de travail
        tempsTravail[formation[i][3]][sol[i]] += Tps;
    }

    //retourne l'evaluation
    return distance_tot_interface;
}

//Permet de dire si une partie de la solution est valide
int partial_eval(int sol[NBR_FORMATION], int nb_to_check){
    int tempsTravail[NBR_JOURS][NBR_INTERFACES] = {{0}}; //temps de travil d'une interface un jour donnée

    //printf("Interface : %d, Formation : %d\n", sol[nb_to_check], nb_to_check);
    for(int i = 0; i <= nb_to_check; i++){

        //Duree de la formation
        int Tps = formation[i][5] - formation[i][4];//temps de la mission

        //test si la solution est valide
        if(isValid(tempsTravail, i, sol[i], Tps) != 1){
            //Chaque raison pour laquel la solution a ete refuse a renvoie different, rend plus simple l'identification du probleme
            //printf("Not Valid, code de retour : %d\n", isValid(tempsTravail, i, sol[i], Tps));
            return -1; //retourne -1 si non valide
        }

        //incremente le temps de travail
        tempsTravail[formation[i][3]][sol[i]] += Tps;
    }

    //retourne l'evaluation
    return 1;
}


//-----------------------------------SOLUTION----------------------------------------------------------------

void resume_interface(int sol[NBR_FORMATION]){

    int distance_parcour_interface[NBR_INTERFACES] = {0}, temps_travail[NBR_INTERFACES] = {0};

    for(int i = 0; i < NBR_INTERFACES; i++){ //pour chaque interface

        for(int j = 0; j < NBR_FORMATION; j++){ //pour chaque formation
            if(sol[j] == i){ //si l'inteface viser
            
                distance_parcour_interface[i] += distance[i + NBR_CENTRES_FORMATION][formation[j][0] + NBR_CENTRES_FORMATION + NBR_INTERFACES]; //somme des distances parcouru
                
                temps_travail[i] += formation[j][5] - formation[j][4];
            }
        }
        printf("L'interface %d a parcouru %d en distance et travaille %d heures \n\n", i, distance_parcour_interface[i], temps_travail[i]);
    }
}

//calcul le total des distances parcouru
int distance_parcouru_au_total(int sol[NBR_FORMATION]){
    int sum = 0;
    for(int i = 0; i < NBR_FORMATION; i++){
        sum += distance[sol[i] + NBR_CENTRES_FORMATION][formation[i][0] + NBR_CENTRES_FORMATION+NBR_INTERFACES];
    }
    return sum;
}

//Affiche la solution
void afficher_solution_tot(int sol[NBR_FORMATION]){

    printf("Evaluation de la solution : %d\n", evaluation_solution(sol));
    printf("Somme des distances parcouru par les interfaces : %d\n\n", distance_parcouru_au_total(sol));

    resume_interface(sol);

    for(int i = 0; i < NBR_FORMATION; i++){
        printf("___________________Formation : %d / Interface : %d___________________\n", i, sol[i]);
        int dist_interface_apprenant = distance[sol[i] + NBR_CENTRES_FORMATION][formation[i][0] + NBR_CENTRES_FORMATION+NBR_INTERFACES];
        printf("Distance entre interface %d et apprenant %d : %d\n", sol[i], formation[i][0], dist_interface_apprenant);

        if(specialite_interfaces[sol[i]][formation[i][1]] == 1){
            printf("Specialite demander est valide : Oui\n\n");
        }else{
            printf("Specialite demander est valide : Non\n\n");
        }
    }
}

//-----------------------------------EVALUATION ALEATOIRE----------------------------------------------------------------

int* Evaluation_aleatoire(){
    int change = 0;

    //pseudo aleatoire
    srand(time(NULL));

    //pour chaque formation
    for(int i = 0; i < NBR_FORMATION; i++){

        //realise des changement aleatoire
        change = floor(rand() % NBR_INTERFACES); //nouvel interface
        Attribution[i] = change; //changement

        //tant que le changement est non valide, on rechange
        while(partial_eval(Attribution, i) != 1){
            change = floor(rand() % NBR_INTERFACES);
            Attribution[i] = change;
        }
    }
    return Attribution;
}

//-----------------------------------NEAREST NEGHBOR----------------------------------------------------------------

//Plus proche voisins
int* Premiere_Evaluation(){

    int tempsTravail[NBR_JOURS][NBR_INTERFACES] = {{0}};//temps de travail des interfaces, heure de debut et fin d'une formation pour une journe donnée
    double distance_min; //distance minimum (pour la boucle for)
    int new_Interface; //memoire de la meilleur interface

    //plus proche voisin
    for(int num_formation = 0; num_formation < NBR_FORMATION; num_formation++){ //Pour chque formation

        distance_min = 999999999.0;//distance maximum pour initialiser solutions

        for(int num_interface = 0; num_interface < NBR_INTERFACES; num_interface++){ //pour chaque interface

            Attribution[num_formation] = num_interface; //on teste avec l'attribution de la formation num_formatio a l'interface num_interface
            //si la distance est inferieur a la precedente meilleur et que la slution est valide
            if((distance[num_interface + NBR_CENTRES_FORMATION][formation[num_formation][0] + NBR_CENTRES_FORMATION+NBR_INTERFACES] < distance_min) && (partial_eval(Attribution, num_formation) == 1)){

                new_Interface = num_interface; //On met en memoire l'interface
                distance_min = distance[num_interface + NBR_CENTRES_FORMATION][formation[num_formation][0] + NBR_CENTRES_FORMATION+NBR_INTERFACES]; //Nouvelle distance min

            }
        }
        //Attribution de la mission
        Attribution[num_formation] = new_Interface;

        //printf("formation : %d , interface : %d\n", num_formation, new_Interface);
    }

    if(evaluation_solution(Attribution) == -1){ //Evalue la solution,verifie que la solution est valide
        Evaluation_aleatoire(); //trouve une autre solution
    }
    afficher_solution_tot(Attribution); //Affiche la solution

    return Attribution; //retourne l'attribution de base
}

//-----------------------------------RECHERCHE TABOU----------------------------------------------------------------

//Le but est que lors d'un blocage de la solution, de trouver tres vite une autre partie de la solution a explorer
/*
Diversification: un mécanisme pour « forcer » l’exploration de régions jusqu’alors inexplorées
*/
void diversification(){
    //nombre de solution en memoire, et variable avec le changement
    int nb_sol = min(nb_solution_trouver_reel,nb_taille_solution), change;

    //pseudo aleatoire
    srand(time(NULL));

    //pour chaque formation
    for(int i = 0; i < NBR_FORMATION; i++){
        //printf("%d ---> ",bestCandidate[i]);

        /*
        2 solutions pour changer la solution,
            - soit on cherche dans les meilleurs solutions trouver que on a retenu
            - soit on change de maniere aleatoire

        //1er solution:
        change = floor(rand() % nb_sol);
        bestCandidate[i] = solution_[change][i];
        */

        //2e solution :
        change = floor(rand() % NBR_INTERFACES); //nouvel interface
        bestCandidate[i] = change; //changement

        //tant que le changement est non valide, on rechange
        while(evaluation_solution(bestCandidate) == -1){
            change = floor(rand() % NBR_INTERFACES);
            bestCandidate[i] = change;
        }
        //printf("%d\n", bestCandidate[i]);
    }
    
    memset(tabuList, 0, sizeof(tabuList)); 
}

//meilleur solution non tabou et differente de l'actuel
/*
On utilise un critere d'aspiration
Critere d'aspiration :  Si une solution meilleur que toutes les solutions trouver la prend meme si tabou
*/
void getBestNonTabou(int iteration, int best_eval_RT){

    //Solution temporaire
    int temp_sol[NBR_FORMATION];

    int val_exclu = 0; //Valeur exclu car dans la solution actuel
    int eval = 0; //Evaluation de la solution
    int meilleur_eval = 999999999; //meilleur evaluation trouver
    int formation_change = -1, interface_change = -1; //changement dans la solution

    //pour chaque formation
    for(int num_formation = 0; num_formation < NBR_FORMATION; num_formation++){

        //on exclu la solution actuel
        val_exclu = bestCandidate[num_formation];

        //transfert la solution a modifier dans une solution temporaire sur laquel on evalura les changements
        memcpy(temp_sol, bestCandidate, sizeof(int)*(NBR_FORMATION+1));

        //pour chaque interface
        for(int num_interface = 0; num_interface < NBR_INTERFACES; num_interface++){

            //Si formation-interface non exclue et possiblement valide
            if(num_interface != val_exclu && competences_interfaces[num_interface][formation[num_formation][2]] == 1){
            
                //devient solution temporaire pour la tester
                temp_sol[num_formation] = num_interface;
                eval = evaluation_solution(temp_sol); //on evalue cet solution temporaire

                //si elle est valide et inferieur a la meilleur trouver et (non tabou ou meilleur que toutes les solutions trouver)
                if( eval != -1 && eval < meilleur_eval && ( tabuList[num_formation][num_interface] < iteration || eval < best_eval_RT ) ){

                    meilleur_eval = eval; // change la meilleur eval

                    //retien les changements
                    formation_change = num_formation;
                    interface_change = num_interface;

                    //printf("eval : %d\n", eval);
                }
            }
        }
    }

    if(formation_change != -1 && interface_change != -1){
        //change la solution actuel
        bestCandidate[formation_change] = interface_change;
        //Ajoute a la liste tabou le choix
        tabuList[formation_change][interface_change] = iteration + memoire_tabou;
    }else{
        diversification();
    }
}

//algo de la recherche tabou
int* RechercheTabou(){

    printf("\n\n_______________________DEBUT RECHERCHE TABOU_______________________\n\n");

    //pour chaque formation
    for(int i = 0; i < NBR_FORMATION; i++){
        tabuList[i][Attribution[i]] = memoire_tabou; //premiere solution tabou

        //transfere la solution dans la solution optimal et la solution actuel
        sBest[i] = Attribution[i];
        bestCandidate[i] = Attribution[i];
    }

    //evaluation de la solution actuel et de la meilleur solution
    int eval_actuel;
    int best_eval_RT = evaluation_solution(sBest);

    //iteration
    int iter = 1;
    while (iter <= nb_iteration){

        //trouve la meilleur solution non tabou
        getBestNonTabou(iter, best_eval_RT);

        eval_actuel = evaluation_solution(bestCandidate);
        //printf("Eval actuel : %d\n", eval_actuel);

        nb_non_solution++;

        //si meilleur que la meilleur trouver avant alors on change la meilleur en memoire
        if(eval_actuel < best_eval_RT){

            best_eval_RT = eval_actuel; //le meilleur evaluation devient la derniere evaluation
            //indique a l'utilisateur qu'une nouvelle solution a ete trouver
            printf("Nouvelle meilleure solution trouvee a l'iteration %d : %d\n", iter, best_eval_RT);
            //met la derniere solution en tant que meilleur solution
            memcpy(&sBest, &bestCandidate, (NBR_FORMATION+1)*sizeof(int));

            //met la solution en memoire pour l'exploiter possiblement plus tard
            memcpy(&solution_[nb_solution_trouver_reel%nb_taille_solution], &bestCandidate, (NBR_FORMATION+1)*sizeof(int));
            nb_solution_trouver_reel++; //incremente le nombre de solution

            nb_non_solution = 0; //reinitialise le nombre d'iteration sans amelioration
        }

        //si le nombre d'iteration sans amelioration est superieur a celui voulu
        if(nb_non_solution > nb_iter_diversification){
            diversification(); //change la solution
            nb_non_solution = 0; //remet a 0 le nombre d'iteration sans solution
        }

        //iteration suivante
        iter++;
    }

    printf("\n\n_______________________FIN RECHERCHE TABOU_______________________\n\n");

    //affiche la solution
    afficher_solution_tot(sBest);

    printf("Meilleur eval (apres recherce tabou) = %d\n", best_eval_RT);
    return sBest;
}

//-----------------------------------TIMER----------------------------------------------------------------

//Limite l'execution du programme a environs n seconde max
void* Wait_Exit(void* args){
    int time_to_sleep = seconds*1000; //Limite le temps a n seconde

    //dors n seconde
    #ifdef _WIN32  //pour windows (pas les meme bibliotheque et fonctions a utiliser)
    Sleep(time_to_sleep);
    #elif _WIN64  //pour windows (pas les meme bibliotheque et fonctions a utiliser)
    Sleep(time_to_sleep);
    #else //pour le reste
    sleep(seconds);
    #endif

    //Fin de l'execution
    printf("\n\n___________LIMITE %d SECONDES ATTEINTE___________\n\n", seconds);

    //affiche la solution
    afficher_solution_tot(sBest);

    exit(0);//arrete l'execution
}

void Timer(){
    pthread_t timerThread; //creer le thread
    pthread_create(&timerThread, NULL, &Wait_Exit, NULL); //lance le thread
}

//-----------------------------------MAIN----------------------------------------------------------------

int main(int argc, char *argv[]) {

    if(argc > 1){
        printf("(double) Temps en minute par unite de distance (valeur par defaut : %f) : ", Temps_par_unite_distance);
        scanf("%lf", &Temps_par_unite_distance);
        
        printf("(int) Temps d'execution maximum du programme en seconde (valeur par defaut : %d) : ", seconds);
        scanf("%d", &seconds);
        
        printf("(double) Penalite si la specialite n'est pas la bonne (valeur par defaut : %f) : ", penalite_spe);
        scanf("%lf", &penalite_spe);
        
        printf("(double) Penalite en cas d'heure supplementaire d'une interface (valeur par defaut : %f) : ", penalite_heure_sup);
        scanf("%lf", &penalite_heure_sup);
        
        printf("(double) Penalite en cas de mauvaise repartition des missions (valeur par defaut : %f) : ", penalite_repartition);
        scanf("%lf", &penalite_repartition);
        
        printf("(double) Penalite si une interface ne dispose pas de pause le midi (valeur par defaut : %f) : ", penalite_midi);
        scanf("%lf", &penalite_midi);
        
        printf("(double) Penalite si une interface fait trop ou pas assez de distance (valeur par defaut : %f) : ", penalite_distance_dif);
        scanf("%lf", &penalite_distance_dif);
        
        printf("(double) Penalite si une interface a trop ou pas assez d'heure de travail (valeur par defaut : %f) : ", penalite_temps_dif);
        scanf("%lf", &penalite_temps_dif);
        
        printf("(int) Longueur de la liste tabou (valeur par defaut : %d) : ", memoire_tabou);
        scanf("%d", &memoire_tabou);
        
        printf("(int) Nombre d'iteration de la recherche tabou (valeur par defaut : %d) : ", nb_iteration);
        scanf("%d", &nb_iteration);
        
        printf("(int) Nombre d'iteration sans amelioration avant une diversification de la solution (valeur par defaut : %d) : ", nb_iter_diversification);
        scanf("%d", &nb_iter_diversification);
    }

    printf("NBR_INTERFACES=%d\n",NBR_INTERFACES) ;
    printf("NBR_APPRENANTS=%d\n",NBR_APPRENANTS) ;
    printf("NBR_NODES=%d\n",NBR_NODES) ;

    GetDistance();
    printf("Matrice des distances:\n");
    print_matrix(distance);

    Premiere_Evaluation();
    
    Timer();
    
    RechercheTabou();

    return 0 ;
}



