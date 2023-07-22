// POUR UNE LECTURE OPTIMALE DU CODE : AFFICHER L'INDENTATION AVEC 4 ESPACES

/* =========================== */
/* ==== FICHIERS D'ENTÊTE ==== */
/* =========================== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>					// permet au programme de récupérer l'année présente
#include "compil_CF_echap_ansi.h" 	// contient des constantes facilitant l'écriture des séquences d'échappement ANSI

/*
	Le code du programme est décomposé en quatre fichiers :

	1) CF_main.c contient le menu principal, donc le main(), les sous-menus, puis la majorité des fonctions directement appelées par l'utilisateur dans les sous-menus, via les switch().
	2) compil_CF_arriereplan.h contient les fonctions "back", celles qui ne sont pas directement appelées par l'utilisateur.
	3) compil_CF_affichage.h contient les fonctions liées à l'affichage général et l'esthétique du programme, comme NETTOYAGE_ECRAN() ou ERREUR().
	4) compil_CF_echap_ansi.h contient des constantes facilitant l'écriture des séquences d'échappement ANSI, qui sont utilisées pour la gestion de l'affichage.

	REMARQUE : compil_CF_arriereplan.h et compil_CF_affichage.h, sont chargés après la déclaration des structures, puisqu'ils font appel à elles
*/

/* =========================== */
/* ======= CONSTANTES ======== */
/* =========================== */
#define MAX_SPECTACLES 100 			// maximum de spectacles enregistrées
#define MAX_REPRESENTATIONS 2000 	// maximum de représentations enregistrées
#define MAX_CARTES 10000 			// maximum de cartes CF enregistrées
#define MAX_REDUC_POURCENTAGE 0.20	// Pourcentage maximum de places achetables avec une réduction par représentation
#define MAX_TEXTE 30				// maximum de caractères possible pour diverses chaînes de caractères utilisées dans le programme
#define MAX_PLACES 862 				// maximum de places pour une représentation (= le nombre de places de la plus grande des trois salles de la Comédie-Française)

// REMARQUE : A l'exception de la constante MAX_PLACES, ces constantes pourraient être changées sans modification du programme, si le besoin s'en faisait sentir.

/* =========================== */
/* ====== TYPES GLOBAUX ====== */
/* =========================== */

// déclaration de la structure d'un spectacle
struct T_Spectacle
{
	int id_spectacle		;
	char nom[50]			;
	int salle				;
	int nb_representations	;
};

// déclaration de la structure d'une représentation
struct T_Representation
{
	int id_representation	; // héritage de Spectacle. Fait le lien entre les données des structures Spectacle et Representation
	char date[30]			; // format AAAAMMJJ
	char spectacle[50]		; // héritage de Spectacle, équivalent de char nom[30]. Evite de multiplier les boucles pour chercher l'information
	int nb_places			; // dépend de la salle de la représentation (862 = Richelieu, 300 = Vieux-Colombier, 137 = Studio-Théâtre)
	char heure_debut[10]	;
	char heure_fin[10]   	;
	int id_spectacle		;
	int salle				; // héritage de Spectacle, équivalent de int salle. Evite de multiplier les boucles pour chercher l'information
	int tarifs_places[12]	;
	float recettes			; // recettes générée par la représentation
	struct place // déclaration de la sous-structure représentant les différentes places de la représentation
	{
		int id_vente[MAX_PLACES]			; // sert à identifier la place d'un client, dans le cas où deux clients portent le même
		char nom_acheteur[MAX_PLACES][50]	;
		int tarif_acheteur[MAX_PLACES]		;
		int etat[MAX_PLACES]				; // booléen. 1 = vendue, 0 = non vendue
		int categorie[MAX_PLACES]			; // Trois modalités : 1 = A, 2 = B, 3 = B
		float prix[MAX_PLACES]				;
		int id_carte_reduction[MAX_PLACES]	; // 0 par défaut. Les places ayant subi une réduction de prix portent dans cette variable l'id de la carte CF à l'origine de la réduction
	} place;
};

// déclaration de la structure d'une carte CF
struct T_Carte
{
	int id_carte				;
	char nom_proprietaire[50]	;
	int type					;
	char date_expiration[30]	; // format AAAAMMJJ
};

#include "compil_CF_affichage.h"
#include "compil_CF_arriereplan.h"

/* ============================ */
/* ==== VARIABLES GLOBALES ==== */
/* ============================ */

struct T_Spectacle TabSpectacles[MAX_SPECTACLES]				;
struct T_Representation TabRepresentations[MAX_REPRESENTATIONS]	;
struct T_Carte TabCartes[MAX_CARTES]							;
int NbSpectacles = 0		;
int NbRepresentations = 0	;
int NbCartes = 0			;
int annonce_sauvegarde = 0	; // booléen qui passe à 1 lors d'une modification des données ; propose de sauvegarder au moment de quitter le proramme si ça n'a pas été fait


/* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* *! IMPORTANT !* 

	Les variables globales qui suivent ont un rôle dans la gestion de l'affichage. Elles existent parce que le programme utilise très fréquemment la fonction
	NETTOYAGE_ECRAN(), qui permet de totalement nettoyer le terminal dans lequel le programme est lancé. Le nettoyage du terminal permet deux choses : 
	(i) naviguer entre les menus sans que les actions et informations passées demeurent à l'écran et soient affichées plusieurs fois, (ii) permettre une actualisation 
	(quasi) instantanée de l'affichage lors de la modification des données.
	Faire suivre un NETTOYAGE_ECRAN() par un réaffichage du menu permet de faire apparaître à l'écran de nouveaux éléments sans que les élements qui étaient déjà présents 
	avant le nettoyage apparaissent deux fois. Ainsi, par exemple, si l'utilisateur décide d'afficher une liste à l'écran (comme la liste des cartes CF), et qu'il 
	ajoute une nouvelle ligne à cette liste, il ne verra pas une ancienne et une nouvelle liste, il n'en verra qu'une seule, dans laquelle la ligne contenant les données 
	qu'il aura saisies sera automatiquement apparue.

	Ce système de nettoyage et d'actualisation utilisé par le programme explique la présence de l'argument int reaffichage dans un très grand nombre de fonctions.
	La variable reaffichage est présente dans la totalité des menus et permet d'indiquer au programme le moment durant lequel il doit (ré)afficher le menu. Lorsque la
	variable reaffichage contient la valeur 1, cela signifie qu'un menu va être réaffiché (Étant entendu qu'être dans le menu n'est pas afficher les printf() qui le caractérisent). 
	Les menus consistent en des boucles, et à chaque fois que cette boucle fait un nouveau tour, la variable reaffichage revient à 0. Elle repasse à 1 si l'une des fonctions appelées
	dans un menu (ou découlant des fonctions appelées dans ce menu) modifie des données, aboutit à un succès dans la modification ET qu'un élément affichant des données est présent
	à l'écran. Dans ce cas-là, la fonction nettoie l'écran, puis elle retourne une variable reaffichage positionnée sur la valeur 1. Par exemple, il y a nettoyage et réaffichage de 
	l'écran lors de l'ajout d'un spectacle uniquement si la liste des spectacles est affichée.
	Si aucun élément permettant de visualiser des données n'est affiché à l'écran, alors la variable reaffichage ne va pas passer à 1 lors de l'aboutissement d'une fonction, 
	parce qu'il ne sert alors à rien d'actualiser l'affichage de l'écran. Cela permet par ailleurs, dans les menus de gestion des spectacles et des représentations
	de proposer deux modes à l'utilisateur : un mode où il peut visualiser les données via une liste, et un autre où il ne voit pas de liste mais où l'historique de ses 
	actions demeure à l'écran. Ce dernier mode est particulièrement utile dans le cas où de très nombreuses lignes de données sont présentes en mémoire (et prennent ainsi
	beaucoup de place dans la fenêtre de terminal).
	
	La variable reaffichage est nécessaire pour que l'utilisateur puisse voir, s'il le souhaite, un historique de ses actions, mais aussi parce que on ne va pas nettoyer 
	l'écran dans les cas où il est souhaitable que des erreurs apparaissent et soient lisibles (Si une erreur apparaît après une opération mais que l'écran est immédiatement 
	nettoyé, l'utilisateur ne pourra jamais la voir).

	Pour plus de précisions, voir le commentaire de la définition de la fonction gestion_affichage_liste(int type_liste, int NbObjets, char erreur[], int reaffichage)
*/

int affichage_liste				; // booléen pour afficher une liste (spectacles, représentations ou cartes CF). 1 = affichée. 0 = cachée
int affichage_liste_selection	; // booléen pour afficher les représentations d'un seul spectacle.  1 = affichée. 0 = cachée
int id_selection				; // sert dans diverses fonctions pour sélectionner un ID. Par exemple, l'ID d'une représentation, dans le cadre de la modification des places de cette représentation. La variable globale évite de saisir à nouveau l'ID à chaque fois que l'on effectue une nouvelle opération.
int stockage_case				; // sert à stocker la case d'une donnée dans le sous-menu des places, et à afficher les annonces dans le menu des données.

/* ==================================== */ 
/* ==== DÉCLARATIONS DES FONCTIONS ==== */
/* ==================================== */

// ------------------- FONCTIONS CONTENUES DANS : CF_main.c -------------------

// fonctions du menu principal
void menu_spectacles()		;
void menu_representations()	;
void menu_places()			;
void menu_cartes()			;
void menu_donnees()			;
void quitter()				;

// fonctions du sous-menu de gestion des spectacles
int ajout_spectacle(int reaffichage)	;
int retrait_spectacle(int reaffichage)	;

// fonctions du sous-menu de gestion des représentations
int ajout_representation(int reaffichage)			;
int retrait_representation(int reaffichage)			;
int retrait_toutes_representations(int reaffichage)	;
int affichage_representations_type(int reaffichage)	; // faire le choix entre afficher toutes les représentations ou seulement les représentations d'un spectacle
void consultation_tarifs_representation()			; // affiche les tarifs des places d'une représentation en fonction des catégories

// fonctions pour la gestion des places
void vente_place()								;
void annulation_vente_place()					;
int selection_representation(int reaffichage)	; // l'utilisateur sélectionne l'ID de la représentation sur laquelle il souhaite travailler
void consultation_infos_place()					; // affiche l'état et le nom de l'acheteur d'une place
void affichage_plan_salle()						; // affiche le plan de la salle d'une représentation
void recherche_places_client()					; // affiche toutes les places d'un client
void appliquer_reduction()						; // application d'une réduction de carte CF à une place

// fonctions pour la gestion des cartes CF

int vente_carte(int reaffichage)				;
int retrait_carte(int reaffichage)				;
int retrait_toutes_cartes(int reaffichage)		;
void consultation_infos_carte()					; // affiche le type de la carte, le nom de son acheteur, et sa date d'expiration
int selection_date_expiration(int reaffichage)	; // l'utilisateur entre une date d'expiration dont hériteront les cartes CF qui seront entrées par la suite
void recherche_id_carte()						;

// fonctions pour la gestion des données
int sauvegarde(int reaffichage)	; // sauvegarde les données en mémoire dans des fichiers au format .csv
int effacement(int reaffichage)	; // efface les données en mémoire

// ------------------- FONCTIONS CONTENUES DANS : compil_CF_arriereplan.h -------------------

// fonctions communes aux divers sous-menus
int gestion_affichage_liste(int type_liste, int nb_objets, char erreur[], int reaffichage)	; // gère l'affichage des listes
void reducteur_nom_spectacle(char nom_spectacle[], char nom_sortie[])						; // raccourci le nom du spectacle
char controle_validation(char validation[])													; // verifie que l'utilisateur fait une bonne saisie en le forçant à écrire oui ou non
void affichage_date(char date_non_formatee[], char date_sortie[])							; // convertit une date au format AAAAMMJJ vers le format JJ/MM/AAAA

// fonctions liées au sous-menu des spectacles
void tri_spectacles()									; // tri les spectacles en fonction de leur ID, dans l'ordre descendant
void interprete_salle(int salle, char salle_sortie[])	; // affiche le nom de la salle du spectacle
float affichage_recettes_spectacle(int id_spectacle)	; // affiche la somme des recettes de toutes les représentations d'un spectacle
void affichage_spectacles()								; // affichage la liste des spectacles sous forme de tableau

// fonctions liées au sous-menu des représentations
void tri_representations()											; // tri des représentations en fonction des ID des spectacles, puis des dates, dans l'ordre descendant
int comptage_places_vendues(struct T_Representation Representation)	; // permet d'afficher le nombre de places vendues de chaque représentation dans le tableau
void affichage_selection_representations(int id_selection)			; // affiche la liste des représentations d'un spectacle sous forme de tableau
void affichage_toutes_representations()								; // affiche la liste des représentations de tous les spectacles sous forme de tableau

// fonctions liées au sous-menu des places
int comptage_reductions_places(int id_selection, int afficher_phrase)	;
void affichage_places(int numero_representation, int debut, int fin, int espaces_debut, int nombre_espaces_debut, int espaces_optionnels, int position_espaces, int nombre_espaces); // voir le commentaire de la déclaration de la fonction
void interprete_tarif(int selection, char tarif[])						; // affiche le type de l'acheteur de la place, dont dépend le tarif de vente de la place
void interprete_etat_vente_place(int selection, char etat[])			; // affiche si la place est vendue ou non vendue
void interprete_categorie_place(int categorie, int salle, char categorie_sortie[])	; // affiche la catégorie de la place 
void interprete_type_carte(int type, char type_sortie[])				;

// fonctions liées au sous-menu des cartes CF
void affichage_cartes();

// fonction liée au sous-menu des données
void chargement(); // chargement des fichiers de sauvegarde des données

// ------------------- FONCTIONS CONTENUES DANS : compil_CF_affichage.h -------------------

// fonctions d'affichage générales / esthétiques
void DECORATION()							;
void TITRE(char nommenu[])					; 
void ERREUR(char erreur[])					;
void ERREURBIS(char erreur[])				;
void ANNONCE(char annonce[])				;
void ANNONCEBIS(char annonce[])				;
void ATTENTION(char annonce[])				;
void FLECHESAISIE()							; // affiche une flèche qui indique que l'utilisateur doit faire une saisie
void NETTOYAGE_ECRAN()						; // fonction qui nettoie totalement l'affichage du terminal
void min_vers_maj(char string[])			; // conversion des caractères minuscules en majuscules

/* ================================================================================================================================= */
/* ================================================== PROGRAMME / MENU PRINCIPAL =================================================== */
/* ================================================================================================================================= */
int main()
{
	NETTOYAGE_ECRAN(); 	// nettoyage du terminal au lancement du programme
	int choix = -1; 	// initialisation de la boucle du menu
	int reaffichage = 1; // permet de faire réapparaître l'affichage du menu

	while(choix != 0) // Tant que l'utilisateur n'entre pas 0, le programme continue à être exécuté
	{		
		if(reaffichage == 1) // Si reaffichage == 1, le menu s'affiche
		{
			reaffichage = 0; // Reaffichage passe à 0 pour ne pas que le menu s'affiche à nouveau, par exemple si l'utilisateur entre une mauvaise saisie et que s'affiche une erreur

			DECORATION();
			TITRE("   MENU D'ACCUEIL              \n");
			printf("-1- Gérer les spectacles      \n"); 
			printf("-2- Gérer les représentations \n"); 
			printf("-3- Gérer les places          \n");
			printf("-4- Gérer les cartes CF       \n");
			printf("-5- Gestion des données       \n");
			printf("-0- Quitter le programme      \n");
			if(choix == -1)
				chargement(); // chargement des données
			printf("\n");
		}
		FLECHESAISIE();
		scanf("%d",&choix);
		switch(choix) // Les reaffichage = 1 compris dans les cases du switch permettent de réafficher le menu des spectacles lorsque l'utilisateur revient sur celui-ci, depuis un sous-menu
			{
			// --------------- ACCÈS AU MENU DE GESTION DES SPECTACLES ---------------
			case 1 : NETTOYAGE_ECRAN()			;
					 menu_spectacles()			;
					 reaffichage = 1			;
					break ;
			// --------------- ACCÈS AU MENU DE GESTION DES REPRESENTATIONS --------------- 
			case 2 : if(NbSpectacles != 0)
					 {
						NETTOYAGE_ECRAN()		;
						menu_representations()	;
						reaffichage = 1			;
					 }
					 else
					 {
						ERREUR("aucun spectacle enregistré\n");
					 }
					break ;
			// --------------- ACCÈS AU MENU DE GESTION DES PLACES --------------- 
			case 3 : if(NbRepresentations != 0) 
					 {
						NETTOYAGE_ECRAN()		;
						menu_places()			;
						reaffichage = 1			;
					 }
					 else
					 {
					 	ERREUR("aucune représentation enregistrée\n");
					 }
					break ;
			// --------------- ACCÈS AU MENU DE GESTION DES CARTES CF ---------------
			case 4 : NETTOYAGE_ECRAN()			;
					 menu_cartes()				;
					 reaffichage = 1			;
					break ;
			// --------------- ACCÈS AU MENU DE GESTION DES DONNÉES ---------------
			case 5 : NETTOYAGE_ECRAN()			;
					 menu_donnees()				;
					 reaffichage = 1			;
					break ;
			// --------------- QUITTER LE PROGRAMME ---------------
			case 0 : quitter()					;
					break ;
			default: ERREUR("mauvaise entrée\n");
					break ;
			}
	}
}

/* ================================================================================================================================= */
/* ================================================ MENU DE GESTION DES SPECTACLES ================================================= */
/* ================================================================================================================================= */
void menu_spectacles()
{
	int choix = -1;
	int reaffichage = 1;
	affichage_liste = 0; // par défaut, l'affichage de la liste des spectacles n'est pas activé

	while(choix != 0)
	{
		/*
		 	Comme dans le menu principal, il y a un rechargement du menu lorsque la variable reaffichage est positionnée sur 1. Le réaffichage
			de ce sous-menu, comme celui des sous-menus dédiés aux représentations et aux cartes CF, dépend de l'accomplissement d'une fonction
			modifiant les données et de si un affichage des données est activé (liste des spectacles, liste des représentations, liste des carte CF),
			donc de si affichage_liste ou affichage_liste_selection valent 1.
			Puisque il se peut qu'aucune modification de données ne soit effectuée ou que l'affichage des données ne soit pas activé,
			La variable reaffichage revient à 0 à chaque fois que le menu a été affiché, donc que la boucle du menu a fait au moins un tour.
		*/
		if(reaffichage == 1)
		{
			reaffichage = 0;

			DECORATION();
			TITRE("   GESTION DES SPECTACLES                       \n");
			printf("-1- Ajouter un spectacle                       \n");
			printf("-2- Retirer un spectacle                       \n");
			if(affichage_liste == 1)
				printf("-3- Cacher la liste des spectacles         \n");
			else
				printf("-3- Afficher la liste des spectacles       \n");
			printf("-0- Retour au menu principal                   \n");
			printf(TEXTE_JAUNEVIF);
			printf("-11- Nettoyer l'affichage                      \n");
			printf(EFFACER_ATTR);
			if(affichage_liste == 1) // si affichage_liste == 1, la liste des spectacles s'affiche
				affichage_spectacles();
			printf("\n");
		}
		FLECHESAISIE();
		scanf("%d",&choix);
		switch(choix)
		{
			case 1 : reaffichage = ajout_spectacle(reaffichage)		;
					break ;
			case 2 : reaffichage = retrait_spectacle(reaffichage)	;
					break ;
			case 3 : reaffichage = gestion_affichage_liste(1, NbSpectacles, "aucun spectacle enregistré\n", reaffichage); // se référer à la définition de la fonction
					break ;
			case 11: NETTOYAGE_ECRAN()								;
					 reaffichage = 1								;
					break ;
			case 0 : NETTOYAGE_ECRAN()								;
					break ;
			default : ERREUR("mauvaise entrée\n")					;
					break ;
		}
	}
}

/* ----------------------------------------------- */
/* ------------ AJOUT DES SPECTACLES ------------- */
/* ----------------------------------------------- */
int ajout_spectacle(int reaffichage)
{
	struct T_Spectacle Spectacle; // variable temporaire contenant le nouveau spectacle durant la saisie des informations
	int i = NbSpectacles;
	char texte_salle[20]; // sert à afficher le nom de la salle du spectacle
	int j;
	int doublon; // sert pour vérifier les cas de doublon dans la saisie de l'ID

	Spectacle.id_spectacle = -1;
	while (Spectacle.id_spectacle != 0) // Tant que l'utilisateur n'entre pas la valeur 0, il continue à entrer des spectacles
	{
		// --------------- SAISIE DE L'ID SERVANT À DÉSIGNER LE SPECTACLE ---------------
		printf("ID du spectacle (0 pour terminer) : ");
		doublon = 1; // initialisation pour entrer dans la boucle de contrôle de saisie qui vérifie l'absence de doublon
		while(Spectacle.id_spectacle < 1 && Spectacle.id_spectacle != 0 || doublon == 1)
		{
			scanf("%d",&Spectacle.id_spectacle);
			doublon = 0; // on suppose que l'utilisateur n'a pas mis un doublon
			j = 0;
			while(j < NbSpectacles && doublon == 0)
			{
				if(Spectacle.id_spectacle == TabSpectacles[j].id_spectacle)
				{
					doublon = 1;
				}
				j++;
			}
			if (Spectacle.id_spectacle < 1 && Spectacle.id_spectacle != 0)
				ERREUR("Saisissez un autre ID (ID du spectacle inferieur à 1) : ");
			else if(doublon == 1)
				ERREUR("Saisissez un autre ID (ID déjà attribué) : ");
		}
		if (Spectacle.id_spectacle != 0 ) // Si l'utilisateur n'a pas entré 0, il saisie les informations du spectacle
		{
			// --------------- SAISIE DU NOM DU SPECTACLE ---------------
			printf("Nom du spectacle : ")	;
			scanf(" %[^\n]s", Spectacle.nom);
			min_vers_maj(Spectacle.nom); // conversion en majuscules du titre

			// --------------- CHOIX DE LA SALLE DU SPECTACLE ---------------
			printf("Salle [1= Richelieu, 2= Vieux-Colombier, 3= Studio-Theatre] : ");
			scanf("%d",&Spectacle.salle);
			while(Spectacle.salle != 1 && Spectacle.salle != 2 && Spectacle.salle != 3) // contrôle de saisie du numéro correspondant à une salle
			{
				ERREUR("Saisissez un choix correct (numéro de salle inexistant) : ");
				scanf("%d",&Spectacle.salle);
			}
			Spectacle.nb_representations = 0; // initialisation du nombre de représentations
    		TabSpectacles[i++] = Spectacle; // ajout du spectacle au tableau des spectacles
			NbSpectacles = i; // mise à jour du nombre de spectacles
			ANNONCE("Spectacle ajouté\n");
			printf("\n");
			if(affichage_liste == 1) // Une modification des données a été effectuée, le menu va donc être réaffiché si la liste des spectacles est affichée
			{
				reaffichage = 1;
			}
			annonce_sauvegarde = 1;
			tri_spectacles(); // tri des spectacles en fonction de leur ID
		}
	}
	if(reaffichage == 1) // Si le réaffichage du menu est prévu, le terminal est nettoyé
	{
		NETTOYAGE_ECRAN();
	}
	return reaffichage;
}

/* ----------------------------------------------- */
/* ----------- RETRAIT DES SPECTACLES ------------ */
/* ----------------------------------------------- */
int retrait_spectacle(int reaffichage) // yyyyy
{
	int a_supprimer; // ID du spectacle à supprimer
	char validation[10]	= "!"; // sert pour la validation de la suppression
	int trouve = 0;  // flag pour indiquer si le spectacle existe
	int compteur; // pour compter les représentations du spectacle supprimé
	int i, j;

	if(NbSpectacles == 0)
		ERREUR("aucun spectacle enregistré\n"); //
	else
	{
		printf("ID du spectacle à supprimer (0 pour annuler) : "); 
		scanf("%d",&a_supprimer);
		if(a_supprimer != 0)
		{
			// --------------- CONTRÔLE DE SAISIE : l'ID entré est-il valide ? ---------------
			i = 0;
			while(i < NbSpectacles && trouve == 0) // recherche du spectacle dans TabSpectacles[]
			{
				if(a_supprimer == TabSpectacles[i].id_spectacle)
				{
					trouve = 1;
				}
				i++;
			}
			if(trouve == 0)
			{
				ERREUR("ID de spectacle non assigné\n"); // erreur si le spectacle n'est pas trouvé
			}
			else
			{
				// --------------- CONTRÔLE  DE SAISIE : l'utilisateur veut-il vraiment supprimer ? ---------------
				ATTENTION(" TOUTES LES REPRÉSENTATIONS ASSOCIÉES AU SPECTACLE SERONT SUPPRIMÉES. Valider la suppression (OUI/NON) : ");
				controle_validation(validation);
				if(strcmp(validation,"OUI") == 0)
				{
					// --------------- SUPPRESSION DU SPECTACLE ---------------
					i = 0;
					trouve = 0;
					while(i < NbSpectacles && trouve == 0)
					{
						if(a_supprimer == TabSpectacles[i].id_spectacle)
						{
							for(j = i ; j < NbSpectacles-1 ; j++)
							{
								TabSpectacles[j] = TabSpectacles[j+1]; // les spectacles sont décalés d'une place dans le tableau qui les gère, ce qui efface le spectacle sélectionné
							}
							NbSpectacles--; // on décrémente le nombre des spectacles
							// --------------- SUPPRESSION DES REPRÉSENTATIONS LIÉES AU SPECTACLE ---------------
							compteur = 0;
							for(i = 0; i < NbRepresentations; i++)
							{
								if(a_supprimer != TabRepresentations[i].id_spectacle)
								{
									TabRepresentations[compteur] = TabRepresentations[i];
									compteur++;
								}
							}
							NbRepresentations = compteur;
							trouve = 1;
						}
						i++;
					}
						annonce_sauvegarde = 1;
						ANNONCE("Spectacle supprimé\n");
						printf("\n");
						if(affichage_liste == 1) // Si l'opération est menée à bien et que le tableau des spectacles est affiché...
						{
							reaffichage = 1; // alors on va réafficher le menu, ce qui affichera le tableau des spectacles actualisé, avec le nouveau spectacle
						}
						if(NbSpectacles == 0) // Si le nombre de spectacles après la suppression tombe à 0 et que le tableau des représentations est affiché...
						{
							affichage_liste = 0; // alors la variable booléenne qui permet d'afficher le tableau passe à 0.
						}
				}
			}
		}
	}
	if(reaffichage == 1)
	{
		NETTOYAGE_ECRAN(); // l'écran doit être nettoyé avant le réaffichage
	}
	return reaffichage;
}

/* ----------------------------------------------- */
/* ---- AFFICHAGE DE LA LISTE DES SPECTACLES ----- */
/* ----------------------------------------------- */
/*
	 Se référer à la fonction gestion_affichage_liste(int type_liste, int NbObjets, char erreur[], int reaffichage),
	 rangée dans les fonctions communes à plusieurs sous-menus du fichier compil_CF_arriereplan.h
*/

/* ================================================================================= */
/* ===================== MENU DE GESTION DES REPRESENTATIONS ======================= */
/* ================================================================================= */
void menu_representations()
{
	int choix = -1;
	int reaffichage = 1;
	affichage_liste = 0; 
	affichage_liste_selection = 0;

	while(choix != 0)
	{
		if(reaffichage == 1)
		{
			reaffichage = 0;

			DECORATION();
			TITRE("   GESTION DES REPRESENTATIONS                \n");
			printf("-1- Ajouter une représentation               \n");
			printf("-2- Retirer une représentation               \n");
			if(affichage_liste == 1 || affichage_liste_selection == 1)
				printf("-3- Cacher la liste des représentations  \n");
			else
				printf("-3- Afficher la liste des représentations\n");
			printf("-4- Retirer toutes les représentations       \n");
			printf("-5- Consulter les tarifs d'une représentation \n");
			printf("-0- Retour au menu principal                 \n");
			printf(TEXTE_JAUNEVIF);
			printf("-11- Nettoyer l'affichage                    \n");
			printf(EFFACER_ATTR);
			if(affichage_liste == 1)
				affichage_toutes_representations();
			else if(affichage_liste_selection == 1)
				affichage_selection_representations(id_selection);
			printf("\n");
		}
		FLECHESAISIE();

		scanf("%d",&choix);
		switch(choix)
		{
			case 1 : reaffichage = ajout_representation(reaffichage)			;
					break ;
			case 2 : reaffichage = retrait_representation(reaffichage)			;
					break ;
			case 3 : reaffichage = affichage_representations_type(reaffichage)	;
					break ;
			case 4 : reaffichage = retrait_toutes_representations(reaffichage)	;
					break ;
			case 5 : consultation_tarifs_representation()						;
					break ;
			case 0 : NETTOYAGE_ECRAN()											;
					break ;
			case 11: NETTOYAGE_ECRAN()											;
					 reaffichage = 1											;
					break ;
			default : ERREUR("mauvaise entrée\n")								;
					break ;
		}
	}
}

/* ---------------------------------------------------- */
/* ------------ AJOUT DES REPRESENTATIONS ------------- */
/* ---------------------------------------------------- */
int ajout_representation(int reaffichage)
{
	struct T_Representation Representation; // variable temporaire Representation. A la fin de la saisie des données, son contenu sera ajouté à TabRepresentations
	int selection_tarifs_places;
	int i = NbRepresentations;
	char texte_salle[20]; // sert à afficher le nom de la salle du spectacle
	int trouve; // flag pour indiquer si la représentation existe dans le tableau
	int annee, mois, jour, heure, minutes; // servent dans la saisie de la date et de l'heure de représentation
	int doublon;
	int j;

	affichage_spectacles();
	Representation.id_spectacle = -1; // entrée dans la boucle des enregistrements de représentations
	while (Representation.id_spectacle != 0) // Tant que l'utilisateur n'entre pas la valeur 0, il continue à entrer des représentations
	{
		trouve = 0;
		printf("ID du spectacle de la représentation (0 pour terminer) : ");
		scanf("%d", &Representation.id_spectacle);

		// --------------- CONTRÔLE DE SAISIE : l'ID entré est-il valide ? ---------------
		j = 0;
		while(j < NbSpectacles && trouve == 0) // La boucle tourne tant que l'on n'a pas parcouru tout le tableau et que le spectacle que l'on cherche n'a pas été trouvé
		{
			if(Representation.id_spectacle == TabSpectacles[j].id_spectacle) // Si l'ID de spectacle entré correspond bien a un spectacle, alors...
			{
				TabSpectacles[j].nb_representations++; // On incrémente le compteur de représentations du tableau des spectacles à chaque ajout de représentations

				// --------------- HERITAGE DES DONNEES DU SPECTACLE PAR LA REPRESENTATION ---------------
				Representation.salle = TabSpectacles[j].salle;
				strcpy(Representation.spectacle, TabSpectacles[j].nom);
				trouve = 1;
			}
			j++;
		}
		// --------------- ENTRÉE DES INFORMATIONS DE LA REPRÉSENTATION ---------------
		if (trouve == 1)
		{
			// --------------- ENTRÉE DU NOMBRE DE PLACES ET DE LA CATÉGORIE DE CHAQUE PLACE EN FONCTION DE LA SALLE + INITIALISATIONS VALEURS PAR DÉFAUT ---------------
			if (Representation.salle == 1) // Salle Richelieu
			{
				Representation.nb_places = 862;
				for(j = 0; j < Representation.nb_places; j++)
				{
					if(j < 248)
					{
						Representation.place.categorie[j] = 3; // catégorie C
						Representation.place.id_vente[j] = 0; // valeur par défaut
						Representation.place.id_carte_reduction[j] = 0; // valeur par défaut
						strcpy(Representation.place.nom_acheteur[j], "VIDE"); // valeur par défaut
						Representation.place.tarif_acheteur[j] = 0; // valeur par défaut
						Representation.place.etat[j] = 0; // valeur par défaut
						Representation.place.prix[j] = 0; // valeur par défaut
					}
					else if(j < 681)
					{
						Representation.place.categorie[j] = 2; // catégorie B
						Representation.place.id_vente[j] = 0;
						Representation.place.id_carte_reduction[j] = 0;
						strcpy(Representation.place.nom_acheteur[j], "VIDE");
						Representation.place.tarif_acheteur[j] = 0;
						Representation.place.etat[j] = 0;
						Representation.place.prix[j] = 0;
					}
					else
					{
						Representation.place.categorie[j] = 1; // catégorie A
						Representation.place.id_vente[j] = 0;
						Representation.place.id_carte_reduction[j] = 0;
						strcpy(Representation.place.nom_acheteur[j], "VIDE");
						Representation.place.tarif_acheteur[j] = 0;
						Representation.place.etat[j] = 0;
						Representation.place.prix[j] = 0;
					}
				}
			}
			else if (Representation.salle == 2) // Vieux-Colombier
			{
				Representation.nb_places = 300;
				for(j = 0; j < Representation.nb_places; j++)
				{
					if(j == 25 || j == 35 || j == 36 || j == 47 ||j == 66 || j == 67 || j == 81 || j == 82 || 
						j == 83 || j == 84 || j == 98 || j == 99 || j == 100 || j == 101 ||j == 115 || j == 116 || 
						j == 117 || j == 118 || j == 133 || j == 134 || j == 135 || j == 136 || j == 151 || j == 152 || 
						j == 153 || j == 168 || j == 169 || j == 170 || j == 185 || j == 186 || j == 201 || j == 202 || 
						j == 203 || j == 218 || j == 219 || j == 234 || j == 235 || j == 236 || j == 251 || j == 252 || 
						j == 267 || j == 268 || j == 269 || j == 284 || j == 299 )
					{
						Representation.place.categorie[j] = 2; // catégorie B (strapontin)
						Representation.place.id_vente[j] = 0;
						Representation.place.id_carte_reduction[j] = 0;
						strcpy(Representation.place.nom_acheteur[j], "VIDE");
						Representation.place.tarif_acheteur[j] = 0;
						Representation.place.etat[j] = 0;
						Representation.place.prix[j] = 0;
					}
					else
					{
						Representation.place.categorie[j] = 1; // catégorie A
						Representation.place.id_vente[j] = 0;
						Representation.place.id_carte_reduction[j] = 0;
						strcpy(Representation.place.nom_acheteur[j], "VIDE");
						Representation.place.tarif_acheteur[j] = 0;
						Representation.place.etat[j] = 0;
						Representation.place.prix[j] = 0;
					}
				}
			}
			else // Studio-Théâtre
			{
				Representation.nb_places = 136;
				for(j = 0; j < Representation.nb_places; j++)
				{
					Representation.place.categorie[j] = 1; // catégorie unique
					Representation.place.id_vente[j] = 0;
					Representation.place.id_carte_reduction[j] = 0;
					strcpy(Representation.place.nom_acheteur[j], "VIDE");
					Representation.place.tarif_acheteur[j] = 0;
					Representation.place.etat[j] = 0;
					Representation.place.prix[j] = 0;
				}
			}

			// --------------- ENTRÉE DE LA DATE DE LA REPRÉSENTATION ---------------
			ANNONCE("ENTRÉE DE LA DATE DE LA REPRÉSENTATION\n");
			printf("Année : ");
			scanf("%d",&annee);
			time_t heure_actuelle; // récupération de la date et l'heure exactes de l'ordinateur
			time(&heure_actuelle);
			struct tm *temps;
			temps = localtime(&heure_actuelle);
			int annee_actuelle = temps -> tm_year + 1900; // variable dans laquelle est placée l'année à laquelle l'ordinateur exécute le programme. On ne peut pas ajouter de représentations au-delà de 2999, et en-dessous de l'année actuelle-1
			while(annee > 2999 || annee < annee_actuelle-1) // Contrôle de saisie de l'année. On tolère une année avant l'année actuelle, dans le cas où l'utilisateur aimerait ajouter un spectacle passé de la présente saison
			{
				ERREUR("Saisissez une année valide : ");
				scanf("%d",&annee);
			}
			printf("Mois : ");
			scanf("%d",&mois);
			while(mois > 12 || mois < 1) // contrôle de saisie du mois
			{
				ERREUR("Saisissez un mois valide : ");
				scanf("%d",&mois);
			}
			printf("Jour : ");
			scanf("%d",&jour);
			if(mois == 2) // contrôle de saisie du jour
			{
				while(jour > 28 || jour < 1)
				{
					ERREUR("Saisissez un jour valide : ");
					scanf("%d",&jour);
				}
			}
			else if(mois == 1 || mois == 3 || mois == 5 || mois == 7 || mois == 8 || mois == 10 || mois == 12)
			{
				while(jour > 31 || jour < 1)
				{
					ERREUR("Saisissez un jour valide : ");
					scanf("%d",&jour);
				}
			}
			else
			{
				while(jour > 30 || jour < 1)
				{
					ERREUR("Saisissez un jour valide : ");
					scanf("%d",&jour);
				}
			}
			// --------------- PASSAGE EN STRING DES VARIABLES JOUR, MOIS ET ANNEE, ET CONCATÉNATION.
			if(mois < 10 && jour < 10)
			{
				sprintf(Representation.date,"%d0%d0%d",annee,mois,jour);
			}
			else if(mois < 10)
			{
				sprintf(Representation.date,"%d0%d%d",annee,mois,jour);
			}
			else if(jour < 10)
			{
				sprintf(Representation.date,"%d%d0%d",annee,mois,jour);
			}
			else
			{
				sprintf(Representation.date,"%d%d%d",annee,mois,jour);
			}
			// --------------- SAISIE DE L'HORAIRE DE DÉBUT ---------------
			ANNONCE("ENTRÉE DE L'HORAIRE DE DÉBUT\n");
			printf("Heure : ");
			scanf("%d",&heure);
			while(heure > 23 || heure < 0) // contrôle de saisie de l'heure
			{
				ERREUR("Saisissez une heure valide : ");
				scanf("%d",&heure);
			}
			printf("Minute : ");
			scanf("%d",&minutes);
			while(minutes > 60 || minutes < 0) // contrôle de saisie des minutes
			{
				ERREUR("Saisissez une minute valide : ");
				scanf("%d",&minutes);
			}
			if(heure < 10 && minutes < 10)
				sprintf(Representation.heure_debut,"0%d:0%d",heure,minutes);
			else if(heure < 10)
				sprintf(Representation.heure_debut,"0%d:%d",heure,minutes);
			else if(minutes < 10)
				sprintf(Representation.heure_debut,"%d:0%d",heure,minutes);
			else
				sprintf(Representation.heure_debut,"%d:%d",heure,minutes);

			// --------------- SAISIE DE L'HORAIRE DE FIN ---------------
			ANNONCE("ENTRÉE DE L'HORAIRE DE FIN\n");
			printf("Heure : ");
			scanf("%d",&heure);
			while(heure > 23 || heure < 0)
			{
				ERREUR("Saisissez une heure valide : ");
				scanf("%d",&heure);
			}
			printf("Minute : ");
			scanf("%d",&minutes);
			while(minutes > 60 || minutes < 0)
			{
				ERREUR("Saisissez une minute valide : ");
				scanf("%d",&minutes);
			}
			if(heure < 10 && minutes < 10)
				sprintf(Representation.heure_fin,"0%d:0%d",heure,minutes);
			else if(heure < 10)
				sprintf(Representation.heure_fin,"0%d:%d",heure,minutes);
			else if(minutes < 10)
				sprintf(Representation.heure_fin,"%d:0%d",heure,minutes);
			else
				sprintf(Representation.heure_fin,"%d:%d",heure,minutes);

			// --------------- ENTRÉE DES TARIFS DE LA REPRÉSENTATION ---------------
			ANNONCE("ENTRÉE DES TARIFS\n");
			printf("Tarifage [1= tarifs par défaut, 2= tarifs définissables] : ");
			scanf("%d",&selection_tarifs_places);
			while(selection_tarifs_places != 1 && selection_tarifs_places != 2)
			{
				ERREUR("Saisissez un choix correct : ");
				scanf("%d",&selection_tarifs_places);
			}
			if(selection_tarifs_places == 1) // Si l'utilisateur souhaite entrer les tarifs par défaut...
			{
				if(Representation.salle == 1) // Si la salle de la representation est la salle Richelieu
				{
					// assignation tarifs des places en classe A
					Representation.tarifs_places[0] = 49;
					Representation.tarifs_places[1] = 25;
					Representation.tarifs_places[2] = 25;
					Representation.tarifs_places[3] = 16;
					// assignation tarifs des places en classe B
					Representation.tarifs_places[4] = 35;
					Representation.tarifs_places[5] = 18;
					Representation.tarifs_places[6] = 18;
					Representation.tarifs_places[7] = 12;
					// assignation tarifs des places en classe C
					Representation.tarifs_places[8] = 19;
					Representation.tarifs_places[9] = 11;
					Representation.tarifs_places[10] = 11;
					Representation.tarifs_places[11] = 6;
				}
				else if(Representation.salle == 2) // Si la salle de la representation est le Vieux-Colombier
				{
					// assignation tarifs des places en classe A
					Representation.tarifs_places[0] = 35;
					Representation.tarifs_places[1] = 18;
					Representation.tarifs_places[2] = 18;
					Representation.tarifs_places[3] = 12;
					// assignation tarifs des places en classe B
					Representation.tarifs_places[4] = 31;
					Representation.tarifs_places[5] = 18;
					Representation.tarifs_places[6] = 18;
					Representation.tarifs_places[7] = 12;
					// cases vides initialisées pour éviter les bugs lors du chargement des données
					Representation.tarifs_places[8] = 0;
					Representation.tarifs_places[9] = 0;
					Representation.tarifs_places[10] = 0;
					Representation.tarifs_places[11] = 0;
				}
				else // Si la salle de la representation est le Studio-Théâtre
				{
					// assignation tarifs des places
					Representation.tarifs_places[0] = 27;
					Representation.tarifs_places[1] = 17;
					Representation.tarifs_places[2] = 17;
					Representation.tarifs_places[3] = 12;
					// cases vides initialisées pour éviter les bugs lors du chargement des données
					Representation.tarifs_places[4] = 0;
					Representation.tarifs_places[5] = 0;
					Representation.tarifs_places[6] = 0;
					Representation.tarifs_places[7] = 0;
					Representation.tarifs_places[8] = 0;
					Representation.tarifs_places[9] = 0;
					Representation.tarifs_places[10] = 0;
					Representation.tarifs_places[11] = 0;
				}
			}
			else // Si l'utilisateur entre un tarif particulier pour la representation...
			{
				if(Representation.salle == 1) // Si la salle de la representation est la salle Richelieu
				{
					ANNONCE("Prix des places en classe A : \n");
					printf("Tarif plein : ");
					scanf("%d",&Representation.tarifs_places[0]);
					printf("Tarif -28 ans : ")	;
					scanf("%d",&Representation.tarifs_places[1]);
					printf("Tarif demandeur d'emploi : ");
					scanf("%d",&Representation.tarifs_places[2]);
					printf("Tarif minima sociaux : ");
					scanf("%d",&Representation.tarifs_places[3]);
					ANNONCE("Prix des places en classe B : \n");
					printf("Tarif plein : ");
					scanf("%d",&Representation.tarifs_places[4]);
					printf("Tarif -28 ans : ")	;
					scanf("%d",&Representation.tarifs_places[5]);
					printf("Tarif demandeur d'emploi : ");
					scanf("%d",&Representation.tarifs_places[6]);
					printf("Tarif minima sociaux : ");
					scanf("%d",&Representation.tarifs_places[7]);
					ANNONCE("Prix des places en classe C : \n");
					printf("Tarif plein : ");
					scanf("%d",&Representation.tarifs_places[8]);
					printf("Tarif -28 ans : ")	;
					scanf("%d",&Representation.tarifs_places[9]);
					printf("Tarif demandeur d'emploi : ");
					scanf("%d",&Representation.tarifs_places[10]);
					printf("Tarif minima sociaux : ");
					scanf("%d",&Representation.tarifs_places[11]);
				}
				else if(Representation.salle == 2) // Si la salle de la représentation est le Vieux-Colombier
				{
					ANNONCE("Prix des places en classe A : \n");
					printf("Tarif plein : ");
					scanf("%d",&Representation.tarifs_places[0]);
					printf("Tarif -28 ans : ")	;
					scanf("%d",&Representation.tarifs_places[1]);
					printf("Tarif demandeur d'emploi : ");
					scanf("%d",&Representation.tarifs_places[2]);
					printf("Tarif minima sociaux : ");
					scanf("%d",&Representation.tarifs_places[3]);
					ANNONCE("Prix des places en classe B : \n");
					printf("Tarif plein : ");
					scanf("%d",&Representation.tarifs_places[4]);
					printf("Tarif -28 ans : ")	;
					scanf("%d",&Representation.tarifs_places[5]);
					printf("Tarif demandeur d'emploi : ");
					scanf("%d",&Representation.tarifs_places[6]);
					printf("Tarif minima sociaux : ");
					scanf("%d",&Representation.tarifs_places[7]);
				}
				else // Si la salle de la représentation est le Studio-Théâtre
				{
					ANNONCE("Prix des places : \n");
					printf("Tarif plein : ");
					scanf("%d",&Representation.tarifs_places[0]);
					printf("Tarif -28 ans : ")	;
					scanf("%d",&Representation.tarifs_places[1]);
					printf("Tarif demandeur d'emploi : ");
					scanf("%d",&Representation.tarifs_places[2]);
					printf("Tarif minima sociaux : ");
					scanf("%d",&Representation.tarifs_places[3]);
				}	
			}


			// --------------- GÉNÉRATION ALÉATOIRE D'UN ID SERVANT À DÉSIGNER LA REPRÉSENTATION + CONTRÔLE DES DOUBLONS ---------------
			doublon = 1; // initialisation pour entrer dans la boucle de contrôle
			while(doublon == 1)
			{
				Representation.id_representation = rand();
				doublon = 0; // on suppose qu'il n'y a pas de doublon
				j = 0;
				while(j < NbRepresentations && doublon == 0)
				{
					if(Representation.id_representation == TabRepresentations[j].id_representation)
					{
						doublon = 1;
					}
					j++;
				}
			}
			// --------------- INITIALISATION DES RECETTES ---------------
			Representation.recettes = 0;

			// --------------- AJOUT DE LA REPRESENTATION AU TABLEAU DES REPRÉSENTATIONS ---------------
			TabRepresentations[i++] = Representation;
			NbRepresentations = i; // mise à jour du nombre de représentations
			ANNONCE(" Représentation ajoutée\n");
			printf("\n");
			if(affichage_liste == 1 || affichage_liste_selection == 1)
			{
				reaffichage = 1;
			}
			annonce_sauvegarde = 1;
			tri_representations(); // tri des représentations
		}
		else if(trouve == 0 && Representation.id_spectacle != 0)
			ERREUR("ID de spectacle non assigné\n");
	}
	if(reaffichage == 1)
	{
		NETTOYAGE_ECRAN();
	}
	return reaffichage;
}

/* ---------------------------------------------------- */
/* ----------- RETRAIT DES REPRESENTATIONS ------------ */
/* ---------------------------------------------------- */
int retrait_representation(int reaffichage)
{
	int a_supprimer; // id de la representation à supprimer
	int id_representation;
	int id_spectacle;
	int trouve = 0; // flag annonçant que la représentation est trouvée
	int confirmation = 0; // booléen. Permet d'annoncer s'il n'y a pas eu de suppression
	int i, j;

	if(NbRepresentations == 0)
	{
		ERREUR("aucune représentation enregistrée\n");
	}
	else
	{
		printf("ID de la représentation à supprimer (0 pour annuler) : ");
		scanf("%d",&a_supprimer);
		if(a_supprimer != 0)
		{
			i = 0;
			while(i < NbRepresentations && confirmation == 0)
			{
				// --------------- DÉCRÉMENTATION DU NOMBRE DE REPRÉSENTATIONS DU SPECTACLE ---------------
				if(a_supprimer == TabRepresentations[i].id_representation) // Lorsque la représentation est trouvée dans TabRepresentations[]
				{
					id_spectacle = TabRepresentations[i].id_spectacle; // On garde en mémoire l'ID du spectacle de la représentation
					for(j = 0 ; j < NbSpectacles ; j++) // On parcourt tous les spectacles
					{
						if(TabSpectacles[j].id_spectacle == TabRepresentations[i].id_spectacle) // Lorsque le spectacle de la représentation est trouvé dans TabSpectacles[]
						{
							TabSpectacles[j].nb_representations--; // on décrémente le nombre de représentations pour le spectacle
						}
					}
				
					id_representation = i; // on garde en mémoire l'ID de la représentation
					
					// ---------------EFFACEMENT DE LA REPRÉSENTATION DU TABLEAU DES REPRÉSENTATIONS ---------------
					for(j = id_representation ; j < NbRepresentations-1 ; j++)
					{
						TabRepresentations[j] = TabRepresentations[j+1]; // Chaque représentation est décalée dans la case qui la précède dans le tableau
					}
					NbRepresentations--; // décrémentation de la variable globale du nombre de représentations
					confirmation = 1;
					ANNONCE(" Représentation supprimée\n");
					printf("\n");
					annonce_sauvegarde = 1;
					// --------------- GESTION DE L'AFFICHAGE DE LA LISTE DES REPRÉSENTATIONS ---------------
					if(affichage_liste == 1 || affichage_liste_selection == 1)
					{
						reaffichage = 1;
					}
					if(NbRepresentations == 0) // Si le nombre de spectacles après la suppression tombe à 0 et que le tableau des représentations est affiché...
					{
						affichage_liste = 0; // alors la variable booléenne qui permet d'afficher le tableau passe à 0.
					}
					if(affichage_liste_selection == 1)
					{
						j = 0;
						while(j < NbRepresentations && trouve == 0)
						{
							if(id_spectacle == TabRepresentations[j].id_spectacle)
							{
								trouve = 1;
							}
							j++;
						}
						if(trouve == 0)
						{
							affichage_liste_selection = 0;
						}
					}
				}
				i++;
			}
			if(confirmation == 0)
			{
				ERREUR("ID de représentation non assigné\n");
			}
		}
	}
	if(reaffichage == 1)
	{
		NETTOYAGE_ECRAN();
	}
	return reaffichage;
}

/* ---------------------------------------------------- */
/* ------ RETRAIT DE TOUTES LES REPRESENTATIONS ------- */
/* ---------------------------------------------------- */
int retrait_toutes_representations(int reaffichage)
{
	char validation[10] = "!";
	int i;

	if(NbRepresentations == 0)
	{
		ERREUR("aucune représentation enregistrée\n");
	}
	else
	{
		ATTENTION(" Valider le retrait de toutes les représentations (OUI/NON) : ");
		controle_validation(validation);
		if(strcmp(validation,"OUI") == 0)
		{
			for(i = 0; i < NbSpectacles; i++)
			{
				TabSpectacles[i].nb_representations = 0;
			}
			NbRepresentations = 0;
			ANNONCE(" Toutes les représentations ont été supprimées\n");
			printf("\n");
			if(affichage_liste == 1 || affichage_liste_selection == 1)
			{
				affichage_liste = 0;
				affichage_liste_selection = 0;
				reaffichage = 1;
			}
			annonce_sauvegarde = 1;
		}
	}
	if(reaffichage == 1)
	{
		NETTOYAGE_ECRAN();
	}
	return reaffichage;
}

/* ---------------------------------------------------- */
/* ---------- SELECTION DU TYPE D'AFFICHAGE ----------- */
/* ---------------------------------------------------- */
int affichage_representations_type(int reaffichage)
{
	int selection_type_affichage;
	int trouve = 0; // flag annonçant que le spectacle est trouvé
	int i;

	if(NbRepresentations == 0) // S'il n'y a aucune représentation la fonction sort une erreur car elle n'a aucune donnée à afficher
	{
		ERREUR("aucune représentation enregistrée\n");
	}
	else if(affichage_liste == 1 || affichage_liste_selection == 1) // Si l'affichage de la liste est activé, alors on le désactive
	{
		affichage_liste = 0; // mise à zéro des deux variables affichant la liste
		affichage_liste_selection = 0;
		reaffichage = 1; // réaffichage
		NETTOYAGE_ECRAN(); // nettoyage
	}
	else // Si l'affichage n'est pas activé, l'utilisateur a le choix entre son mode d'affichage
	{
		ANNONCE("Pour afficher toutes les représentations, entrer 1. Pour afficher celles d'un seul spectacle, entrer 2 : ");
		scanf("%d",&selection_type_affichage);
		while(selection_type_affichage != 1 && selection_type_affichage != 2) // contrôle de saisie
		{
			ERREUR("Saisie invalide (choix inexistant) : ");
			scanf("%d",&selection_type_affichage);
		}
		if(selection_type_affichage == 1) // si l'utilisateur veut afficher toutes les représentations, gestion_affichage_liste aura son premier argument sur 1, donc passera affichage_liste à 1
		{
			reaffichage = gestion_affichage_liste(1, NbRepresentations, "aucune représentation enregistrée", reaffichage);
		}
		else // sinon, l'utilisateur doit indiquer de quel spectacle il souhaite voir les représentations
		{
			affichage_spectacles(); // affichage de la liste des spectacles pour qu'il puisse voir leurs ID
			ANNONCE("ID du spectacle dont il faut afficher les représentations (0 pour annuler) : "); // entrée de l'ID du spectacle dont l'utilisateur souhaite voir les représentations
			scanf("%d",&id_selection);
			i = 0;
			while(i <= NbSpectacles && trouve == 0) // Le tableau des représentations est parcouru tant que le spectacle correspondant à l'ID n'est pas trouvé
			{
				if(id_selection == TabSpectacles[i].id_spectacle)
				{
					trouve = 1; // passage à 1 du flag, si le spectacle est trouvé
					if(TabSpectacles[i].nb_representations == 0)
					{
						trouve = 2; // passage à 2 du flag, si le spectacle est trouvé n'a aucune représentation enregistrée 
					}
				}
				i++;
			}
			if(trouve == 2 && id_selection != 0) // Si le spectacle a été trouvé mais qu'il n'a aucune représentation, alors...
			{
				ERREUR("aucune représentation enregistrée pour ce spectacle\n");
			}
			else if(trouve == 0 && id_selection != 0) // Sinon, si l'ID du spectacle indiqué ne correspond à aucun spectacle, alors...
			{
				ERREUR("ID de spectacle non assigné\n");
			}
			else if(id_selection != 0) // Sinon, si l'utilisateur n'a pas entré 0, la fonction gestion_affichage_liste va être appelée
			{
				reaffichage = gestion_affichage_liste(2, NbRepresentations, "aucune représentation enregistrée", reaffichage); // elle affichera toutes les représentations du spectacle dont l'ID a été stocké dans la variable id_selection
			}
		}
	}
	return reaffichage;
}

/* ----------------------------------------------- */
/* ---- CONSULTER LE PRIX D'UNE REPRÉSENTATION ---- */
/* ----------------------------------------------- */
void consultation_tarifs_representation()
{
	int a_consulter; // ID du spectacle que l'on souhaite consulter
	char date_affichage[15];
	int trouve = 0; // flag pour indiquer si le spectacle existe dans le tableau
	int i;

	if(NbRepresentations == 0)
		ERREUR("aucune représentation enregistrée\n");
	else
	{
		printf("ID de la représentation dont il faut afficher les tarifs (0 pour annuler) : ");
		scanf("%d",&a_consulter);
		if(a_consulter != 0)
		{
			i = 0;
			while(i <= NbRepresentations && trouve == 0) // Tant que le tableau des représentations n'a pas été parcouru entièrement et que la représentation n'a pas été trouvée...
			{
				if(a_consulter == TabRepresentations[i].id_representation) // Si la représentation est trouvée, alors...
				{
					printf("\n");
					printf(FOND_BLEU);
					printf(" PRIX DES PLACES POUR : "); 
					printf(TEXTE_JAUNEVIF);
					affichage_date(TabRepresentations[i].date,date_affichage);
					printf("%s, le %s (%d) ", TabRepresentations[i].spectacle, date_affichage, TabRepresentations[i].id_representation);
					printf(EFFACER_ATTR);
					printf("\n\n");
					if(TabRepresentations[i].salle == 1) // Si la salle de la représentation est Richelieu...
					{
						ANNONCE("Prix des places en classe A : \n"); // tous les tarifs sont affichés
						printf("Tarif plein : %d\n",TabRepresentations[i].tarifs_places[0]);
						printf("Tarif -28 ans : %d\n",TabRepresentations[i].tarifs_places[1]);
						printf("Tarif demandeur d'emploi : %d\n",TabRepresentations[i].tarifs_places[2]);
						printf("Tarif minima sociaux : %d\n",TabRepresentations[i].tarifs_places[3]);
						ANNONCE("Prix des places en classe B : \n");
						printf("Tarif plein : %d\n",TabRepresentations[i].tarifs_places[4]);
						printf("Tarif -28 ans : %d\n",TabRepresentations[i].tarifs_places[5]);
						printf("Tarif demandeur d'emploi : %d\n",TabRepresentations[i].tarifs_places[6]);
						printf("Tarif minima sociaux : %d\n",TabRepresentations[i].tarifs_places[7]);
						ANNONCE("Prix des places en classe C : \n");
						printf("Tarif plein : %d\n",TabRepresentations[i].tarifs_places[8]);
						printf("Tarif -28 ans : %d\n",TabRepresentations[i].tarifs_places[9]);
						printf("Tarif demandeur d'emploi : %d\n",TabRepresentations[i].tarifs_places[10]);
						printf("Tarif minima sociaux : %d\n",TabRepresentations[i].tarifs_places[11]);
					}
					else if(TabRepresentations[i].salle == 2) // Vieux-Colombier
					{
						ANNONCE("Prix des places en classe A : \n");
						printf("Tarif plein : %d\n",TabRepresentations[i].tarifs_places[0]);
						printf("Tarif -28 ans : %d\n",TabRepresentations[i].tarifs_places[1]);
						printf("Tarif demandeur d'emploi : %d\n",TabRepresentations[i].tarifs_places[2]);
						printf("Tarif minima sociaux : %d\n",TabRepresentations[i].tarifs_places[3]);
						ANNONCE("Prix des places en classe B : \n");
						printf("Tarif plein : %d\n",TabRepresentations[i].tarifs_places[4]);
						printf("Tarif -28 ans : %d\n",TabRepresentations[i].tarifs_places[5]);
						printf("Tarif demandeur d'emploi : %d\n",TabRepresentations[i].tarifs_places[6]);
						printf("Tarif minima sociaux : %d\n",TabRepresentations[i].tarifs_places[7]);
					}
					else // Studio-Théâtre
					{
						ANNONCE("Prix des places : \n");
						printf("Tarif plein : %d\n",TabRepresentations[i].tarifs_places[0]);
						printf("Tarif -28 ans : %d\n",TabRepresentations[i].tarifs_places[1]);
						printf("Tarif demandeur d'emploi : %d\n",TabRepresentations[i].tarifs_places[2]);
						printf("Tarif minima sociaux : %d\n",TabRepresentations[i].tarifs_places[3]);
					}
					trouve = 1;
					printf("\n");
				}
				i++;
			}
			if(trouve == 0)
			{
				ERREUR("ID de représentation non assigné\n"); 
			}
		}
	}
}

/* ================================================================================= */
/* ========================== MENU DE GESTION DES PLACES =========================== */
/* ================================================================================= */
void menu_places()
{
	int choix = -1;
	int reaffichage = 1;
	char date_affichage[15];
	id_selection = 0;
	stockage_case = 0;

	while(choix != 0)
	{
		if(reaffichage == 1)
		{
			reaffichage = 0;

			DECORATION();
			TITRE("   GESTION DES PLACES                           \n");
			printf("-1- Ajouter une place vendue                   \n");
			printf("-2- Annuler la vente d'une place               \n");
			ANNONCE("-3- Sélection de la représentation à modifier\n");
			printf("-4- Afficher le plan de la salle               \n");
			printf("-5- Consulter les informations d'une place     \n");
			printf("-6- Afficher toutes les places d'un client     \n");
			printf("-7- Appliquer une réduction de carte CF        \n");
			printf("-0- Retour au menu principal                   \n");
			printf(TEXTE_JAUNEVIF);
			printf("-11- Nettoyer l'affichage                      \n");
			printf(EFFACER_ATTR);
			if(id_selection != 0)
			{
				ANNONCE("\n ID de la représentation sélectionnée : "); 
				affichage_date(TabRepresentations[stockage_case].date,date_affichage);
				printf("%d\n", id_selection);
				comptage_reductions_places(id_selection, 1);
				ANNONCE(" Spectacle : ");
				printf("%s ",TabRepresentations[stockage_case].spectacle);
				ANNONCE("| Date : ");
				printf("%s\n", date_affichage);
			}
			printf("\n");
		}
		FLECHESAISIE();

		scanf("%d",&choix);
		switch(choix)
		{
			case 1 : vente_place();					 	
					break;
			case 2 : annulation_vente_place();
					break;
			case 3 : reaffichage = selection_representation(reaffichage);
					break;
			case 4 : affichage_plan_salle();
					break;
			case 5 : consultation_infos_place();
					break;
			case 6 : recherche_places_client();
					break;
			case 7 : appliquer_reduction();
					break;
			case 11 :NETTOYAGE_ECRAN();
					 reaffichage = 1;
					break;
			case 0 : NETTOYAGE_ECRAN();
					break;
			default: ERREUR("mauvaise entrée\n");
					break;
		}
	}
}

/* ---------------------------------------------------- */
/* ------------ AJOUTER UNE PLACE VENDUE -------------- */
/* ---------------------------------------------------- */
/*
	Les traitements des places se font par place-1 car l'utilisateur entre le le numéro de la place, tandis que le tableau qui gère les places commence à 0.
*/
void vente_place()
{
	int place = -1; // place à vendre. initialisée à -1 pour entrer dans la boucle
	char acheteur[35]; // contient le nom de l'acheteur de la place
	int doublon; // sert à vérifier si l'ID de place généré est un doublon
	int i;

	if(id_selection == 0)
	{
		ERREUR("aucune représentation sélectionnée\n");
	}
	else
	{
		while(place != 0) // l'utilisateur continue de noter vendue des places tant qu'il n'entre pas 0
		{
			affichage_plan_salle(id_selection);
			if(place != -1)
			{
				if(place > TabRepresentations[stockage_case].nb_places || place < 1) // la condition d'erreur se place ici, autrement elle ne pourrait pas être affichée, à cause de l'effacement qui permet le rafraichissement
				{
					printf(CLIGNOTEMENT);
					ERREUR("numéro de place inexistant\n\n");
					printf(EFFACER_ATTR);
				}
				else if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				{
					printf(CLIGNOTEMENT);
					ERREUR("cette place a déjà été vendue\n\n");
					printf(EFFACER_ATTR);
				}
			}
			printf(" Numéro de la place vendue (0 pour terminer) : ");
			scanf("%d",&place);
			if(place != 0 && place > 0 && place <= TabRepresentations[stockage_case].nb_places && TabRepresentations[stockage_case].place.etat[place-1] == 0) // Si le numéro de la place n'est pas 0, est supérieur à 0 et que la place existe dans la représentation, et qu'elle n'est pas vendue, alors...
			{
				TabRepresentations[stockage_case].place.etat[place-1] = 1;
				printf(" Prénom et nom de l'acheteur : ");
				scanf(" %[^\n]s", acheteur);
				min_vers_maj(acheteur);
				strcpy(TabRepresentations[stockage_case].place.nom_acheteur[place-1], acheteur);
				printf(" Tarif de l'acheteur (1 = plein, 2 = -28 ans, 3 = demandeur d'emploi, 4 = minima sociaux) : ");
				scanf("%d",&TabRepresentations[stockage_case].place.tarif_acheteur[place-1]);
				while(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] != 1 && TabRepresentations[stockage_case].place.tarif_acheteur[place-1] != 2 && TabRepresentations[stockage_case].place.tarif_acheteur[place-1] != 3 && TabRepresentations[stockage_case].place.tarif_acheteur[place-1] != 4)
				{
					ERREUR("Saisissez un choix correct : ");
					scanf("%d",&TabRepresentations[stockage_case].place.tarif_acheteur[place-1]);
				}

				if(TabRepresentations[stockage_case].place.categorie[place-1] == 1) // si la place appartient à la catégorie A, alors...
				{
					if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 1) // si le tarif est plein, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[0];
					else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 2) // si le tarif est -28 ans, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[1];
					else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 3) // si le tarif est demandeur d'emploi, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[2];
					else // si le tarif est minima sociaux, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[3];
				}
				else if(TabRepresentations[stockage_case].place.categorie[place-1] == 2) // sinon, si la place appartient à la catégorie B, alors...
				{
					if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 1) // si le tarif est plein, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[4];
					else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 2) // si le tarif est -28 ans, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[5];
					else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 3) // si le tarif est demandeur d'emploi, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[6];
					else // si le tarif est minima sociaux, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[7];
				}
				else
				{
					if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 1) // si le tarif est plein, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[8];
					else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 2) // si le tarif est -28 ans, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[9];
					else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 3) // si le tarif est demandeur d'emploi, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[10];
					else // si le tarif est minima sociaux, alors...
						TabRepresentations[stockage_case].place.prix[place-1] = TabRepresentations[stockage_case].tarifs_places[11];
				}

				// GÉNÉRATION ALÉATOIRE D'UN ID SERVANT À DÉSIGNER LA VENTE, AVEC CONTRÔLE DES DOUBLONS
				doublon = 1; // initialisation pour entrer dans la boucle de contrôle
				while(doublon == 1)
				{
					TabRepresentations[stockage_case].place.id_vente[place-1] = rand();
					doublon = 0;
					i = 0;
					while(i < TabRepresentations[stockage_case].nb_places && doublon == 0)
					{
						if(TabRepresentations[stockage_case].place.id_vente[place-1] == TabRepresentations[stockage_case].place.id_vente[i] && place-1 != i )
						{
							doublon = 1;
						}
						i++;
					}
				}

				// AJOUT DU PRIX DE LA PLACE AUX RECETTES DE LA REPRESENTATION
				TabRepresentations[stockage_case].recettes+= TabRepresentations[stockage_case].place.prix[place-1];

				place = -1; // pour relancer la boucle, si l'utilisateur souhaite continuer à utiliser la fonction
				annonce_sauvegarde = 1;
			}
		}
		printf("\n");
	}
}

/* ---------------------------------------------------- */
/* ----------- ANNULER LA VENTE D'UNE PLACE ----------- */
/* ---------------------------------------------------- */
void annulation_vente_place()
{
	int place = -1;

	if(id_selection == 0)
	{
		ERREUR("aucune représentation sélectionnée\n");
	}
	else
	{
		while(place != 0) // l'utilisateur continue d'annuler des places tant qu'il n'entre pas 0
		{
			affichage_plan_salle(id_selection);
			if(place != -1)
			{
				if(place > TabRepresentations[stockage_case].nb_places || place < 1) // la condition d'erreur se place ici, autrement elle ne pourrait pas être affichée, à cause de l'effacement qui permet le rafraichissement
				{
					printf(CLIGNOTEMENT);
					ERREUR("numéro de place inexistant\n\n");
					printf(EFFACER_ATTR);
				}
				else if(TabRepresentations[stockage_case].place.etat[place-1] == 0)
				{
					printf(CLIGNOTEMENT);
					ERREUR("cette place n'a pas été vendue\n\n");
					printf(EFFACER_ATTR);
				}
			}
			printf(" Numéro de la place dont la vente est annulée (0 pour terminer) : ");
			scanf("%d",&place);
			if(place != 0 && place > 0 && place <= TabRepresentations[stockage_case].nb_places)
			{
				if(TabRepresentations[stockage_case].place.etat[place-1] == 1) // si la place est vendue, alors...
				{
					// SOUSTRACTION DU PRIX DE LA PLACE AUX RECETTES DE LA REPRESENTATION
					TabRepresentations[stockage_case].recettes-= TabRepresentations[stockage_case].place.prix[place-1];

					// RÉINITIALISATION VERS LES VALEURS PAR DÉFAUT DE LA PLACE
					strcpy(TabRepresentations[stockage_case].place.nom_acheteur[place-1], "VIDE");
					TabRepresentations[stockage_case].place.tarif_acheteur[place-1] = 0;
					TabRepresentations[stockage_case].place.etat[place-1] = 0;
					TabRepresentations[stockage_case].place.prix[place-1] = 0;
					TabRepresentations[stockage_case].place.id_vente[place-1] = 0;
					TabRepresentations[stockage_case].place.id_carte_reduction[place-1] = 0;

					place = -1; // pour relancer la boucle, si l'utilisateur souhaite continuer à utiliser la fonction
					annonce_sauvegarde = 1;
				}
			}
		}
	printf("\n");
	}
}

/* ---------------------------------------------------- */
/* SELECTION DE LA REPRESENTATION DES PLACES A MODIFIER */
/* ---------------------------------------------------- */
int selection_representation(int reaffichage)
{
	int trouve = 0; // flag
	int id_spectacle = 0; // sert dans le cas où l'utilisateur choisit de ne voir les représentations que d'un seul spectacle
	int i;
	char validation[10] = "!";
	int type_liste;
	int temp_id_selection;
	int temp_stockage_case; // yyyyy

	temp_id_selection = id_selection; // sert dans le cas où l'utilisateur souhaite sortir de la fonction, en entrant 0 ; garde le dernier ID entré.
	temp_stockage_case = stockage_case; // garde la position de la sélection dans le tableau
	
	ANNONCE("Pour afficher toutes les représentations, entrer 1. Pour afficher celles d'un seul spectacle, entrer 2 : ");
	scanf("%d",&type_liste);
	while(type_liste != 1 && type_liste != 2)
	{
		ERREUR("Saisie invalide (choix inexistant) : ");
		scanf("%d",&type_liste);
	}
	if(type_liste == 1) // voir les ID de toutes les représentations 
	{
		affichage_toutes_representations(); 
	}
	else // ne voir les ID que d'une représentation en particulier
	{
		affichage_spectacles();
		ANNONCE("ID du spectacle dont il faut afficher les représentations (0 pour annuler) : ");
		scanf("%d",&id_spectacle);
		i = 0;
		while(i < NbSpectacles && trouve == 0) // recherche du spectacle dans TabSpectacles[]
		{
			if(id_spectacle == TabSpectacles[i].id_spectacle) // Si le spectacle est trouvé, alors...
			{
				trouve = 1; // le flag prend la valeur 1
				if(TabSpectacles[i].nb_representations == 0) // Si le spectacle n'a aucune représentation, alors...
				{
					trouve = 2; // le flag prend la valeur 2
				}
			}
			i++;
		}
		if(trouve == 1)
		{
			affichage_selection_representations(id_spectacle);
		}
		else if(trouve == 2)
		{
			ERREUR("aucune représentation enregistrée pour ce spectacle\n");
		}
		else if(trouve == 0 && id_spectacle != 0)
		{
			ERREUR("ID de spectacle non assigné\n");
		}
	}
	if(type_liste == 1 || trouve == 1)
	{
		ANNONCE("Sélectionner une représentation (0 pour annuler) : ");
		scanf("%d",&id_selection);
		trouve = 0;
		while(trouve != 1)
		{
			i = 0;
			while(i < NbRepresentations && trouve == 0)
			{
				if(id_selection == TabRepresentations[i].id_representation || id_selection == 0)
				{
					trouve = 1;
					stockage_case = i;
				}
				i++;
			}
			if(trouve != 1)
			{
				ERREUR("Saisissez un ID existant (0 pour annuler) : ");
				scanf("%d",&id_selection);
			}
		}

	}
		if(id_selection == 0 && temp_id_selection != 0) // si l'utilisateur a entré 0 à la sélection d'ID de représentation, le programme, garde en mémoire l'ID qui avait été précédemment entré
		{
			id_selection = temp_id_selection;
			stockage_case = temp_stockage_case;
		}
		if(id_selection != 0 && id_selection != temp_id_selection) // si l'ID entré n'est pas le même que le précédent, alors...
		{
			reaffichage = 1;
			NETTOYAGE_ECRAN();
		}

	return reaffichage;
}

/* ---------------------------------------------------- */
/* ----------- AFFICHER LE PLAN DE LA SALLE ----------- */
/* ---------------------------------------------------- */
/*
	Fonction qui affiche le plan de chaque salle en fonction de la représentation sur laquelle l'utilisateur a choisi de travailler. S'il n'a pas au préalable
	fait la sélection d'une représentation dans la fonction selection_representation(), alors il ne peut pas accéder à cette fonction.
	Le plan est généré avec des caractères rectangulaires, qui représentent chacun une place. Chaque place dispose d'une couleur particulière en fonction de sa
	catégorie, et elle est claire ou foncé en fonction de si elle est vendue ou non.
*/
void affichage_plan_salle()
{
	char date_affichage[15];
	int i = 0;
	int trouve = 0;

	if(id_selection == 0)
	{
		ERREUR("aucune représentation sélectionnée\n");
	}
	else
	{
		NETTOYAGE_ECRAN(); // permet l'actualisation instantanée du plan lors d'une modification des données
	/*
		L'affichage du plan de la salle a un fonctionnement différent de celui des listes de données, puisqu'il ne s'agit pas d'un affichage permanent 
		dépendant d'une variable et d'une condition dans un menu.
		Cette fonction a été créée non seulement pour que l'utilisateur puisse visualiser l'état de ventes des places, mais pour qu'il puisse instantanément 
		voir chaque changement opéré sur une place. Son intérêt réside dans le fait qu'aussitôt que l'utilisateur a appuyé sur entrée, il voit
		la place qu'il a sélectionnée s'éclairer (si elle est vendue) ou s'assombrir (si sa vente est annulée) ; là où, par exemple, dans la fonction 
		ajout_spectacle il est nécessaire, pour voir la modification de l'affichage des données, de quitter cette boucle sur laquelle repose la fonction,
		qui ne s'arrête pas tant que l'utilisateur souhaite continuer à entrer des données et qu'il n'a pas appuyé sur 0.
		La mise à jour d'un affichage nécessite un effacement puis un réaffichage, sauf qu'il est impossible de les opérer correctement au sein d'une fonction
		ayant pour caractéristique de se répéter pour l'entrée ou la modification de plusieurs données, car cela effacerait les lignes qui composent le menu 
		depuis lequel elle aura été appelée. Pour palier à ce problème, il faut soit modifier la fonction pour qu'elle ne permette qu'une modification à la fois, 
		soit admettre une nécessaire redondance en copiant dedans les lignes qui composent le menu qui l'appelle. C'est ce qui a été fait ici.
	*/
		// --------------- FAUX MENU ---------------
		DECORATION();
		TITRE("   GESTION DES PLACES                           \n");
		printf("-1- Ajouter une place vendue                   \n");
		printf("-2- Annuler la vente d'une place               \n");
		ANNONCE("-3- Sélection de la représentation à modifier\n");
		printf("-4- Afficher le plan de la salle               \n");
		printf("-5- Consulter les informations d'une place     \n");
		printf("-6- Afficher toutes les places d'un client     \n");
		printf("-7- Appliquer une réduction de carte CF        \n");
		printf("-0- Retour au menu principal                   \n");
		printf(TEXTE_JAUNEVIF);
		printf("-11- Nettoyer l'affichage                      \n");
		printf(EFFACER_ATTR);
		if(id_selection != 0)
		{
			ANNONCE("\n ID de la représentation sélectionnée : "); 
			affichage_date(TabRepresentations[stockage_case].date,date_affichage);
			printf("%d\n", id_selection);
			comptage_reductions_places(id_selection, 1);
			ANNONCE(" Spectacle : ");
			printf("%s ",TabRepresentations[stockage_case].spectacle);
			ANNONCE("| Date : ");
			printf("%s\n", date_affichage);
		}

		// --------------- ENTÊTE DE LA SALLE DE LA REPRÉSENTATION ---------------
		printf("\n  ");
		printf(TEXTE_BLEU);
		printf(FOND_BLANC);
		printf(CLIGNOTEMENT);
		if(TabRepresentations[stockage_case].nb_places == 300)
			printf("        THEATRE DU VIEUX-COLOMBIER       \n");
		else if(TabRepresentations[stockage_case].nb_places == 862)
			printf("        SALLE RICHELIEU       \n");
		else
			printf("        STUDIO-THÉÂTRE       \n");
		printf(EFFACER_ATTR);
		printf("\n");
		if(TabRepresentations[stockage_case].nb_places == 862)
		{
			printf(TEXTE_ROUGE);
			printf(" ░ "); 
			printf(EFFACER_ATTR);
		}
		if(TabRepresentations[stockage_case].nb_places == 300 || TabRepresentations[stockage_case].nb_places == 862)
		{
			printf(TEXTE_JAUNEVIF);
			printf(" ░ "); 
			printf(EFFACER_ATTR);
		}
		printf(TEXTE_CYANCLAIR);
		printf(" ░ ");
		printf(EFFACER_ATTR); 
		printf("= NON VENDUE ");
		if(TabRepresentations[stockage_case].nb_places == 862)
		{
			printf(TEXTE_ROUGE);
			printf("█"); 
			printf(EFFACER_ATTR);
			printf(" = VENDUE (CAT C) "); 
		}
		if(TabRepresentations[stockage_case].nb_places == 300 || TabRepresentations[stockage_case].nb_places == 862)
		{
			printf(TEXTE_JAUNEVIF);
			printf("█"); 
			printf(EFFACER_ATTR);
			printf(" = VENDUE (CAT B/STRAPONTIN) "); 
		}
		printf(TEXTE_CYANCLAIR);
		printf("█"); 
		printf(EFFACER_ATTR);
		printf(" = VENDUE (CAT A/PLACEMENT LIBRE) "); 


		printf("\n\n");

		/*
			Chaque appel de la fonction affichage_places() représente une ligne de places affichée dans le terminal.
			Les paramètres de affichage_place() sont expliqués au début de la définition de la fonction.
		*/

		// --------------- AFFICHAGE DU THÉÂTRE DU VIEUX-COLOMBIER ---------------
		if(TabRepresentations[stockage_case].nb_places == 300)
		{
			printf("                 1 à 9 ");
			affichage_places(stockage_case, 0, 9, 0, 0, 0, 0, 0)		; // affiche les places 1 à 9 de la salle (0 à 8 dans le tableau)
			printf("\n\n                 10 à 16 ");
			affichage_places(stockage_case, 9, 16, 0, 0, 0, 0, 0)		;
			printf("\n\n               17 à 25 ");
			affichage_places(stockage_case, 16, 25, 0, 0, 0, 0, 0)		;
			printf("\n\n             26 à 36 ");
			affichage_places(stockage_case, 25, 36, 0, 0, 0, 0, 0)		;
			printf("\n\n             37 à 48 ");
			affichage_places(stockage_case, 36, 48, 0, 0, 0, 0, 0)		;
			printf("\n\n               49 à 58 ");
			affichage_places(stockage_case, 48, 58, 0, 0, 0, 0, 0)		;
			printf("\n\n                 59 à 66 ");
			affichage_places(stockage_case, 58, 66, 0, 0, 0, 0, 0)		;
			printf("\n\n      67 à 83 ");
			affichage_places(stockage_case, 66, 83, 1, 1, 1, 82, 3)		;
			printf("\n\n    84 à 100 ");
			affichage_places(stockage_case, 83, 100, 1, 2, 1, 99, 3)	;
			printf("\n\n  101 à 117 ");
			affichage_places(stockage_case, 100, 117, 1, 3, 1, 116, 3)	;
			printf("\n\n 118 à 135 ");
			affichage_places(stockage_case, 117, 135, 1, 4, 1, 134, 1)	;
			printf("\n\n 136 à 152 ");
			affichage_places(stockage_case, 135, 152, 1, 4, 1, 151, 3)	;
			printf("\n\n 153 à 169 ");
			affichage_places(stockage_case, 152, 169, 1, 4, 0, 0, 0)	;
			printf("\n\n 170 à 185 ");
			affichage_places(stockage_case, 169, 185, 1, 4, 0, 0, 0)	;
			printf("\n\n 186 à 202 ");
			affichage_places(stockage_case, 185, 202, 1, 4, 0, 0, 0)	;
			printf("\n\n 203 à 218 ");
			affichage_places(stockage_case, 202, 218, 1, 4, 0, 0, 0)	;
			printf("\n\n 219 à 235 ");
			affichage_places(stockage_case, 218, 235, 1, 4, 0, 0, 0)	;
			printf("\n\n 236 à 251 ");
			affichage_places(stockage_case, 235, 251, 1, 4, 0, 0, 0)	;
			printf("\n\n 252 à 268 ");
			affichage_places(stockage_case, 251, 268, 1, 4, 0, 0, 0)	;
			printf("\n\n 269 à 284 ");
			affichage_places(stockage_case, 268, 284, 1, 4, 0, 0, 0)	;
			printf("\n\n       285 à 300 ");
			affichage_places(stockage_case, 284, 300, 0, 0, 0, 0, 0)	;
			printf("\n\n");
		}
		// --------------- AFFICHAGE DE LA SALLE RICHELIEU ---------------
		else if(TabRepresentations[stockage_case].nb_places == 862)
		{
			printf("        1 à 60 ");
			affichage_places(stockage_case, 0, 9, 0, 0, 0, 0, 0)		;
			printf("  ");
			affichage_places(stockage_case, 9, 22, 0, 0, 0, 0, 0)		;
			printf("  ");
			affichage_places(stockage_case, 22, 38, 0, 0, 0, 0, 0)		;
			printf("  ");
			affichage_places(stockage_case, 38, 51, 0, 0, 0, 0, 0)		;
			printf("  ");
			affichage_places(stockage_case, 51, 60, 0, 0, 0, 0, 0)		;
			printf("\n\n      61 à 118 ");
			affichage_places(stockage_case, 60, 68, 0, 0, 0, 0, 0)		;
			printf("  ");
			affichage_places(stockage_case, 68, 81, 0, 0, 0, 0, 0)		;
			printf("    ");
			affichage_places(stockage_case, 81, 97, 0, 0, 0, 0, 0)		;
			printf("    ");
			affichage_places(stockage_case, 97, 110, 0, 0, 0, 0, 0)		;
			printf("  ");
			affichage_places(stockage_case, 110, 118, 0, 0, 0, 0, 0)	;
			printf("\n\n");
			printf("  						           119 à 126 ");
			affichage_places(stockage_case, 118, 122, 0, 0, 0, 0, 0)	;
			printf("      ");
			affichage_places(stockage_case, 122, 126, 0, 0, 0, 0, 0)	;
			printf("\n\n 127 à 189 ");
			affichage_places(stockage_case, 126, 137, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 137, 150, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 150, 166, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 166, 179, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 179, 189, 0, 0, 0, 0, 0)	;
			printf("\n\n 190 à 248 ");
			affichage_places(stockage_case, 189, 200, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 200, 211, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 211, 227, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 227, 238, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 238, 248, 0, 0, 0, 0, 0)	;
			printf("\n\n\n");
			printf(" 249 à 311 ");
			affichage_places(stockage_case, 248, 259, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 259, 272, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 272, 288, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 288, 301, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 301, 311, 0, 0, 0, 0, 0)	;
			printf("\n\n 312 à 370 ");
			affichage_places(stockage_case, 311, 322, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 322, 333, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 333, 349, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 349, 360, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 360, 370, 0, 0, 0, 0, 0)	;
			printf("\n\n\n");
			printf(" 371 à 433 ");
			affichage_places(stockage_case, 370, 381, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 381, 394, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 394, 410, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 410, 423, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 423, 433, 0, 0, 0, 0, 0)	;
			printf("\n\n 434 à 496 ");
			affichage_places(stockage_case, 433, 444, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 444, 457, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 457, 473, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 473, 486, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 486, 496, 0, 0, 0, 0, 0)	;
			printf("\n\n 497 à 559 ");
			affichage_places(stockage_case, 496, 507, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 507, 520, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 520, 536, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 536, 549, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 549, 559, 0, 0, 0, 0, 0)	;
			printf("\n\n 560 à 622 ");
			affichage_places(stockage_case, 559, 570, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 570, 583, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 583, 599, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 599, 612, 0, 0, 0, 0, 0)	;
			printf("  ");
			affichage_places(stockage_case, 612, 622, 0, 0, 0, 0, 0)	;
			printf("\n\n 623 à 681 ");
			affichage_places(stockage_case, 622, 633, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 633, 644, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 644, 660, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 660, 671, 0, 0, 0, 0, 0)	;
			printf("    ");
			affichage_places(stockage_case, 671, 681, 0, 0, 0, 0, 0)	;
			printf("\n\n\n");
			printf("	 682 à 742 ");
			affichage_places(stockage_case, 681, 742, 0, 0, 0, 0, 0)	;
			printf("\n\n	 743 à 803 ");
			affichage_places(stockage_case, 742, 803, 0, 0, 0, 0, 0)	;
			printf("\n\n	   804 à 862 ");
			affichage_places(stockage_case, 803, 862, 0, 0, 0, 0, 0)	;
			printf("\n\n");

		}
		// --------------- AFFICHAGE DU STUDIO-THÊATRE ---------------
		else
		{
			printf("   1 à 9 ");
			affichage_places(stockage_case, 0, 9, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   10 à 18 ");
			affichage_places(stockage_case, 9, 18, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   18 à 27 ");
			affichage_places(stockage_case, 18, 27, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   28 à 36 ");
			affichage_places(stockage_case, 27, 36, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   37 à 46 ");
			affichage_places(stockage_case, 36, 46, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   47 à 57 ");
			affichage_places(stockage_case, 46, 57, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   58 à 67 ");
			affichage_places(stockage_case, 57, 67, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   68 à 79 ");
			affichage_places(stockage_case, 67, 79, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("   80 à 91 ");
			affichage_places(stockage_case, 79, 91, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf("  92 à 103 ");
			affichage_places(stockage_case, 91, 103, 0, 0, 0, 0, 0)		;
			printf("\n\n");
			printf(" 104 à 115 ");
			affichage_places(stockage_case, 103, 115, 0, 0, 0, 0, 0)	;
			printf("\n\n");
			printf(" 116 à 126 ");
			affichage_places(stockage_case, 115, 126, 0, 0, 0, 0, 0)	;
			printf("\n\n");
			printf(" 127 à 136 ");
			affichage_places(stockage_case, 126, 136, 0, 0, 0, 0, 0)	;
			printf("\n\n");
		}
	}
}

/* ---------------------------------------------------- */
/* ------ AFFICHER LES INFORMATIONS D'UNE PLACE ------- */
/* ---------------------------------------------------- */
void consultation_infos_place()
{
	int place;
	char texte_etat[MAX_TEXTE]	;
	char texte_tarif[MAX_TEXTE]	;
	char texte_categorie[MAX_TEXTE];

	if(id_selection == 0)
	{
		ERREUR("aucune représentation sélectionnée\n");
	}
	else
	{
		// --------------- RECHERCHE DE LA REPRÉSENTATION DANS LE TABLEAU ---------------
		printf("Numéro de la place : "); 
		scanf("%d",&place); // saisie du numéro de la place
		if(place > 0 && place <= TabRepresentations[stockage_case].nb_places)
		{
			printf("\n");
			printf(FOND_BLEU);
			printf(" PLACE %d POUR LA REPRESENTATION : ", place); 
			printf(TEXTE_JAUNEVIF);
			printf("%d ", TabRepresentations[stockage_case].id_representation);
			printf(EFFACER_ATTR);
			printf("\n\n");
			// --------------- AFFICHAGE DES INFORMATIONS DE LA PLACE ---------------
			printf("ID de vente :");
			if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				printf(TEXTE_VERTCLAIR);
			else
				printf(TEXTE_JAUNEVIF);
			printf(" %d\n",TabRepresentations[stockage_case].place.id_vente[place-1]);
			printf(EFFACER_ATTR);
			printf("Nom de l'acheteur :");
			if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				printf(TEXTE_VERTCLAIR);
			else
				printf(TEXTE_JAUNEVIF);
			printf(" %s\n",TabRepresentations[stockage_case].place.nom_acheteur[place-1]);
			printf(EFFACER_ATTR);
			interprete_tarif(TabRepresentations[stockage_case].place.tarif_acheteur[place-1],texte_tarif);
			printf("Tarif :");
			if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				printf(TEXTE_VERTCLAIR);
			else
				printf(TEXTE_JAUNEVIF);
			printf(" %s\n", texte_tarif);
			printf(EFFACER_ATTR);
			interprete_etat_vente_place(TabRepresentations[stockage_case].place.etat[place-1],texte_etat);
			printf("Etat de vente :");
			if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				printf(TEXTE_VERTCLAIR);
			else
				printf(TEXTE_JAUNEVIF);
			printf(" %s\n", texte_etat);
			printf(EFFACER_ATTR);
			interprete_categorie_place(TabRepresentations[stockage_case].place.categorie[place-1], TabRepresentations[stockage_case].salle, texte_categorie);
			printf("Catégorie :");
			if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				printf(TEXTE_VERTCLAIR);
			else
				printf(TEXTE_JAUNEVIF);
			printf(" %s\n", texte_categorie);
			printf(EFFACER_ATTR);
			
			printf("Prix :");
			if(TabRepresentations[stockage_case].place.etat[place-1] == 1)
				printf(TEXTE_VERTCLAIR);
			else
				printf(TEXTE_JAUNEVIF);
			printf(" %.2f\n", TabRepresentations[stockage_case].place.prix[place-1]);
			printf(EFFACER_ATTR);
			printf("\n");
		}
		else
		{
			ERREUR("numéro de place inexistant\n");
		}
	}
}

/* ---------------------------------------------------- */
/* ------ AFFICHER TOUTES LES PLACES D'UN CLIENT ------ */
/* ---------------------------------------------------- */
void recherche_places_client()
{
	char nom_recherche[35];
	int espace = 0;
	int i, j;
	char date_affichage[15];
	int trouve = 0;
	int compteur = 0; // pour compter le nombre de places du client

	printf("Nom du client (0 pour annuler) : ");
	scanf(" %[^\n]s", nom_recherche);
	min_vers_maj(nom_recherche);
	for(i = 0; i < NbRepresentations; i++)
	{
		for(j = 0; j < TabRepresentations[i].nb_places; j++)
		{
			if(strcmp(nom_recherche,TabRepresentations[i].place.nom_acheteur[j]) == 0)
			{
				if(espace == 0)
				{
					printf("\n");
					printf(FOND_BLEU);
					printf(" PLACE(S) TROUVÉE(S) POUR : ");
					printf(TEXTE_JAUNEVIF);
					printf("%s ", nom_recherche); 
					printf(EFFACER_ATTR);
					printf("\n\n");
					espace = 1;
				}
				affichage_date(TabRepresentations[i].date,date_affichage);
				compteur++;
				printf(" Place %d (ID place : %d) pour %s le %s (ID représentation : %d)\n", j+1, TabRepresentations[i].place.id_vente[j], TabRepresentations[i].spectacle, date_affichage, TabRepresentations[i].id_representation);
			}
		}
	}
	if(compteur != 0)
	{
		printf(TEXTE_JAUNE);
		printf("\n");
		printf("%d place(s) trouvée(s) au nom de %s\n", compteur, nom_recherche);
		printf("\n");
	}
	else if(atoi(nom_recherche) != 0 || strcmp(nom_recherche,"0") != 0)
	{
		printf(TEXTE_ROUGE);
		printf("%d place(s) trouvée(s) au nom de %s\n", compteur, nom_recherche);
	}
	printf(EFFACER_ATTR);
}

/* ---------------------------------------------------- */
/* ------- APPLIQUER UNE RÉDUCTION DE CARTE CF -------- */
/* ---------------------------------------------------- */
void appliquer_reduction()
{
	int selection_carte; // sélection de la carte CF
	int place = -1; // sélection de la place
	int trouve = -1; // flag
	int max_places_reduc; // nombre max de réductions possible pour la représentation
	int nb_reduc = 0; // réductions tout tarif sauf -28 ans 
	int nb_reduc_jeunesse = 0; // réductions réservées aux places -28 ans
	char type_carte[MAX_TEXTE];
	int i;

	if(id_selection == 0)
	{
		ERREUR("aucune représentation sélectionnée\n");
	}
	else if(NbCartes != 0)
	{
		affichage_cartes();
		printf("ID de la carte CF (0 pour annuler) : ");
		scanf("%d",&selection_carte);
		i = 0;
		while(i < NbCartes && trouve == -1) // Vérification de l'ID de la carte
		{
			if(TabCartes[i].id_carte == selection_carte)
			{
				trouve = i;
			}
			i++;
		}
		if(trouve != -1)
		{
			// --------------- Affichage des informations de la carte CF ---------------
			affichage_plan_salle();
			printf("\n");
			printf(FOND_BLEU);
			printf(" CARTE CF : "); 
			printf(TEXTE_JAUNEVIF);
			printf("%d ", TabCartes[trouve].id_carte);
			printf(EFFACER_ATTR);
			printf("\n\n");
			printf("Propriétaire de la carte :");
			printf(" %s\n",TabCartes[trouve].nom_proprietaire);
			interprete_type_carte(TabCartes[trouve].type, type_carte);
			// --------------- Entrée du nombre de réductions par défaut dont dispose une carte CF, en fonction de son type ---------------
			if(TabCartes[trouve].type == 3) // Carte famille
			{
				nb_reduc = 2;
				nb_reduc_jeunesse = 4;
			}
			else if(TabCartes[trouve].type == 2) // Carte jeune (-28)
			{
				nb_reduc_jeunesse = 2;
			}
			else // Carte adulte
			{
				nb_reduc = 2;
			}
			printf("Carte de type %s détectée\n\n", type_carte);
			printf("Sur quelle(s) place(s) appliquer une réduction ? (0 pour annuler)\n");
			
			// --------------- COMPTAGE DU NOMBRE DE RÉDUCTIONS DISPONIBLES ---------------
			for(i = 0; i < TabRepresentations[stockage_case].nb_places; i++)
			{
				if(TabRepresentations[stockage_case].place.id_carte_reduction[i] == TabCartes[trouve].id_carte) // Si une carte a dans la variable id_carte_reduction le même id que la carte CF...
				{
					printf(TEXTE_JAUNE);
					printf("Réduction obtenue pour la place : %d\n",i+1); // cela signifie qu'une réduction a été obtenue pour cette place via la carte CF,  donc on décrémente le compteur de réductions de la carte
					printf(EFFACER_ATTR);
					if(TabRepresentations[stockage_case].place.tarif_acheteur[i] == 2) // Si la place a un tarif de type jeune, on décrémente le compteur de réduction jeunesse
					{
						nb_reduc_jeunesse-= 1;
					}
					else
					{
						nb_reduc-= 1;
					}
				}
			}
			while(place != 0)
			{
				printf(TEXTE_JAUNE);
				if(TabCartes[trouve].type == 1 || TabCartes[trouve].type == 3 )
				{
					printf("%d réduction(s)", nb_reduc);
				}
				else
				{
					printf("%d réduction(s) jeunesse", nb_reduc_jeunesse);
				}
				if(TabCartes[trouve].type == 3)
				{
					printf(" et %d réduction(s) jeunesse", nb_reduc_jeunesse);
				}
				printf(" disponible(s)\n");
				printf(EFFACER_ATTR);
				printf("Numéro de place (0 pour annuler) : ");
				scanf("%d", &place);

				// --------------- VÉRIFICATION DU NOMBRE DE RÉDUCTIONS TOTAL SUR LA REPRÉSENTATIONS ---------------
				max_places_reduc = TabRepresentations[stockage_case].nb_places*MAX_REDUC_POURCENTAGE;
				if(comptage_reductions_places(id_selection, 0) == max_places_reduc && place != 0)
				{
					ERREUR("Impossible d'ajouter une réduction. Trop de places ont déjà bénéficié d'une réduction sur la représentation (20\% des places)\n");
				}
				else
				{
					if(place != 0)
					{
						if(TabRepresentations[stockage_case].place.etat[place-1] == 1) // Si la place est vendue, alors...
						{
							if(TabRepresentations[stockage_case].place.id_carte_reduction[place-1] == 0) // Si la place n'a pas déjà eu de réduction, alors...
							{
								if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 2) // Si la place est au tarif -28 ans, alors...
								{
									if(nb_reduc_jeunesse > 0) // Si la carte bénéficie de réductions pour -28 ans alors...
									{
										TabRepresentations[stockage_case].place.id_carte_reduction[place-1] = TabCartes[trouve].id_carte; // On entre l'id de la carte dans place.id_carte_reduction
										TabRepresentations[stockage_case].recettes-= TabRepresentations[stockage_case].place.prix[place-1]; // soustraction aux recettes de l'ancien prix de la place
										TabRepresentations[stockage_case].place.prix[place-1] -= TabRepresentations[stockage_case].place.prix[place-1]*0.60; // On modifie le prix d'achat de la place en fonction de la réduction
										TabRepresentations[stockage_case].recettes-= TabRepresentations[stockage_case].place.prix[place-1]; // ajout aux recettes du nouveau prix de la place
										nb_reduc_jeunesse-= 1; // réduction de compteur de réductions jeunesse
										annonce_sauvegarde = 1;
									}
									else
									{
										ERREUR("aucune réduction jeunesse disponible\n");
									}

								}
								else if(TabRepresentations[stockage_case].place.tarif_acheteur[place-1] == 1 && nb_reduc > 0) // Sinon, si la place est au tarif plein et qu'il y a une réduction restante...
								{
									TabRepresentations[stockage_case].place.id_carte_reduction[place-1] = TabCartes[trouve].id_carte; // On entre l'id de la carte dans place.id_carte_reduction
									TabRepresentations[stockage_case].recettes-= TabRepresentations[stockage_case].place.prix[place-1]; // soustraction aux recettes de l'ancien prix de la place
									TabRepresentations[stockage_case].place.prix[place-1] -= TabRepresentations[stockage_case].place.prix[place-1]*0.17; // sinon elle est de 17%
									TabRepresentations[stockage_case].recettes+= TabRepresentations[stockage_case].place.prix[place-1]; // ajout aux recettes du nouveau prix de la place
									nb_reduc-= 1; // réduction du compteur de réductions
									annonce_sauvegarde = 1;
								}
								else
								{
									ERREUR("aucune réduction disponible\n");
								}
							}
							else
							{
								ERREUR("une reduction a déjà été appliquée à cette place\n");
							}
						}
						else if(place > TabRepresentations[stockage_case].nb_places || place < 1)
						{
							ERREUR("numéro de place inexistant\n");
						}
						else
						{
							ERREUR("cette place n'a pas été vendue\n");
						}
					}
				}
			}
		}
		else if(selection_carte != 0)
		{
			ERREUR("ID de carte non assigné\n");
		}
	}
	else
	{
		ERREUR("aucune carte CF enregistrée\n");
	}
}

/* ================================================================================= */
/* ======================== MENU DE GESTION DES CARTES CF ========================== */
/* ================================================================================= */
void menu_cartes()
{
	int choix = -1;
	int reaffichage = 1; // le menu n'apparaît que si la variable reaffichage est sur 1, ce qui ne survient que lorsque le terminal a été nettoyé via NETTOYAGE_ECRAN()
	char date_sortie[MAX_TEXTE];
	affichage_liste = 0;
	id_selection = 0;

	if(NbCartes != 0)
		id_selection = atoi(TabCartes[0].date_expiration);

	while(choix != 0)
	{
		if(reaffichage == 1)
		{
			reaffichage = 0;
			DECORATION();
			TITRE("   GESTION DES CARTES CF                        \n");
			printf("-1- Ajouter une carte CF \n");
			printf("-2- Retirer une carte CF\n");
			if(affichage_liste == 1)
				printf("-3- Cacher la liste des cartes CF          \n");
			else
				printf("-3- Afficher la liste des cartes CF        \n");
			printf("-4- Retirer toutes les cartes CF               \n");
			printf("-5- Consulter les informations d'une carte CF  \n");
			ANNONCE("-6- Sélection de la date d'expiration         \n");
			printf("-7- Rechercher l'ID d'une carte CF			   \n");
			printf("-0- Retour au menu principal                   \n");
			printf(TEXTE_JAUNEVIF);
			printf("-11- Nettoyer l'affichage                      \n");
			printf(EFFACER_ATTR);
			if(id_selection != 0)
			{
				ANNONCE("\n Date d'expiration des cartes CF : ");
				sprintf(date_sortie,"%d",id_selection);
				affichage_date(date_sortie,date_sortie);
				printf("%s\n",date_sortie);
			}
			if(affichage_liste == 1)
				affichage_cartes();
			printf("\n");
		}
		FLECHESAISIE();
		scanf("%d",&choix);
		switch(choix)
		{
			case 1 : reaffichage = vente_carte(reaffichage)				;
					break ;
			case 2 : reaffichage = retrait_carte(reaffichage)			;
					break ;
			case 3 : reaffichage = gestion_affichage_liste(1, NbCartes, "aucune carte CF enregistrée\n", reaffichage);
					break ;
			case 4 : reaffichage = retrait_toutes_cartes(reaffichage)	;
					break ;
			case 5 : consultation_infos_carte()							;
					break ;
			case 6 : reaffichage = selection_date_expiration(reaffichage); 
					break ;
			case 7 : recherche_id_carte()								;
					break ;
			case 11: NETTOYAGE_ECRAN()									;
					 reaffichage = 1									;
					break ;
			case 0 : NETTOYAGE_ECRAN()									;
					break ;
			default: ERREUR("mauvaise entrée\n");
					 break ;
		}
	}
}

/* ---------------------------------------------------- */
/* ----------- AJOUTER LA VENTE D'UNE CARTE ----------- */
/* ---------------------------------------------------- */
int vente_carte(int reaffichage)
{
	int i = NbCartes;
	int choix = -1;
	int doublon;
	int j;

	if(id_selection == 0)
	{
		ERREUR("aucune date d'expiration définie\n");
	}
	else if(NbCartes < MAX_CARTES)
	{
		while(choix != 0 && NbCartes < MAX_CARTES)
		{
			printf("Type [1= Adulte, 2= Jeune, 3= Famille] (0 pour terminer) : ");
			scanf("%d",&choix);
			while(choix != 1 && choix != 2 && choix != 3 && choix != 0)
			{
				ERREUR("Saisissez un choix correct : ");
				scanf("%d",&choix);
			}
			if(choix != 0)
			{
				TabCartes[i].type = choix;
				printf("Prénom et nom de l'acheteur : ");
				scanf(" %[^\n]s",TabCartes[i].nom_proprietaire);
				min_vers_maj(TabCartes[i].nom_proprietaire);
				sprintf(TabCartes[i].date_expiration,"%d",id_selection);

				doublon = 1;
				while(doublon == 1)
				{
					TabCartes[i].id_carte = rand();
					doublon = 0; // on suppose qu'il n'y a pas de doublon
					j = 0;
					while(j < NbCartes && doublon == 0)
					{
						if(TabCartes[i].id_carte == TabCartes[j].id_carte && i != j)
						{
							doublon = 1;
						}
						j++;
					}
				}
				i++;
				NbCartes = i;
				annonce_sauvegarde = 1;
				ANNONCE("Carte ajoutée\n");
				printf("\n");
				if(affichage_liste == 1)
				{
					reaffichage = 1;
				}
			}
		}
	}
	else
	{
		ERREUR("Nombre de cartes CF maximum déjà atteint\n");
	}
	if(reaffichage == 1)
	{
		NETTOYAGE_ECRAN();
	}
	return reaffichage;
}
/* ---------------------------------------------------- */
/* ---------------- RETIRER UNE CARTE ----------------- */
/* ---------------------------------------------------- */
int retrait_carte(int reaffichage)
{
	int a_supprimer; // ID de la carte à supprimer
	int trouve = 0; // flag pour indiquer si la carte existe
	int i, j;

	if(NbCartes == 0)
		ERREUR("aucune carte CF enregistrée\n");
	else
	{
		printf("ID de la carte à supprimer (0 pour annuler) : "); 
		scanf("%d",&a_supprimer);
		if(a_supprimer != 0)
		{
			// --------------- CONTRÔLE DE SAISIE : l'ID entré est-il valide ? ---------------
			i = 0;
			while(i < NbCartes && trouve == 0)
			{
				if(a_supprimer == TabCartes[i].id_carte)
				{
					trouve = 1;
				}
				i++;
			}
			if(trouve == 0)
			{
				ERREUR("ID de carte non assigné\n");
			}
			else
			{
				// --------------- SUPPRESSION DE LA CARTE ---------------
				i = 0;
				trouve = 0;
				while(i < NbCartes && trouve == 0)
				{
					if(a_supprimer == TabCartes[i].id_carte)
					{
						for(j = i ; j < NbCartes-1 ; j++)
						{
							TabCartes[j] = TabCartes[j+1]; // les cartes sont décalées d'une place dans le tableau qui les gère, ce qui efface la carte sélectionnée
						}
						NbCartes--; // on décrémente le nombre des cartes
						trouve = 1;
					}
					i++;
				}
				annonce_sauvegarde = 1;
				ANNONCE("Carte supprimée\n");
				printf("\n");
				if(affichage_liste == 1)
				{
					reaffichage = 1;
				}
				if(NbCartes == 0)
				{
					affichage_liste = 0;
				}
			}
		}
	}
	if(reaffichage == 1)
	{
		NETTOYAGE_ECRAN();
	}
	return reaffichage;
}

/* ----------------------------------------------- */
/* ---- AFFICHAGE DE LA LISTE DES CARTES CF ----- */
/* ----------------------------------------------- */
/*
	 Se référer à la fonction gestion_affichage_liste(int type_liste, int NbObjets, char erreur[], int reaffichage),
	 rangée dans les fonctions communes à plusieurs sous-menus du fichier compil_CF_arriereplan.h
*/

/* ---------------------------------------------------- */
/* ---------- RETRAIT DE TOUTES LES CARTES ------------ */
/* ---------------------------------------------------- */
int retrait_toutes_cartes(int reaffichage)
{
	char validation[10] = "!";

	if(NbCartes == 0)
	{
		ERREUR("aucune carte CF enregistrée\n");
	}
	else
	{
		ATTENTION(" Valider le retrait de toutes les cartes (OUI/NON) : ");
		controle_validation(validation);
		if(strcmp(validation,"OUI") == 0)
		{
			NbCartes = 0;
			id_selection = 0;
			affichage_liste = 0;
			annonce_sauvegarde = 1;
			NETTOYAGE_ECRAN();
			reaffichage = 1;
		}
	}
	return reaffichage;
}

/* ---------------------------------------------------- */
/* ------ AFFICHER LES INFORMATIONS D'UNE CARTE ------- */
/* ---------------------------------------------------- */
void consultation_infos_carte()
{
	int choix;
	char texte_type[15];
	char date_affichage[15];
	int espace = 0; // sert pour ajouter un espace et améliorer la lisibilité de la fonction
	int i, j;
	int trouve = -1;

	if(NbCartes == 0)
	{
		ERREUR("aucune carte CF enregistrée\n");
	}
	else
	{
		printf("ID de la carte à consulter (0 pour annuler) : ");
		scanf("%d",&choix);
		i = 0;
		while(i < NbCartes && trouve == -1)
		{
			if(TabCartes[i].id_carte == choix)
			{
				trouve = i;
			}
			i++;
		}
		if(trouve != -1)
		{
			printf("\n");
			printf(FOND_BLEU);
			printf(" CARTE CF : "); 
			printf(TEXTE_JAUNEVIF);
			printf("%d ", TabCartes[trouve].id_carte);
			printf(EFFACER_ATTR);
			printf("\n\n");
			printf("Type de carte :");
			interprete_type_carte(TabCartes[trouve].type, texte_type);
			printf(" %s\n", texte_type);
			printf("Nom de l'acheteur :");
			printf(" %s\n",TabCartes[trouve].nom_proprietaire);
			affichage_date(TabCartes[trouve].date_expiration,date_affichage);
			printf("Date d'expiration : %s\n", date_affichage);
			printf("\n");
			for(i = 0; i < NbRepresentations; i++)
			{
				for(j = 0; j < TabRepresentations[i].nb_places; j++)
				{
					if(TabRepresentations[i].place.id_carte_reduction[j] == TabCartes[trouve].id_carte)
					{
						if(espace == 0)
						{
							ANNONCE("Carte utilisée pour obtenir une réduction sur : \n");
						}
						affichage_date(TabRepresentations[i].date,date_affichage);
						printf(" Place %d (ID place : %d) pour %s le %s (ID représentation : %d)\n", j+1, TabRepresentations[i].place.id_vente[j], TabRepresentations[i].spectacle, date_affichage, TabRepresentations[i].id_representation);
						espace = 1;
					}
				}
				if(i == NbRepresentations-1 && espace == 1)
				{
					printf("\n");
				}
			}
		}
		else if(trouve == -1 && choix != 0)
		{
			ERREUR("ID de carte non assigné\n");
		}
	}
}

/* ---------------------------------------------------- */
/* ---------- CHOISIR UNE DATE D'EXPIRATION ----------- */
/* ---------------------------------------------------- */
int selection_date_expiration(int reaffichage)
{
	char date_expiration[MAX_TEXTE];
	int annee, mois, jour;

	ANNONCE("Entrée de la date d'expiration :\n");
	printf("Année (0 pour annuler) : ");
	scanf("%d",&annee);
	if(annee != 0)
	{
		// CONTRÔLE DE SAISIE : de l'entrée de l'année
		time_t heure_actuelle; // récupération de la date et l'heure exactes de l'ordinateur
		time(&heure_actuelle);
		struct tm *temps;
		temps = localtime(&heure_actuelle);
		int annee_actuelle = temps -> tm_year + 1900; // variable dans laquelle est placée l'année à laquelle l'ordinateur exécute le programme
		while(annee > 2999 || annee < annee_actuelle) // On ne peut pas ajouter de représentations au-delà de 2999, et en-dessous de l'année actuelle
		{
			ERREUR("Saisissez une année valide : ");
			scanf("%d",&annee);
		}
		printf("Mois de la représentation : ");
		scanf("%d",&mois);

		// CONTRÔLE DE SAISIE : de l'entrée du mois
		while(mois > 12 || mois < 1)
		{
			ERREUR("saisissez un mois valide : ");
			scanf("%d",&mois);
		}
		printf("Jour de la représentation : ");
		scanf("%d",&jour);

		// CONTRÔLE DE SAISIE : de l'entrée du jour
		if(mois == 2)
		{
			while(jour > 28 || jour < 1)
			{
				ERREUR("Saisissez un jour valide : ");
				scanf("%d",&jour);
			}
		}
		else if(mois == 1 || mois == 3 || mois == 5 || mois == 7 || mois == 8 || mois == 10 || mois == 12)
		{
			while(jour > 31 || jour < 1)
			{
				ERREUR("Saisissez un jour valide : ");
				scanf("%d",&jour);
			}
		}
		else
		{
			while(jour > 30 || jour < 1)
			{
				ERREUR("Saisissez un jour valide : ");
				scanf("%d",&jour);
			}
		}
		// Les variables int jour, mois et annee sont passées en string et concaténée via sprintf.
		if(mois < 10 && jour < 10)
		{
			sprintf(date_expiration,"%d0%d0%d",annee,mois,jour);
		}
		else if(mois < 10)
		{
			sprintf(date_expiration,"%d0%d%d",annee,mois,jour);
		}
		else if(jour < 10)
		{
			sprintf(date_expiration,"%d%d0%d",annee,mois,jour);
		}
		else
		{
			sprintf(date_expiration,"%d%d%d",annee,mois,jour);
		}
		// conversion de la date au format string
		id_selection = atoi(date_expiration);
		
		NETTOYAGE_ECRAN();
		reaffichage = 1;
	}
	return reaffichage;
}

/* ---------------------------------------------------- */
/* ----------- RECHERCHER L'ID D'UNE CARTE ------------ */
/* ---------------------------------------------------- */
/*
	L'utilisateur entre le nom complet du propriétaire d'une carte et le programme lui retournera l'ID et le type toutes les cartes lui correspondant.
*/
void recherche_id_carte()
{
	char nom_recherche[35];
	char type_carte[10];
	int espace = 0; // espace pour la lisibilité
	int i, j;

	if(NbCartes == 0)
	{
		ERREUR("aucune carte CF enregistrée\n");
	}
	else
	{
		printf("Nom du propriétaire de la carte (0 pour annuler) : ");
		scanf(" %[^\n]s", nom_recherche);
		min_vers_maj(nom_recherche);
		for (i = 0; i < NbCartes; i++) // Parcours du tableau des cartes dans la boucle
		{
			if(strcmp(TabCartes[i].nom_proprietaire,nom_recherche) == 0) // Si la saisie dans nom_recherche est identique à la valeur de TabCartes[i].nom_proprietaire, alors...
			{
				// --------------- AFFICHAGE DE L'ID ET DU TYPE DES CARTES DONT LE NOM DE L'ACHETEUR CORRESPOND A LA SAISIE
				if(espace == 0)
				{
					printf("\n");
					printf(FOND_BLEU);
					printf(" CARTE(S) TROUVÉE(S) POUR : ");
					printf(TEXTE_JAUNEVIF);
					printf("%s ", TabCartes[i].nom_proprietaire);
					printf(EFFACER_ATTR);
					printf("\n\n");
					espace = 1;
				}
				interprete_type_carte(TabCartes[i].type,type_carte);
				printf(" Carte %d de type %s\n", TabCartes[i].id_carte, type_carte);
			}
		}
		if(espace == 1)
		{
			printf("\n");
		}
		else if(atoi(nom_recherche) != 0 || strcmp(nom_recherche,"0") != 0)
		{
			ERREUR("ID de carte non assigné\n");
		}
	}
}

/* ================================================================================= */
/* =========================== MENU DE GESTION DES DONNÉES ========================= */
/* ================================================================================= */
void menu_donnees()
{
	int choix = -1;
	int reaffichage = 1;
	stockage_case = 0;

	while(choix != 0)
	{
		/*
			Le réaffichage sert dans ce menu à réinitialiser les compteurs des cartes Spectacles, représentations et cartes
		*/
		if(reaffichage == 1)
		{
			reaffichage = 0;
			DECORATION();
			TITRE("   GESTION DES DONNEES                             \n");
			printf(TEXTE_JAUNE);
			printf(" Nombre de spectacles enregistrés : %d            \n", NbSpectacles);
			printf(" Nombre de représentations enregistrées : %d      \n", NbRepresentations);
			printf(" Nombre de cartes CF enregistrées : %d            \n", NbCartes);
			printf(EFFACER_ATTR);
			printf("\n-1- Sauvegarder les données                     \n");
			printf("-2- Effacer les données des fichiers de sauvegarde\n");
			printf("-0- Retour au menu principal                      \n");

			if(stockage_case == 1) // provoque une annonce de lors de la résolution de la fonction sauvegarde()
			{
				ANNONCEBIS(" Les données ont été sauvegardées \n");
				stockage_case = 0;
			}
			else if(stockage_case == 2) // provoque une annonce lors de la résolution de la fonction effacement()
			{
				ANNONCEBIS(" Les données ont été effacées \n");
				stockage_case = 0;
			}
			printf("\n");
		}
		FLECHESAISIE();
		scanf("%d",&choix);

		switch(choix)
		{
			case 1 : reaffichage = sauvegarde(reaffichage)	;
					if(stockage_case == 1) // effacement de l'écran placé ici, pour éviter l'effacement de l'écran lors de la sauvegarde qui survient en quittant le programme ; la fonction de sauvegarde étant appelée dans quitter()
					{
						NETTOYAGE_ECRAN()					;
					}
					break ;
			case 2 : reaffichage = effacement(reaffichage)	;
					break ;
			case 0 : NETTOYAGE_ECRAN()						;
					break ;
			default: ERREUR("mauvaise entrée\n")			;
					break ;
		}
	}
}

/* ---------------------------------------------------- */
/* -------------- SAUVEGARDE DES DONNÉES -------------- */
/* ---------------------------------------------------- */
int sauvegarde(int reaffichage)
{
	// noms des fichiers de sauvegarde
	char fichier_spectacles[30] = "CF_spectacles.csv";
	char fichier_representations[30] = "CF_representations.csv";
	char fichier_places[30] = "CF_places.csv";
	char fichier_cartes[30] = "CF_cartes.csv";
	char validation[10] = "!";
	int i, j;

	if (NbSpectacles == 0 && NbCartes == 0) // L'utilisateur ne peut pas sauvegarder des données vides, parce que cela n'a pas de sens. S'il vient de lancer le programme pour la première fois ou d'effacer les données, il doit entrer des données pour pouvoir sauvegarder
	{
		ERREUR("données vides, il n'y a rien à sauvegarder\n");
	}
	else
	{
		ATTENTION(" Valider la sauvegarde (OUI/NON) : ");
		controle_validation(validation);
		if(strcmp(validation,"OUI") == 0)
		{
			// SAUVEGARDE DES DONNÉES RELATIVES AUX SPECTACLES
			FILE* f1 = fopen(fichier_spectacles,"w"); // ouverture en écriture du fichier des spectacles
			for(i = 0; i < NbSpectacles; i++)
			{
				fprintf(f1,"%d,%s,%d,%d\n", TabSpectacles[i].id_spectacle, TabSpectacles[i].nom, TabSpectacles[i].salle, TabSpectacles[i].nb_representations); // sauvegarde de chaque spectacle
			}
			fclose(f1); // fermeture du fichier des spectacles

			// SAUVEGARDE DES DONNÉES RELATIVES AUX REPRÉSENTATIONS (ET À LEURS PLACES)
			f1 = fopen(fichier_representations,"w"); // ouverture en écriture du fichier des représentations
			FILE* f2 = fopen(fichier_places,"w"); // ouverture en écriture du fichier des places
			for(i = 0; i < NbRepresentations; i++)
			{
				fprintf(f1,"%d,%s,%d,%d,%s,%s,%s,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", TabRepresentations[i].id_representation, TabRepresentations[i].spectacle, TabRepresentations[i].salle, TabRepresentations[i].nb_places, TabRepresentations[i].date, TabRepresentations[i].heure_debut, TabRepresentations[i].heure_fin, TabRepresentations[i].recettes, TabRepresentations[i].id_spectacle, TabRepresentations[i].tarifs_places[0], TabRepresentations[i].tarifs_places[1], TabRepresentations[i].tarifs_places[2], TabRepresentations[i].tarifs_places[3], TabRepresentations[i].tarifs_places[4], TabRepresentations[i].tarifs_places[5], TabRepresentations[i].tarifs_places[6], TabRepresentations[i].tarifs_places[7], TabRepresentations[i].tarifs_places[8], TabRepresentations[i].tarifs_places[9], TabRepresentations[i].tarifs_places[10], TabRepresentations[i].tarifs_places[11]); // sauvegarde de chaque représentation
				for(j = 0; j < TabRepresentations[i].nb_places; j++)
				{
					fprintf(f2,"%d,%s,%d,%d,%d,%f,%d\n", TabRepresentations[i].place.id_vente[j], TabRepresentations[i].place.nom_acheteur[j], TabRepresentations[i].place.tarif_acheteur[j], TabRepresentations[i].place.etat[j], TabRepresentations[i].place.categorie[j], TabRepresentations[i].place.prix[j], TabRepresentations[i].place.id_carte_reduction[j]); // sauvegarde de chaque place
				}
			}
			fclose(f1); // fermeture du fichier des représentations
			fclose(f2); // fermeture du fichier des places

			// SAUVEGARDES DES DONNÉES RELATIVES AUX CARTES CF
			f1 = fopen(fichier_cartes,"w"); // ouverture du fichier des cartes

			for(i = 0; i < NbCartes; i++)
			{
				fprintf(f1,"%d,%s,%s,%d\n", TabCartes[i].id_carte, TabCartes[i].nom_proprietaire, TabCartes[i].date_expiration, TabCartes[i].type); // sauvegarde de chaque carte
			}
			fclose(f1); // fermeture du fichier des cartes
			ANNONCEBIS(" Les données ont été sauvegardées \n");
			stockage_case = 1;
			annonce_sauvegarde = 0;
			reaffichage = 1;
		}
	}
	return reaffichage;
}

/* ---------------------------------------------------- */
/* -------------- EFFACEMENT DES DONNÉES -------------- */
/* ---------------------------------------------------- */
int effacement(int reaffichage)
{
	char validation[10] = "!";

	if (NbSpectacles == 0 && NbCartes == 0)
		ERREUR("données vides, il n'y a rien à effacer\n");
	else
	{
		ATTENTION(" Valider l'effacement des données (OUI/NON) : ");
		controle_validation(validation);
		if(strcmp(validation,"OUI") == 0)
		{
			// --------------- MISE À ZÉRO DES DONNÉES ---------------
			NbSpectacles = 0;
			NbRepresentations = 0;
			NbCartes = 0;
			reaffichage = 1;	
			stockage_case = 2;
			NETTOYAGE_ECRAN();
			// Il n'y a pas d'annonce_sauvegarde = 1 pour éviter que l'utilisateur, sans le vouloir, sauvegarde un fichier vide ---------------
		}
	}
	return reaffichage;
}

/* ================================================================================================================================= */
/* ===================================================== QUITTER LE PROGRAMME ====================================================== */
/* ================================================================================================================================= */
void quitter()
{
	char validation[10] = "!";

	if(annonce_sauvegarde == 1)
	{
		ATTENTION(" DES MODIFICATIONS N'ONT PAS ÉTÉ SAUVEGARDÉES. Sauvegarder avant de quitter (OUI/NON) : ");
		controle_validation(validation);
		if(strcmp(validation,"OUI") == 0)
		{
			sauvegarde(1);
		}
	}
}
