/* ---------------------------------------------------- */
/* --------------- DÉCORATION D'EN-TÊTE --------------- */
/* ---------------------------------------------------- */
void DECORATION()
{
	//printf(FOND_BLANC); // Si on préfère que l'entête décorative ait un fond blanc
	printf("                                                                                         \n");
	printf(TEXTE_ROUGE);
	printf(" █   ▄▀█   █▀▀ █▀█ █▀▄▀█ █▀▀ █▀▄ █ █▀▀   █▀▀ █▀█ ▄▀█ █▄ █ █▀▀ ▄▀█ █ █▀ █▀▀   ██████████  \n");
	printf(" █▄▄ █▀█   █▄▄ █▄█ █ ▀ █ ██▄ █▄▀ █ ██▄   █▀  █▀▄ █▀█ █ ▀█ █▄▄ █▀█ █ ▄█ ██▄   ██      ██  \n");
	printf("                                                                             ██  ██  ██  \n");
	printf("           █▀ █ █▀▄▀█ █ █ █     █▀▀ ▀█▀   █▀ █ █▄ █ █▀▀ █ █ █   █ █▀         ██      ██  \n");
	printf("           ▄█ █ █ ▀ █ █▄█ █▄▄   ██▄  █    ▄█ █ █ ▀█ █▄█ █▄█ █▄▄ █ ▄█         ██████████  \n");
	printf("                                                                                         \n");
	printf(EFFACER_ATTR);
	printf("\n");
}

/* ---------------------------------------------------- */
/* ------------------ TITRE DE MENU ------------------- */
/* ---------------------------------------------------- */
void TITRE(char nommenu[])
{
	printf(TEXTE_NOIR);
	printf(FOND_BLANC);
	printf("%s",nommenu);
	printf(EFFACER_ATTR);
	printf("\n");
}

/* ---------------------------------------------------- */
/* ----------- AFFICHER UN MESSAGE D'ERREUR ----------- */
/* ---------------------------------------------------- */
void ERREUR(char erreur[])
{
	printf(TEXTE_ROUGE);
	printf(" ERREUR : %s",erreur);
	printf(EFFACER_ATTR);
}

/* ---------------------------------------------------- */
/* --------- AFFICHER UN MESSAGE D'ERREUR BIS --------- */
/* ---------------------------------------------------- */
/*
	Alors que la fonction ERREUR() affiche le texte de l'erreur en rouge, celle-ci affiche l'erreur sur un fond rouge
	Les erreurs de la fonction ERREURBIS() sont des erreurs plus importantes
*/
void ERREURBIS(char erreur[])
{
	printf("\n");
	printf(FOND_ROUGE);
	printf(" ERREUR : %s",erreur);
	printf(EFFACER_ATTR);
}

/* ---------------------------------------------------- */
/* ---------- AFFICHER UN MESSAGE D'ANNONCE ----------- */
/* ---------------------------------------------------- */
void ANNONCE(char annonce[])
{
	printf(TEXTE_JAUNE);
	printf("%s",annonce);
	printf(EFFACER_ATTR);
}

/* ---------------------------------------------------- */
/* -------- AFFICHER UN MESSAGE D'ANNONCE BIS --------- */
/* ---------------------------------------------------- */
/*
	Alors que la fonction ERREUR() affiche le texte de l'annonce en jaune, celle-ci affiche l'annonce sur un fond bleu
	Les annonces de la fonction ANNONCEBIS() sont des annonces plus importantes
*/
void ANNONCEBIS(char annonce[])
{
	printf("\n");
	printf(FOND_BLEU);
	printf("%s",annonce);
	printf(EFFACER_ATTR);
}

/* ---------------------------------------------------- */
/* ------ AFFICHER UN MESSAGE *!* ATTENTION *!* ------- */
/* ---------------------------------------------------- */
void ATTENTION(char annonce[])
{
	printf(FOND_BLANC);
	printf(TEXTE_ROUGE);
	printf(CLIGNOTEMENT);
	printf(" *!* ATTENTION *!* ");
	printf(EFFACER_ATTR);
	printf("%s",annonce);
}

/* ---------------------------------------------------- */
/* ------------ AFFICHAGE FLECHE DE SAISIE ------------ */
/* ---------------------------------------------------- */
void FLECHESAISIE()
{
	printf(">>> "); // indique à l'utilisateur qu'il doit entrer une saisie
}

/* ---------------------------------------------------- */
/* --------- NETTOYER L'AFFICHAGE DU TERMINAL --------- */
/* ---------------------------------------------------- */
/* 
	Ce programme fait usage de la séquence d'échappement ANSI \033c pour nettoyer la fenêtre du terminal dans son intégralité.
	A chaque fois que le programme rencontre cette séquence d'échappement, l'affichage du terminal est réinitialisé
*/
void NETTOYAGE_ECRAN()
{
	printf("\033c" ); // séquence d'échappement qui nettoie la totalité de ce qui a été affiché dans le terminal
}

/* ---------------------------------------------------- */
/* -------------- CONVERSION EN MAJUSCULE ------------- */
/* ---------------------------------------------------- */
/*
	Cette fonction convertit l'intégralité de la chaîne de caractères entrée en argument en caractères majuscules
*/
void min_vers_maj(char string[])
{
	int i;
	for(i = 0; i <  strlen(string) ; i++)
	{
  		string[i] = toupper(string[i]); // chaque caractère de la chaîne est converti en majuscule
	}
}
