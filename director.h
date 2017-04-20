
/* Nombre de fichiers max pouvant etre géré par le programme */
#define MAX_FICHIERS 32
/* Nombe de caractere max par fichiers */
#define MAX_CARACTERE 1024


typedef struct info{
	char path[256]; // chemin du fichier
	int decalage;	
	char sens;		// c:chiffrement d:dechiffrement
	char message[MAX_CARACTERE]; // le message
	int position;	// connaitre la position dans le msg 
}INFO;

typedef struct TABinfo{
	INFO Inf[MAX_FICHIERS];
}TABinfo;






/* 	Prend en argument le fichier index et trie les infos de chaque ligne 
	dans la structure INFO. Renvoi un tableau de INFO  
	ainsi que le nombre de message lus avec le pointeur 'nb_msg' */
TABinfo decoupage(char* argv,int* nb_msg);

/* Permet d'afficher le contenu de la structure TABinfo */
void printTABinfo(TABinfo cc, int nb_msg);

/* Permet d'afficher le contenu de la structure INFO */
void printINFO(INFO i);

/* 	Crée nb_msg processus */
void creation_processus(TABinfo* t, int nb_msg);

/*Crée un nouveau fichier et écrit le message codé*/
void nouveau_fichier(INFO i);

/* 	Recupere les messages des fichiers et les copie dans la structure TABinfo*/
TABinfo recupere_message(TABinfo t, int nb_msg);

/* 	Exemple avec une fonction de test*/
void creation_thread(INFO I);

/*	Permet d'encrypter le message */
void *encrypt(void *arg);


/*	Facilite la decalage de lettre dans la table ascii 
	en cas de grand decalage*/
int calculDecalage(int decalage, int position);