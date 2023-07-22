/* ================================================================================================================================= */
/* ======================================= APPEL DES VARIABLES GLOBALES DU FICHIER PRINCIPAL ======================================= */
/* ================================================================================================================================= */

extern struct T_Spectacle TabSpectacles[];
extern struct T_Representation TabRepresentations[];
extern struct T_Carte TabCartes[];
extern int NbSpectacles;
extern int NbRepresentations;
extern int NbCartes;
extern int affichage_liste;
extern int affichage_liste_selection;  
extern int id_selection;
extern int comptage_places_vendues(struct T_Representation Representation);

/* ================================================================================================================================= */
/* =========================================== FONCTIONS COMMUNES À PLUSIEURS SOUS-MENUS =========================================== */
/* ================================================================================================================================= */

/* ---------------------------------------------------- */
/* -------- GESTION DE L'AFFICHAGE DES LISTES --------- */
/* ---------------------------------------------------- */
/*
	Cette fonction gère l'affichage des données des variables structurées. Son rôle est de changer la valeur des variables globales 
	affichage_liste et affichage_liste_selection, lesquelles sont booléennes, et liées à des conditions situées au niveau des sous-menus ; 
	lorsque la valeur de ces variables globales est à 1, elles activent des conditions, au sein desquelles se trouve l'appel de fonctions 
	affichant en liste de lignes les données des variables structurées. 
	L'argument type_liste détermine le type de liste affiché, et NbObjets est le nombre d'observations compris dans la variable structurée
	que l'on souhaite afficher (NbSpectacles, NbReprésentations, NbCartes).
	La valeur 1 pour l'argument type_liste correspond à l'affichage de la totalité des données de la variable structurée, tandis que
	la valeur 2 correspond à une sélection. affichage_liste_selection est une variable utilisée pour les représentations, si l'on souhaite
	n'afficher que les représentations d'un spectacle en particulier.
	La variable erreur[] quant à elle est l'erreur qui sera affichée dans le cas où NbObjets est égal à 0, où donc qu'il n'y a rien à afficher.
*/
int gestion_affichage_liste(int type_liste, int NbObjets, char erreur[], int reaffichage)
{
	if(type_liste == 1) // Si la liste qui doit être affichée contient toutes les lignes d'une variable structurée, alors...
	{
		if(affichage_liste == 0 && NbObjets != 0) // Si la liste n'est pas affichée et que des objets existent dans la variable structurée, alors...
		{
			affichage_liste = 1; // on affiche la liste
			NETTOYAGE_ECRAN(); // on nettoie le terminal
			reaffichage = 1; // on indique le réaffichage du menu
		}
		else if(affichage_liste == 1) // Si la liste est affichée, alors...
		{
			affichage_liste = 0; // on cache la liste
			NETTOYAGE_ECRAN(); // on nettoie le terminal
			reaffichage = 1; // on indique le réaffichage du menu
		}
		else // Si aucun objet n'existe dans la variable structurée, alors...
		{
			ERREUR(erreur); // on a une erreur
		}
	}
	else // Si la liste qui doit être affichée doit seulement contenir une partie des lignes de la variable structurée, alors...
	{
		if(affichage_liste_selection == 0 && NbObjets != 0)
		{
			affichage_liste_selection = 1;
			NETTOYAGE_ECRAN();
			reaffichage = 1;
		}
		else if(affichage_liste_selection == 1)
		{
			affichage_liste_selection = 0;
			NETTOYAGE_ECRAN();
			reaffichage = 1;
		}
		else
		{
			ERREUR(erreur);
		}
	}
	return reaffichage; // S'il n'y a pas eu d'erreur, la fonction va renvoyer une variable reaffichage contenant 1, et le menu va se réafficher
}

/* ---------------------------------------------------- */
/* ---------------- REDUCTEUR DE NOM  ----------------- */
/* ---------------------------------------------------- */
/*
	Si le nom a un nombre de caractères supérieur à 30, alors la chaîne de sortie renvoiera ses 30 premiers caractères suivis de points de suspension
*/
void reducteur_de_nom(char nom[], char nom_sortie[])
{
    if(strlen(nom) > 30) // si la chaîne a plus de 30 caractères, alors...
    {
        strncpy(nom_sortie, nom, 30); // on copie 30 caractères de cette chaîne
		nom_sortie[30] = '\0'; // on termine la chaîne avec un caractère null pour éviter les bugs
		strcat(nom_sortie,"..."); // on concatène la chaîne avec des points de suspension
    }
    else // sinon on renvoie la chaîne telle quelle
    {
        strcpy(nom_sortie, nom);
    }
}

/* ---------------------------------------------------- */
/* -------------- FONCTION DE VALIDATION -------------- */
/* ---------------------------------------------------- */
/*
	Contrôle de saisie qui force l'utilisateur à valider un choix par OUI ou NON
*/
char controle_validation(char validation[])
{
	while(strcmp(validation,"OUI") != 0 && strcmp(validation,"NON") != 0) // Tant que validation[] ne contient pas OUI ou NON...
		{
     	scanf("%s", validation); //  on resaisie validation
     	min_vers_maj(validation);
			if(strcmp(validation,"OUI") != 0 && strcmp(validation,"NON") != 0) // Si validation n'est toujours pas OUI ou NON, alors une erreur apparaît
			{
				printf(TEXTE_ROUGE);
				printf("Entrée invalide. Saisissez OUI ou NON : ");
				printf(EFFACER_ATTR);
			}
		}
	return validation[10]; // on renvoie validation lorsque finalement elle contient OUI ou NON
}


/* ---------------------------------------------------- */
/* ---- AFFICHAGE DE LA DATE AU FORMAT JJ/MM/AAAA ----- */ 
/* ---------------------------------------------------- */
/*
	Les dates sont enregistrées au format AAAAMMJJ (pour permettre le tri des représentations en fonction des dates). Cette fonction formate
	les dates pour créer des dates de sortie au format JJ/MM/AAAA, dans le cadre de l'affichage des données.
*/
void affichage_date(char date_non_formatee[], char date_sortie[])
{
	 sprintf(date_sortie, "%c%c/%c%c/%c%c%c%c", date_non_formatee[6], date_non_formatee[7], date_non_formatee[4], date_non_formatee[5], date_non_formatee[0], date_non_formatee[1], date_non_formatee[2], date_non_formatee[3]);
}

/* ================================================================================================================================= */
/* ================================================ FONCTIONS SOUS-MENU SPECTACLES ================================================= */
/* ================================================================================================================================= */

/* ---------------------------------------------------- */
/* ---------------- TRI DES SPECTACLES ---------------- */
/* ---------------------------------------------------- */
/*
	Cette fonction est appelée lorsque l'utilisateur a enregistré un nouveau spectacle. Elle trie tous les spectacles dans l'ordre ascendant,
	en fonction de l'ID qui leur a été assigné.
*/
void tri_spectacles()
{
	struct T_Spectacle temp; // variable temporaire utilisée pour garder en mémoire une case du tableau des spectacles
	int i, j;

	for(i = 0; i < NbSpectacles-1; i++)
	{
		for(j = i + 1; j < NbSpectacles; j++)
		{
			if(TabSpectacles[j].id_spectacle < TabSpectacles[i].id_spectacle) // Si le spectacle suivant a un ID plus petit, alors...
			{
				temp = TabSpectacles[i]; // le spectacle précédent est mis dans une variable temporaire
				TabSpectacles[i] = TabSpectacles[j]; // le spectacle suivant est mis dans la case du spectacle précédent
				TabSpectacles[j] = temp; // la case du spectacle suivant prend les valeurs de la variable temporaire, donc du spectacle précédent
			}
		}
	}
}

/* ----------------------------------------------- */
/* ---- AFFICHAGE DE LA SALLE DES SPECTACLES ----- */
/* ----------------------------------------------- */
/*
	La salle des spectacles et des représentations est connue par une variable int détenant trois modalités.
	Cette fonction "traduit" ces modalités numériques pour afficher le nom des salles.
*/
void interprete_salle(int salle, char salle_sortie[])
{
	switch(salle)
	{
		case 1 : strcpy(salle_sortie,"Richelieu")			; // Si salle == 1, alors "Richelieu" est copié dans la variable salle_sortie[]
				 break ;
		case 2 : strcpy(salle_sortie,"Vieux-Colombier")	;
				 break ;
	  	case 3 : strcpy(salle_sortie,"Studio-Theatre")		;
				 break ;
	}
}

/* ----------------------------------------------- */
/* ---- AFFICHER LES RECETTES D'UN SPECTACLE ----- */
/* ----------------------------------------------- */
/*
	Cette fonction somme les recettes des différentes représentations d'un spectacle pour retourner le montant total rapporté
	par le spectacle.
*/
float affichage_recettes_spectacle(int id_spectacle)
{
	int i = 0, j = 0;
	int trouve = 0; // flag
	float recettes = 0;
	int compteur = 0; // pour compter le nombre de représentations du spectacle parcourues

	while(i < NbSpectacles && trouve == 0) // recherche du spectacle dans TabSpectacles[]
	{
		if(TabSpectacles[i].id_spectacle == id_spectacle)
		{
			while(compteur != TabSpectacles[i].nb_representations) // comptage des représentations du spectacle. Quand compteur == TabSpectacles[i].nbrepresentations, le comptage s'arrête
			{
				if(TabRepresentations[j].id_spectacle == id_spectacle)
				{
						recettes += TabRepresentations[j].recettes; // sommage des recettes de toutes les représentations
						compteur++;
				}
				j++;
			}
			trouve = 1;
		}
		i++;
	}
	return recettes;
}

/* ----------------------------------------------- */
/* ---------- AFFICHAGE DES SPECTACLES ----------- */
/* ----------------------------------------------- */
/*
	Cette fonction affiche la liste des spectacles contenus dans le tableau TabSpectacles. A chaque tour de la boucle for(), un nouveau spectacle est affiché
*/
void affichage_spectacles()
{
	char nom_affichage[35]; // contient le nom du spectacle limité à 30 caractères
	char salle_affichage[20]; // contient le nom de la salle du spectacle
	int i;

	printf("-----------------------------------------------------------------------------------------------------------\n");
	printf("%-15s %-35s %-20s %-11s %-20s\n","ID_SPECTACLE","NOM_SPECTACLE","SALLE", "RECETTES", "NB_REPRESENTATIONS");
	printf("-----------------------------------------------------------------------------------------------------------\n");
	for (i = 0 ; i < NbSpectacles ; i++) // affichage d'une ligne par case de TabSpectacles[]
	{
		reducteur_de_nom(TabSpectacles[i].nom,nom_affichage);
		interprete_salle(TabSpectacles[i].salle, salle_affichage);
		printf("%12d    %-35s %-20s %-11.0f %-20d\n",TabSpectacles[i].id_spectacle, nom_affichage, salle_affichage, affichage_recettes_spectacle(TabSpectacles[i].id_spectacle), TabSpectacles[i].nb_representations);
	}
	printf("-----------------------------------------------------------------------------------------------------------\n");
}

/* ================================================================================================================================= */
/* ============================================== FONCTIONS SOUS-MENU REPRÉSENTATIONS ============================================== */
/* ================================================================================================================================= */

/* ---------------------------------------------------- */
/* -------------- TRI DES REPRESENTATIONS ------------- */
/* ---------------------------------------------------- */
/*
	Cette fonction est appelée lorsque l'utilisateur a enregistré une nouvelle représentation. Elle trie toutes les représentations dans l'ordre ascendant,
	en fonction de l'ID de leur spectacle puis de leur date de représentation.
*/
void tri_representations()
{
	struct T_Representation temp; // variable temporaire utilisée pour garder en mémoire une case du tableau des représentations
	int i, j;

	// Tri des représentations en fonction de l'ID de leur spectacle
	for(i = 0; i < NbRepresentations-1; i++)
	{
		for(j = i + 1; j < NbRepresentations; j++)
		{
			if(TabRepresentations[j].id_spectacle < TabRepresentations[i].id_spectacle) // Si la représentation suivante a un ID plus petit, alors...
				{
					temp = TabRepresentations[i]; // la représentation précédente est mise dans une variable temporaire
					TabRepresentations[i] = TabRepresentations[j]; // la représentation suivante est mise dans la case de la représentation précédente
					TabRepresentations[j] = temp; // la case de la représentation suivante prend les valeurs de la variable temporaire, donc de la représentation précédente
				}
		}
	}
	// Tri des représentations en fonction de leur date de représentation
	for(i = 0; i < NbRepresentations-1; i++)
	{
		for(j = i + 1; j < NbRepresentations; j++)
		{
			if(TabRepresentations[j].id_spectacle == TabRepresentations[i].id_spectacle && atoi(TabRepresentations[j].date) < atoi(TabRepresentations[i].date)) // Si l'ID de la représentation suivante est égal à celui de la précédente mais que sa date est antérieure
			{
				temp = TabRepresentations[j];
				TabRepresentations[j] = TabRepresentations[i];
				TabRepresentations[i] = temp;
			}
		}
	}
}

/* ---------------------------------------------------- */
/* ------ AFFICHAGE DU NOMBRE DE PLACES VENDUES ------- */
/* ---------------------------------------------------- */
/*
	Compte combien de places ont été vendues pour une représentation
*/
int comptage_places_vendues(struct T_Representation Representation)
{
	int i, nbventes = 0;

	for(i = 0; i < Representation.nb_places; i++)
	{
		if(Representation.place.etat[i] == 1) // Si Representation.place.etat == 1, alors...
			nbventes++; // c'est que la place est vendue
	}

	return nbventes;
}

/* ---------------------------------------------------- */
/* --- AFFICHAGE DES REPRESENTATIONS (UN SPECTACLE) --- */
/* ---------------------------------------------------- */
/*
	Cette fonction affiche la liste des représentations d'un seul spectacle. A chaque tour de la boucle for(), une nouvelle représentation est affichée
*/
void affichage_selection_representations(int id_selection)
{
	char nom_affichage[35];
	char date_affichage[15];
	char salle_affichage[20];
	int nbventes;
	int i;

	printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("%-20s %-13s %-36s %-12s %-13s %-14s %-12s %-18s %-11s %-15s\n","ID_REPRESENTATION", "DATE", "NOM_SPECTACLE", "NB_PLACES", "NB_VENDUES", "HEURE_DEBUT", "HEURE_FIN", "SALLE", "RECETTES", "ID_SPECTACLE");
	printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	for (i = 0; i < NbRepresentations; i++) // une ligne par représentation
	{
		if(id_selection == TabRepresentations[i].id_spectacle) // seules les représentations dont l'ID correspond à celui recherché sont affichées
		{
			reducteur_de_nom(TabRepresentations[i].spectacle,nom_affichage);
			affichage_date(TabRepresentations[i].date,date_affichage);
			interprete_salle(TabRepresentations[i].salle, salle_affichage);
			nbventes = comptage_places_vendues(TabRepresentations[i]);
			printf("%17d    %-13s %-36s %-12d %-13d %-14s %-12s %-18s %-11.0f %-15d\n",TabRepresentations[i].id_representation, date_affichage, nom_affichage, TabRepresentations[i].nb_places, nbventes, TabRepresentations[i].heure_debut, TabRepresentations[i].heure_fin, salle_affichage, TabRepresentations[i].recettes, TabRepresentations[i].id_spectacle);
		}
	}
	printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

/* ---------------------------------------------------- */
/* ------ AFFICHAGE DES REPRESENTATIONS (TOUTES) ------ */
/* ---------------------------------------------------- */
/*
	Cette fonction affiche la liste des représentations contenus dans le tableau TabRepresentations. A chaque tour de la boucle for(), une nouvelle représentation est affichée
*/
void affichage_toutes_representations()
{
	char nom_affichage[35];
	char date_affichage[15];
	char salle_affichage[20];
	int nbventes;
	int i;

	printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	printf("%-20s %-13s %-36s %-12s %-13s %-14s %-12s %-18s %-11s %-15s\n","ID_REPRESENTATION", "DATE", "NOM_SPECTACLE", "NB_PLACES", "NB_VENDUES", "HEURE_DEBUT", "HEURE_FIN", "SALLE", "RECETTES", "ID_SPECTACLE");
	printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	for (i = 0 ; i < NbRepresentations ; i++) // une ligne par représentation
	{
		reducteur_de_nom(TabRepresentations[i].spectacle,nom_affichage);
		affichage_date(TabRepresentations[i].date,date_affichage);
		interprete_salle(TabRepresentations[i].salle, salle_affichage);
		nbventes = comptage_places_vendues(TabRepresentations[i]);
		printf("%17d    %-13s %-36s %-12d %-13d %-14s %-12s %-18s %-11.0f %-15d\n",TabRepresentations[i].id_representation, date_affichage, nom_affichage, TabRepresentations[i].nb_places, nbventes, TabRepresentations[i].heure_debut, TabRepresentations[i].heure_fin, salle_affichage, TabRepresentations[i].recettes, TabRepresentations[i].id_spectacle);
	}
	printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

 /* ================================================================================================================================= */
/* =================================================== FONCTIONS SOUS-MENU PLACES =================================================== */
/* ================================================================================================================================== */

/* ---------------------------------------------------- */
/* ----- COMPTAGE NOMBRE DE RÉDUCTIONS APPLIQUÉES ----- */
/* ---------------------------------------------------- */
/*
	Cette fonction est utilisée à deux reprises : au sein de l'affichage du menu des places, et lorsque le programme doit savoir si le nombre
	maximum de places à prix réduit pour une représentation est atteint ; d'où l'argument afficher_phrase. Dans le second cas, une annonce est est inutile.
	Si une phrase d'annonce doit être activée, alors l'argument afficher_phrase prend la valeur 1, autrement on lui met une valeur quelconque (comme 0).
*/
int comptage_reductions_places(int id_selection, int afficher_phrase)
{
	int compteur = 0; // pour compter le nombre de places achetées à prix réduit
	int trouve = 0; // flag
	int i, j;

	i = 0;
	while(i < NbRepresentations && trouve == 0) // recherche du spectacle dans TabRepresentations[]
	{
		if(id_selection == TabRepresentations[i].id_representation) // Si l'ID que l'on a entré est égal à l'ID d'une représentation alors...
		{
			for(j = 0; j < TabRepresentations[i].nb_places; j++)  // On vérifie pour chaque place si une réduction a été appliquée ou non
			{
				if(TabRepresentations[i].place.id_carte_reduction[j] != 0) // Si la variable place.id_reduction est différente de 0, c'est qu'elle contient l'ID de la carte CF qui a permis une réduction
				{
					compteur++; // Une place est ajoutée au compteur
				}
			}
			if(afficher_phrase == 1)
			{
				printf(TEXTE_JAUNE);
				printf(" Nombre de places achetées avec une réduction (Max %.0f) : ", TabRepresentations[i].nb_places*MAX_REDUC_POURCENTAGE);
				printf(EFFACER_ATTR);
				printf("%d\n",compteur);
			}
			trouve = 1;
		}
		i++;
	}
	return compteur; // A la fin, le compteur est retourné. Il indique le nombre de places achetées à prix réduit 
}

/* ---------------------------------------------------- */
/* --------------- AFFICHAGE DES PLACES --------------- */
/* ---------------------------------------------------- */
/*
	Cette fonction affiche les places d'une représentation, et colore leur affichage en fonction des données qu'elles contiennent. Leur couleur varie en fonction de leur
	catégorie, et leur clarté varie en fonction de si elles sont vendues ou non. Une place vendue est représentée par le caractère █ tandis qu'une place non vendue est
	représentée par le caractère ░.
	L'usage répété de cette fonction permet au programme d'afficher un plan pour la salle de théâtre d'une représentation.
	
	L'argument numero_representation désigne l'ID de la représentation dont les places sont affichées (voir la fonction affichage_plan_salle()).

	Explication des arguments :
	- debut et fin : si l'argument début est égal à 5, alors on affiche les places à partir de la place 6 (Dans le tableau qui gère les places, le numéro est 5, puisque un tableau 
	  commence à 0), et si l'argument fin est égal à 9, on affiche les places jusqu'à la place 9.
	- espaces_debut : booléen. Mettre 1 lorsque on veut avoir des espaces après la première place affichée par la boucle
	- nombre_espaces_debut : nombre d'espaces à afficher après la première place
	- espaces_optionnels : booléen. Mettre 1 lorsque on veut avoir des espaces à un endroit précis de l'affichage des places
	- position_espaces : les espaces sont placés après la place indiquée
	- nombre_espaces : le nombre d'espaces affiché
	Exemple : si on a mis 8 à position_espaces et 3 à nombre_espaces, 3 espaces seront affichés entre la place 8 et 9
*/
void affichage_places(int numero_representation, int debut, int fin, int espaces_debut, int nombre_espaces_debut, int espaces_optionnels, int position_espaces, int nombre_espaces)
{
	int i, j;

	// --------------- INITIALISATION DE LA BOUCLE CONSTITUANT LA FONCTION ---------------
	for(i = debut; i < fin; i++ ) // Affichage de places allant de place debut à place fin
	{	
		// --------------- ESPACE(S) OPTIONNEL(S) ---------------
		if(espaces_optionnels == 1) // Si espace_optionnels est activé, alors...
		{
			if(i == position_espaces) // quand j est égal à position_espaces, c'est que la boucle est arrivée au moment où elle doit placer des espaces
			{
				for(j = 0; j < nombre_espaces; j++ ) // le nombre d'espaces souhaité s'affiche
				{
					printf(" ");
				}
			}
		}
		// --------------- APPLICATION D'UNE COULEUR À L'AFFICHAGE DE LA PLACE, EN FONCTION DE LA CATÉGORIE DE CELLE-CI ---------------
		if(TabRepresentations[numero_representation].place.categorie[i] == 1) // Si la place est de catégorie A, elle est bleue
		{
			printf(TEXTE_CYANCLAIR);
		}
		else if(TabRepresentations[numero_representation].place.categorie[i] == 2) // Si la place est de catégorie B, elle est jaune
		{
			printf(TEXTE_JAUNEVIF);
		}
		else if(TabRepresentations[numero_representation].place.categorie[i] == 3) // Si la place est de catégorie C, elle est rouge
		{
			printf(TEXTE_ROUGE);
		}

		if(TabRepresentations[numero_representation].place.etat[i] == 0) // Si la place n'a pas été vendue, elle est représentée par un carré sombre
		{
			printf("░ ");
		}
		else // sinon elle sera représentée par un carré clair
		{
			printf("█ ");
		}
		printf(EFFACER_ATTR);

		// --------------- ESPACE(S) OPTIONNEL(S) DE DEBUT ---------------
		if(espaces_debut == 1)
		{
			if(i == debut)
			{
				for(j = 0; j < nombre_espaces_debut; j++)
				{
					printf(" ");
				}
			}
		}
	}
}

/* ----------------------------------------------- */
/* ------ AFFICHAGE DU TARIF DE L'ACHETEUR ------- */
/* ----------------------------------------------- */
/*
	Le tarif des places est connu par une variable int détenant quatre  modalités.
	Cette fonction "traduit" ces modalités numériques pour afficher le nom du tarif.
*/
void interprete_tarif(int tarif, char tarif_sortie[])
{
	switch(tarif)
	{
		case 1 : strcpy(tarif_sortie,"PLEIN")				; // Si tarif == 1, alors "PLEIN" est copié dans la variable tarif_sortie[]
				 break ;
		case 2 : strcpy(tarif_sortie,"-28 ANS")			;
				 break ;
		case 3 : strcpy(tarif_sortie,"DEMANDEUR D'EMPLOI")	;
				 break ;
		case 4 : strcpy(tarif_sortie,"MINIMA SOCIAUX")		;
				 break ;
		case 0 : strcpy(tarif_sortie,"VIDE")				;
				 break ;
	}
}

/* ----------------------------------------------- */
/* -- AFFICHAGE DE L'ETAT DE VENTE D'UNE PLACE --- */
/* ----------------------------------------------- */
/*
	L'état de vente des places est connu par une variable int binaire.
	Cette fonction "traduit" l'état pour afficher le nom du tarif.
*/
void interprete_etat_vente_place(int etat, char etat_sortie[])
{
	switch(etat)
	{
		case 1 : strcpy(etat_sortie,"VENDUE")		; // Si etat == 1, alors "VENDUE" est copié dans la variable etat[]
				 break ;
		case 0 : strcpy(etat_sortie,"NON VENDUE")	;
				 break ;
	}
}

/* ----------------------------------------------- */
/* ---- AFFICHAGE DE LA CATÉGORIE DE LA PLACE ---- */
/* ----------------------------------------------- */
/*
	Cette fonction renvoie le nom de la catégorie de la place en fonction de sa modalité
	et de de la salle dans laquelle elle se trouve.
*/
void interprete_categorie_place(int categorie, int salle, char categorie_sortie[])
 {
	if(salle == 1) // Salle Richelieu = trois catégories
	{
		switch(categorie)
		{
			case 1 : strcpy(categorie_sortie,"CATEGORIE A"); // Si la place concerne une représentation se passant dans la salle Richelieu et qu'elle a 1 dans sa variable categorie, elle est de catégorie A
					break ;
			case 2 : strcpy(categorie_sortie,"CATEGORIE B");
					break ;
			case 3 : strcpy(categorie_sortie,"CATEGORIE C");
					break ;
		}
	}
	else if(salle == 2) // Vieux-Colombier = deux catégories
	{
		switch(categorie)
		{
			case 1 : strcpy(categorie_sortie,"CATEGORIE A");
					break ;
			case 2 : strcpy(categorie_sortie,"CATEGORIE STRAPONTIN");
					break ;
		}
	}
	else // Studio-Théâtre = pas de catégories (placement libre)
	{
		switch(categorie)
		{
			case 1 : strcpy(categorie_sortie,"PLACEMENT LIBRE");
					break ;
		}
	}
 }

 /* ---------------------------------------------------- */
/* ----------- AFFICHER LE TYPE DES CARTES ------------ */
/* ---------------------------------------------------- */
 void interprete_type_carte(int type, char type_sortie[])
 {
	switch(type)
	{
		case 1 : strcpy(type_sortie,"ADULTE")		;
				 break ;
		case 2 : strcpy(type_sortie,"JEUNE")	;
				 break ;
		case 3 : strcpy(type_sortie,"FAMILLE");
	}
 }

 /* ================================================================================================================================= */
/* ================================================= FONCTIONS SOUS-MENU CARTES CF ================================================== */
/* ================================================================================================================================== */

/* ---------------------------------------------------- */
/* ----------- AFFICHER LA LISTE DES CARTES ----------- */
/* ---------------------------------------------------- */
/*
	Cette fonction affiche la liste des cartes CF contenus dans le tableau TabCartes. A chaque tour de la boucle for(), une nouvelle carte est affichée
*/
void affichage_cartes()
{
	char nom_affichage[35];
	char type_affichage[15];
	char date_affichage[15];
	int i;

	printf("-----------------------------------------------------------------------------------\n");
	printf("%-15s %-35s %-11s %-12s\n","ID_CARTE", "NOM_ACHETEUR", "TYPE", "DATE_EXPIRATION");
	printf("-----------------------------------------------------------------------------------\n");
	for (i = 0 ; i < NbCartes ; i++)
	{
		reducteur_de_nom(TabCartes[i].nom_proprietaire,nom_affichage);
		interprete_type_carte(TabCartes[i].type, type_affichage);
		affichage_date(TabCartes[i].date_expiration,date_affichage);
		printf("%-15d %-35s %-11s %-12s\n",TabCartes[i].id_carte, nom_affichage, type_affichage, date_affichage);
	}
	printf("-----------------------------------------------------------------------------------\n");
}

 /* ================================================================================================================================= */
/* ================================================== FONCTIONS SOUS-MENU DONNÉES =================================================== */
/* ================================================================================================================================== */

/* ---------------------------------------------------- */
/* -------------- CHARGEMENT DES DONNÉES -------------- */
/* ---------------------------------------------------- */
void chargement()
{
	struct T_Spectacle Spectacle; // Chaque spectacle va d'abord être chargé dans cette variable
	struct T_Representation Representation; // Chaque représentation va d'abord être chargée dans cette variable
	struct T_Carte Carte; // Chaque carte va d'abord être chargée dans cette variable
	char fichier_spectacles[30] = "CF_spectacles.csv";
	char fichier_representations[30] = "CF_representations.csv";
	char fichier_places[30] = "CF_places.csv";
	char fichier_cartes[30] = "CF_cartes.csv";
	int test_contenu_spectacles, test_contenu_cartes; // permet de vérifier si les fichiers sont vides
	int suivant; // sert dans le chargement des spectacles et cartes
	int i, j;
	FILE *f1; // fichier_spectacles
	FILE *f2; // fichier_representations
	FILE *f3; // fichier_places
	FILE *f4; // fichier_cartes

// --------------- VERIFICATION DE L'EXISTENCE DES FICHIERS ---------------
	f1 = fopen(fichier_spectacles,"r"); // ouverture du fichier des spectacles
	f4 = fopen(fichier_cartes,"r"); // ouverture du fichier des cartes
	if(f1 == NULL && f4 == NULL) // Si le fichier des spectacles et le fichier des cartes n'existent pas, l'ensemble des fichiers est crée (ceux existant déjà sont écrasés)
	{
		ERREURBIS("Fichiers des spectacles et des cartes CF non trouvés ");
		ANNONCEBIS(" Fichiers automatiquement crées :");
		printf(TEXTE_JAUNEVIF);
		ANNONCEBIS(" CF_spectacles.csv \n CF_representations.csv \n CF_places.csv \n CF_cartes.csv \n");
		printf(EFFACER_ATTR);
		// Chaque fichier est crée
		f1 = fopen(fichier_spectacles,"w"); // ouverture en écriture de fichier_spectacles = création de celui-ci
		fclose(f1); // fermeture de fichier_spectacles
		f2 = fopen(fichier_representations,"w"); // création du fichier des représentations
		fclose(f2); // fermeture de fichier_representations
		f3 = fopen(fichier_places,"w"); // création du fichier des places
		fclose(f3); // fermeture de fichier_places
		f4 = fopen(fichier_cartes,"w"); // création du fichier des cartes
		fclose(f4); // fermeture de fichier_cartes
		// On ouvre les fichiers des spectacles et des cartes en lecture, en prévision de la suite de la fonction (du message d'annonce de chargement)
		f1 = fopen(fichier_spectacles,"r");
		f4 = fopen(fichier_cartes,"r");
	}
	else if (f1 == NULL) // Si le fichier des spectacles n'existe pas, il est crée, et ceux des représentations et des places sont crées (ou écrasés, s'ils existent)
	{
		ERREURBIS("fichier des spectacles non trouvé ");
		ANNONCEBIS(" Fichiers automatiquement crées :");
		printf(TEXTE_JAUNEVIF);
		ANNONCEBIS(" CF_spectacles.csv \n CF_representations.csv \n CF_places.csv \n");
		printf(EFFACER_ATTR);
		// Les fichiers des spectacles et ceux qui en découlent sont crées
		f1 = fopen(fichier_spectacles,"w");
		fclose(f1);
		f2 = fopen(fichier_representations,"w");
		fclose(f2);
		f3 = fopen(fichier_places,"w");
		fclose(f3);
		// On ouvre le fichier des spectacles, en prévision de la suite de la fonction (du message de chargement)
		f1 = fopen(fichier_spectacles,"r");
	}
	else if(f4 == NULL) // Si le fichier des cartes n'existe pas, il est crée
	{
		ERREURBIS("Fichier des cartes CF non trouvé ");
		ANNONCEBIS(" Fichier automatiquement crée :");
		printf(TEXTE_JAUNEVIF);
		ANNONCEBIS(" CF_cartes.csv \n");
		printf(EFFACER_ATTR);
		f4 = fopen(fichier_cartes,"w");
		fclose(f4);
		// On ouvre les fichier des cartes, en prévision de la suite de la fonction (du message de chargement)
		f4 = fopen(fichier_cartes,"r");
	}
	test_contenu_spectacles = fgetc(f1); // permet de savoir si le fichier des spectacles est vide
	test_contenu_cartes = fgetc(f4); // permet de savoir si le fichier des cartes est vide
	
	// --------------- MESSAGE D'ANNONCE DE CHARGEMENT --------------- 
	if(test_contenu_spectacles == EOF && test_contenu_cartes == EOF) // Si aucun des fichiers ne contient de données alors...
	{
		printf(FOND_ROUGE);
		printf("\n CF_spectacles.csv et CF_cartes.csv vides : aucune donnée chargée \n");
		printf(EFFACER_ATTR);
		fclose(f1);
		fclose(f4);
	}
	else // Si l'un des fichiers contient des données alors...
	{
		ANNONCEBIS(" Données chargées \n");
		fclose(f1);
		fclose(f4);
	}
	if(test_contenu_spectacles != EOF) // Si le fichier des spectacles n'est pas vide, alors...
	{
		// --------------- CHARGEMENT DES DONNÉES DES SPECTACLES ---------------
		f1 = fopen(fichier_spectacles,"r"); // ouverture du fichier des spectacles en lecture
		i = 0;
		while(!feof(f1)) // Tant que on n'a pas atteint la fin du fichier...
		{
			suivant = fscanf(f1,"%d,%[^,],%d,%d", &Spectacle.id_spectacle, Spectacle.nom, &Spectacle.salle, &Spectacle.nb_representations);
			if (suivant != EOF) // Si la ligne suivante n'est pas la fin du fichier, alors...
			{
				NbRepresentations += Spectacle.nb_representations; // on met à jour le nombre de représentations
				TabSpectacles[i++] = Spectacle; // on fait entrer la ligne dans TabSpectacles[]
			}
		}
		NbSpectacles = i; // mise à jour du nombre de spectacles
		fclose(f1);
		// --------------- CHARGEMENT DES DONNÉES DES REPRÉSENTATIONS ---------------
		f1 = fopen(fichier_representations,"r"); // ouverture du fichier des représentations en lecture
		f2 = fopen(fichier_places,"r"); // ouverture du fichier des places en lecture
		for(i = 0; i < NbRepresentations; i++) // On charge toutes les représentations. On connaît le nombre de représentations grâce aux spectacles
		{
			fscanf(f1,"%d,%[^,],%d,%d,%[^,],%[^,],%[^,],%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &Representation.id_representation, Representation.spectacle, &Representation.salle, &Representation.nb_places, Representation.date, Representation.heure_debut, Representation.heure_fin, &Representation.recettes, &Representation.id_spectacle, &Representation.tarifs_places[0], &Representation.tarifs_places[1], &Representation.tarifs_places[2], &Representation.tarifs_places[3], &Representation.tarifs_places[4], &Representation.tarifs_places[5], &Representation.tarifs_places[6], &Representation.tarifs_places[7], &Representation.tarifs_places[8], &Representation.tarifs_places[9], &Representation.tarifs_places[10], &Representation.tarifs_places[11]);
			
			// --------------- CHARGEMENT DES DONNÉES DES PLACES ---------------
			for(j = 0; j < Representation.nb_places;j++) // On charge toutes les places. On connaît le nombre de places d'une représentation via Representation.nb_places
			{
				fscanf(f2,"%d,%[^,],%d,%d,%d,%f,%d\n", &Representation.place.id_vente[j], Representation.place.nom_acheteur[j], &Representation.place.tarif_acheteur[j], &Representation.place.etat[j], &Representation.place.categorie[j], &Representation.place.prix[j], &Representation.place.id_carte_reduction[j]); 
			}
			TabRepresentations[i] = Representation;
		}		
		fclose(f1);
		fclose(f2);
	}
	if(test_contenu_cartes != EOF) // Si le fichier des cartes n'est pas vide, alors...
	{
		// --------------- CHARGEMENT DES DONNÉES DES CARTES ---------------
		f4 = fopen(fichier_cartes,"r"); // ouverture du fichier des cartes en lecture
		i = 0;
		while(!feof(f4))// Tant que on n'a pas atteint la fin du fichier...
		{
			suivant = fscanf(f4,"%d,%[^,],%[^,],%d", &Carte.id_carte, Carte.nom_proprietaire, Carte.date_expiration, &Carte.type);
			if (suivant != EOF) // Si la ligne suivante n'est pas la fin du fichier, alors...
			{
				TabCartes[i++] = Carte; // on la fait entrer dans TabCartes[]
			}
		}
		NbCartes = i; // mise à jour du nombre des des cartes
		fclose(f4);
	}
}