//-----------------------------------VARIABLE GLOBALE AJOUTE----------------------------------------------------------------
/*
/!\     Le nombre de jour doit etre +1 si le premier jour commence a 1     /!\
*/
#define NBR_JOURS              7 //nbr jour au total
#define NBR_COMPETENCE         2 //nbr de competence

//======================= parametre modifiable =======================

//temps par unite de distance (temps en minute) : par exemple un temps 0.1 equivaut a 6 secondes par unite de distance
double Temps_par_unite_distance = 0.1;

//temps d'execution du programme
int seconds = 30;

//penalite
double penalite_spe = ( 125.0 / 100.0 ); //penalite pour non respect de la specialite
double penalite_heure_sup = ( 25.0 / 100.0); //penalite pour des heure suplement (sur une journe ou une semaine)
double penalite_repartition = ( 125.0 / 100.0 ); //penalite pour une mauvaise repartition
double penalite_midi = ( 10.0 / 100.0 ); //penalite si aucune pause le midi
double penalite_distance_dif = ( 125.0 / 100.0 ); //penalite en cas de trop petite ou trop grande distance
double penalite_temps_dif = ( 125.0 / 100.0 ); //penalite en cas de trop peu ou trop grand temps de travails

//parametre recherche tabou
int memoire_tabou = 22; //longueur de la liste tabou
int nb_iteration = 500; //nombre d'iteration de la reccherche
int nb_iter_diversification = 52; //Nombre d'iteration sans nouvelle meilleur solution avant de diversifier la solution
int const nb_taille_solution = 10; //Nombre de meilleur solution memoriser

/*
//======================= variable lier a l'execution /!\ Ne pas modifier /!\ =======================
*/
int Attribution[NBR_FORMATION] = {-1}; //L'attribution des interfaces dans les mission
int bestCandidate[NBR_FORMATION]; //Resultat de la derniere iteration dans la recherche tabou (est initialise plus tard)
int tabuList[NBR_FORMATION][NBR_INTERFACES] = {{0}}; //Liste tabou
double distance[NBR_NODES][NBR_NODES] ; //Matrice des distances
int sBest[NBR_FORMATION];     //meilleur chemin de la recherche tabou
int solution_[nb_taille_solution][NBR_FORMATION] = {{0}}; //Liste des amelioration de solution
int nb_solution_trouver_reel = 0; //nb d'amelioration de la solution
int nb_non_solution = 0; //Nombre d'iteration sans amelioration
