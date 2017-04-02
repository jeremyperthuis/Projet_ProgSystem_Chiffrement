
/* Nombre de fichiers max pouvant etre géré par le programme */
#define MAX_FICHIERS 32


typedef struct info{
	char path[256];
	char decalage[16];
	char sens;
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

/* Crée nb_msg processus */
void creation_processus(int nb_msg);