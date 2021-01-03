PROJET AG41
===========

Rappel du sujet
---------------

L'objectif du projet est de trouver une "bonne" solution sur un probleme d'attribution d'interface (soignant) à des personnes sourde ou aveugle demanant des formations. Une bonne solution comprend plusieurs parametre :
* Une bonne répartition de la charge de travail entre les interfaces
* Une distance de trajet minimal pour toutes les interfaces
* Une correspondance entre la spécialisation des interfaces et des formaions

Solution choisie
----------------

Le projet excute une recherche tabou sur un jeu de donnée. L'objectif est de trouver une "bonne" solution en attribuant aux interfaces des missions en respectant plus ou moins les contraintes.

Le jeu de donnée peut etre modifier, il suffit d'aller dans "Source" puis d'ouvrir et modifier le fichier "data.h"
Les contraintes et parametre de l'execution sont aussi modifiable simplement, il faut se rendre encore une fois dans le dossier "Source" et modifier les variables indiquer comme modifiable dans le fichier "variable.h"

Execution du programme
----------------------

Pour compiler le programme, lancer la commande "./build". /!\ il est necessaire d'avoir GNU /!\\
La compilation se trouvera dans le dossier nomme "Compilation"

Enfin pour lancer le programme compiler, il faut ecrire la commande "./run"
Une fois cet commande executee il sera demander a l'utilisateur s'il souhaite modifier les infomations entrees par defaut ou les garder.
S'il decide de les modifier, le type de la variable ainsi que la valeur par defaut sont indiquees.
