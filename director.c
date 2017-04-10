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
	/*On recupere l'index dans un buffer*/
	char buf[MAX_FICHIERS*64];
	int fd1=open(argv,O_RDONLY);
	read(fd1,&buf,MAX_FICHIERS*64);
	
	TABinfo temp;
	
	char c;
	char decaltemp[16];
	
	int i=0, j=0, nb=0;
	
	while((c=buf[i])!='\0')
	{		
		if(c==';')	// on atteint le premier ';'
		{	
			i++;
			
			temp.Inf[nb].path[j]='\0';
			j=0;

 			while((c=buf[i])!=';') 
 			{
 				decaltemp[j]=c;
 				j++;
 				i++;
 			}

 			decaltemp[j]='\0';
 			temp.Inf[nb].decalage=atoi(decaltemp);
 			i++;
 			c=buf[i];
 			temp.Inf[nb].sens=c;
 			i+=2;
 			nb++;
 			j=0;
 			c=buf[i];
		}

		temp.Inf[nb].path[j]=c;
		j++;
		i++;
	}
	*nb_msg=nb;
	return temp;
}


void printTABinfo(TABinfo cc, int nb_msg)
{
	for(int i=0;i<nb_msg;i++)
	{
		printf("TABinfo[%d].path: %s\n",i,cc.Inf[0].path);
		printf("TABinfo[%d].decalage: %d\n",i,cc.Inf[i].decalage);
		printf("TABinfo[%d].sens: %c\n",i,cc.Inf[i].sens);
		printf("TABinfo[%d].message: %s\n\n",i,cc.Inf[i].message);
		
	}
}

void creation_processus(TABinfo t, int nb_msg)
{
	pid_t pid, status;
	int i;
	for(i =0;i<nb_msg;i++)
	{

		pid=fork();
		if(pid==-1) exit(0);
		if(pid==0) 
		{
			printf("on est dans le fils\n");
			printf("processus message:%s\n", t.Inf[i].message);
			exit(getpid());
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
	for(i=0;i<nb_msg;i++)
	{
		char buf[MAX_CARACTERE];
		int fd1=open(t.Inf[i].path,O_RDONLY);
		read(fd1,&buf,MAX_CARACTERE);
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
	printf("On est dans encrypt\n");
	while((*(char*)arg>=65 && *(char*)arg<=90) || (*(char*)arg>=97 && *(char*)arg<=122))
	{
		*(char*) arg+=3;
		arg++;
	}

	pthread_exit(NULL);
}


void creation_thread(TABinfo* t)
{
	int i=0;
	int nb_thread=t->Inf[0].decalage;
	while(t->Inf[0].message[i]!='\0')
	{
		printf("i=%d\n",i);
		int sym = t->Inf[0].message[i];

		if((sym>=65 && sym<=90)||(sym>=97 && sym <= 122)) // si c'est une lettre
		{
			pthread_t mythread;
			pthread_create(&mythread, NULL,encrypt,&t->Inf[0].message[i]);
			pthread_join(mythread,NULL);

			while((sym>=65 && sym<=90)||(sym>=97 && sym <= 122))
			{

				i++;
				sym = t->Inf[0].message[i];
			}
			printf("fin while\n");
		}

		else
			i++;
	
	}
}