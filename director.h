
/* Nombre de fichiers max pouvant etre géré par le programme */
#define MAX_FICHIERS 32
/* Nombe de caractere max par fichiers */
#define MAX_CARACTERE 1024


typedef struct info{
	char path[256];
	int decalage;
	char sens;
	char message[MAX_CARACTERE];
	int position;
}INFO;

typedef struct TABinfo{
	INFO Inf[MAX_FICHIERS];
}TABinfo;

typedef struct mot{
	char w[64];
} mot;





/* 	Prend en argument le fichier index et trie les infos de chaque ligne 
	dans la structure INFO. Renvoi un tableau de INFO  
	ainsi que le nombre de message lus avec le pointeur 'nb_msg' */
TABinfo decoupage(char* argv,int* nb_msg);

/* Permet d'afficher le contenu de la structure TABinfo */
void printTABinfo(TABinfo cc, int nb_msg);

/* Crée nb_msg processus */
void creation_processus(TABinfo* t, int nb_msg);

/* Recupere les messages des fichiers et les copie dans la structure TABinfo*/
TABinfo recupere_message(TABinfo t, int nb_msg);

/* Exemple avec une fonction de test*/
void creation_thread(INFO* I);

void *encrypt(void *arg);