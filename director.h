
/* Nombre de fichiers max pouvant etre géré par le programme */
#define NB_MAX_FICHIERS 32


typedef struct info{
	char path[256];
	char decalage[16];
	char sens;
}INFO;

typedef struct TABinfo{
	INFO Inf[NB_MAX_FICHIERS];
}TABinfo;

/* 	Prend en argument le fichier index et trie les infos de chaque ligne 
	dans la structure INFO. Renvoi un tableau de INFO */
TABinfo decoupage(char* argv);