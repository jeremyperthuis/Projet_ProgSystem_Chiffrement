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
	read(fd1,&buf,MAX_FICHIERS*64);
	

	TABinfo temp;
	
	
	char c;
	// stocke le decalage 
	char decaltemp[16];
	
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
		//conversion des char en int
		temp.Inf[nb].decalage=atoi(decaltemp);
		j=0;
		i++;
		c=buf[i];
		temp.Inf[nb].sens=c;
		i++;
		c=buf[i];
		if(c==10)
		{	
			nb++;
			printf("chariot\n");
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
	printf("******************************\n");
}

void printINFO(INFO i)
{
	printf("\n*************INFO*****************\n\n");
	
	{
		printf("path: %s\n",i.path);
		printf("decalage: %d\n",i.decalage);
		printf("sens: %c\n",i.sens);
		printf("message: %s\n\n",i.message);
	}
	printf("******************************\n");
}


void creation_processus(TABinfo* t, int nb_msg)
{
	pid_t pid, status;
	int i,j;
	TABinfo tmp = *t;

	char messageRecu[MAX_CARACTERE]; 
	int descripteurTube[2]; // parametre du pipe
	pipe(descripteurTube);
	

	for(i =0;i<nb_msg;i++)
	{

		pid=fork();
		
		if(pid==-1) exit(0);
		
		if(pid==0) // on est dans les fils
		{
			printf("on est dans le fils n°%d , message = %s\n",i,t->Inf[i].message);
			
			creation_thread(tmp.Inf[i]);
			
			
			write(descripteurTube[1],tmp.Inf[nb_msg].message,MAX_CARACTERE);
			exit(getpid());
		}

		if(pid>0)
		{
			read(descripteurTube[0],messageRecu,MAX_CARACTERE);
			//printf("message_recu:%s\n",messageRecu);

			for(j=0;j<MAX_CARACTERE;j++)
			{
				tmp.Inf[nb_msg].message[j]=messageRecu[j];
			}
			//printf("tmp : %s\n",tmp.Inf[nb_msg].message);
		}

	}

	for(i=0;i<nb_msg;i++)
	{
		wait(&status);
	}
	
}


TABinfo recupere_message(TABinfo t, int nb_msg)
{
	int i;
	int fd[nb_msg];	
	for(i=0;i<nb_msg;i++)
	{
		char buf[MAX_CARACTERE]="";
		fd[i]=open(t.Inf[i].path,O_RDONLY);
		read(fd[i],&buf,MAX_CARACTERE);
		int j=0;
		while(buf[j]!='\0')
		{
			t.Inf[i].message[j]=buf[j];
			j++;
		}
		close(fd[i]);
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

void creation_thread(INFO I)
{	
	INFO tmp = I;
	tmp.position=0;
	//printf("position:%d\n",tmp.position);

	int i=0;
	int nb_thread=tmp.decalage;


	while(tmp.message[i]!='\0')
	{
		int sym = tmp.message[i];

		// si le caractere est une lettre
		if((sym>=65 && sym<=90)||(sym>=97 && sym <= 122))
		{
			pthread_t mythread;
			
			pthread_create(&mythread, NULL,encrypt,&tmp);
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

	printf("	message codé : %s\n",tmp.message);
	nouveau_fichier(tmp);
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