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
 				temp.Inf[nb].decalage[j]=c;
 				j++;
 				i++;
 			}

 			temp.Inf[nb].decalage[j]='\0';
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
		printf("TABinfo[%d].decalage: %s\n",i,cc.Inf[i].decalage);
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

void *fonction1(void *arg)
{
	printf("On est dans thread1\n");
	printf("%d\n",*(int*) arg);  
	pthread_exit(NULL);
}


void creation_thread()
{
	int* a = malloc(sizeof(int));
        *a = 34;
	pthread_t mythread;
	pthread_create(&mythread, NULL,fonction1,a);
	pthread_join(mythread,NULL);
}