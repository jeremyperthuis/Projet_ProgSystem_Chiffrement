#include "director.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>


TABinfo decoupage(char* argv, int* nb_msg)
{
	// Ce buffer stocke tout le contenu de index.txt
	char buf[MAX_FICHIERS*64];
	int fd1=open(argv,O_RDONLY);
	if (fd1==-1) printf("erreur open index.txt\n");
	read(fd1,&buf,MAX_FICHIERS*64);
	close(fd1);

	TABinfo temp;
	char c;
	// on stocke le decalage dans un tableau de char avant de le convertir 
	char decaltemp[16]="";
	int i=0, j=0, nb=0;
	
	// Tant qu'on arrive pas a la fin de index.txt
	c=buf[i];
	while((c != '\0'))
	{	
		// Tant qu'on atteint pas la fin du path
		while (c!=';')
		{
			temp.Inf[nb].path[j]=c;
			i++;
			j++;
			c=buf[i];
		}
		temp.Inf[nb].path[j+1]='\0';
		
		i++;
		j=0;
		c=buf[i];

		// Tant qu'on atteint pas la fin du decalage
		while(c!=';')	
		{
			decaltemp[j]=c;
			i++;
			j++;
			c=buf[i];
		}
		decaltemp[j+1]='\0';
		//conversion du decalage en int
		temp.Inf[nb].decalage=atoi(decaltemp);
		if(temp.Inf[nb].decalage <= 0) printf("erreur decalage incorect\n");

		j=0;
		i++;
		c=buf[i];
		// si le sens n'est pas reconnu
		if(c!='c' && c!='d') printf("Erreur sens incorect !\n");

		temp.Inf[nb].sens=c;
		i++;
		c=buf[i];

		// si on rencontre le symbole de retour a la ligne
		if(c==10)
		{	
			nb++;
			i++;
			c=buf[i];
		}
	}

	// on renvoi le nombre de lignes lues par pointeur
	*nb_msg=nb;

	return temp;
}


void printTABinfo(TABinfo cc, int nb_msg)
{
	printf("\n*************TABINFO*****************\n\n");
	for(int i=0;i<nb_msg;i++)
	{
		printf("TABinfo[%d].path: %s\n",i,cc.Inf[i].path);
		printf("TABinfo[%d].decalage: %d\n",i,cc.Inf[i].decalage);
		printf("TABinfo[%d].sens: %c\n",i,cc.Inf[i].sens);
		printf("TABinfo[%d].message: %s\n\n",i,cc.Inf[i].message);
		
	}
	printf("****************************************************\n");
}



void creation_processus(TABinfo* t, int nb_msg)
{
	if(nb_msg==0) printf("aucun message à traiter\n");
	pid_t pid, status;
	int i,j;
	TABinfo tmp = *t;


	char messageRecu[MAX_CARACTERE]; 
	int descripteurTube[2]; // parametre du pipe
	if(pipe(descripteurTube)==-1) printf("erreur pipe\n");
	

	for(i =0;i<nb_msg;i++)
	{

		pid=fork();
		
		if(pid==-1) exit(0);
		
		if(pid==0) // on est dans les fils
		{
			printf("****************************************************\n");
			printf("MESSAGE N°%d\n\ndecalage : %d\nmessage à traiter : \n\n%s\n",
					i+1,t->Inf[i].decalage,t->Inf[i].message);

			// Si le mot est vide
			if(strlen(t->Inf[i].message)==0)
			{
				printf("ERREUR : PAS DE MESSAGE A TRAITER !\n");
			}
			
			
			
			creation_thread(tmp.Inf[i],i);

			write(descripteurTube[1],tmp.Inf[nb_msg].message,MAX_CARACTERE);

			
			exit(getpid());
		}

		if(pid>0)
		{
			read(descripteurTube[0],messageRecu,MAX_CARACTERE);

			for(j=0;j<MAX_CARACTERE;j++)
			{
				tmp.Inf[nb_msg].message[j]=messageRecu[j];
			}
		}

	}
	printf("****************************************************\n");
	// on attent la fin des processus fils
	for(i=0;i<nb_msg;i++)
	{
		wait(&status);
	}
	
}

void creation_thread(INFO I, int nf)
{	
	INFO tmp = I;
	tmp.position=0;
	

	int i=0;
	//compte le nombre de thread invoqué
	int nb_thread=0;

	// Tant que l'on atteint pas la fin du message
	while(tmp.message[i]!='\0')
	{
		int sym = tmp.message[i];

		// si le caractere est une lettre
		if((sym>=65 && sym<=90)||(sym>=97 && sym <= 122))
		{
			pthread_t mythread;
			
			pthread_create(&mythread, NULL,encrypt,&tmp);
			nb_thread++;
			i++;	
			pthread_join(mythread,NULL);
			
			// cette boucle permet d'avancer juste après la fin du mot
			while((sym>=65 && sym<=90)||(sym>=97 && sym <= 122))
			{
				i++;
				tmp.position++;
				sym = tmp.message[i];
			}
		}

		// si le caractere n'est pas une lettre
		else
			i++;
			tmp.position++;
	}

	if(tmp.sens=='c')
	{
		// si le message est non vide
		if(strlen(tmp.message))
		{
			printf(" -> Le message chiffré a été écrit dans le fichier msg%d_cypher.txt\n\n",nf+1);
			nouveau_fichier(tmp);
		}
	}

	else if(tmp.sens=='d')
	{
		// si le message est non vide
		if(strlen(tmp.message))
		{
			printf(" -> message dechiffré : \n\n%s\n",tmp.message);
		}
	}
	printf("[Processus n°%d : %d thread(s) crée(s)]\n",nf,nb_thread);
}


TABinfo recupere_message(TABinfo t, int nb_msg)
{
	printf("\nNombre de message à traiter :%d\n\n",nb_msg);
	int i;
	int fd[nb_msg];	
	for(i=0;i<nb_msg;i++)
	{
		char buf[MAX_CARACTERE]="";
		
		fd[i]=open(t.Inf[i].path,O_RDONLY);
		if(fd[i]==-1) printf("erreur ouverture fichier ! %s\n",t.Inf[i].path);

		if(read(fd[i],&buf,MAX_CARACTERE)==0) printf("Erreur message n°%d vide !\n",i+1);
		close(fd[i]);

		int j=0;
		while(buf[j]!='\0')
		{
			t.Inf[i].message[j]=buf[j];
			j++;
		}
	}
	return t;
}

void *encrypt(void *arg)
{
	// On copie le contenu pour faciliter la manipulation des données
	INFO tmp= *(INFO*)arg;
	int pos=tmp.position;
	char sens=tmp.sens;

	// Tant que le mot contient que des lettres
	while((tmp.message[pos]>=65 && tmp.message[pos]<=90) || 
		(tmp.message[pos]>=97 && tmp.message[pos]<=122))
	{
		// On creer un pointeur qui pointe sur l'argument de la fonction pour faciliter les manipulations
		INFO* test= (INFO*)arg;
		test->message[pos]=calculDecalage(tmp.decalage,test->message[pos],sens);
		tmp.message[pos++];
	}

	pthread_exit(NULL);
}



int calculDecalage(int decalage, int position, char sens)
{
	                
	// si le decalage est grand
	if(decalage >= 26)
	{
		decalage = decalage%26;
	}

	// si on veut crypter
	if(sens=='c')
	{

		// Si c'est une majuscule
		if(position>=65 && position<=90)
		{
			// si le decalage depase de la liste des majuscules
			if((decalage+position)>90)
			{
				position -=(26-decalage);
			}

			// si le decalage ne depasse pas de la liste des majuscules
			else if((decalage+position)<=90)
			{
				position+=decalage;
			}
		}
		//si c'est une minuscule
		else if(position<=122 && position>=97)
		{
			// si le decalage depase de la liste des minuscules
			if((decalage+position)>122)
			{
				position -=(26-decalage);
			}

			// si le decalage ne depasse pas de la liste des minuscules
			else if((decalage+position)<=122)
			{
				position+=decalage;
			}
		}
	}

	// Si on veut decrypter
	else if (sens=='d')
	{

		// Si c'est une majuscule
		if(position>=65 && position<=90)
		{
			// si le decalage depase de la liste des majuscules
			if((position-decalage)<65)
			{
				position +=(26-decalage);
			}

			// si le decalage ne depasse pas de la liste des majuscules
			else if((position-decalage)>=65)
			{
				position-=decalage;
			}
		}
		//si c'est une minuscule
		else if(position<=122 && position>=97)
		{
			// si le decalage depase de la liste des minuscules
			if((position-decalage)<97)
			{
				position +=(26-decalage);
			}

			// si le decalage ne depasse pas de la liste des minuscules
			else if((position-decalage)>=97)
			{
				position-=decalage;
			}
		}
	}
		
	return position;
}



void nouveau_fichier(INFO i)
{
	int cpt=0;
	char tmp[256];

	// permet de trouver le "." dans le path original
	while(i.path[cpt]!='.')
	{
		tmp[cpt]=i.path[cpt];
		cpt++;
	}

	// on remplace le "." par le suffixe "_cypher.txt" 
	tmp[cpt]='\0';
	strcat(tmp,"_cypher.txt");

	// permet de compter le nombre de caractere a ecrire dans le msg
	cpt=0;
	
	while(i.message[cpt]!='\0')
	{
		cpt++;
	}

	// creation et ecriture du fichier
	int fd=open(tmp,O_CREAT | O_WRONLY,S_IRWXU);
	write(fd,&i.message,cpt);
	close(fd);
}