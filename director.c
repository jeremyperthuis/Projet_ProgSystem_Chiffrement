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
	printf("\n*************TABINFO*****************\n\n");
	for(int i=0;i<nb_msg;i++)
	{
		printf("TABinfo[%d].path: %s\n",i,cc.Inf[0].path);
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
	TABinfo tmp2= tmp;

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
			
			printf("messagecod: %s\n",tmp.Inf[nb_msg].message);
			write(descripteurTube[1],tmp.Inf[nb_msg].message,MAX_CARACTERE);
			exit(getpid());
		}

		if(pid>0)
		{
			read(descripteurTube[0],messageRecu,MAX_CARACTERE);
			printf("message_recu:%s\n",messageRecu);

			for(j=0;j<MAX_CARACTERE;j++)
			{
				tmp.Inf[nb_msg].message[j]=messageRecu[j];
			}
			printf("tmp : %s\n",tmp.Inf[nb_msg].message);
		}
	}

	for(i=0;i<nb_msg;i++)
	{
		wait(&status);
	}
	//printTABinfo(t,nb_msg);
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
	INFO tmp= *(INFO*)arg;
	int pos=tmp.position;
	while((tmp.message[pos]>=65 && tmp.message[pos]<=90) || (tmp.message[pos]>=97 && tmp.message[pos]<=122))
	{
		INFO* test= (INFO*)arg;
		test->message[pos]+=tmp.decalage;
		tmp.message[pos++];

	}

	pthread_exit(NULL);
}


INFO creation_thread(INFO I)
{	
	INFO tmp = I;
	tmp.position=0;
	printf("position:%d\n",tmp.position);

	int i=0;
	int nb_thread=tmp.decalage;

	while(tmp.message[i]!='\0')
	{
		printf("i=%d\n",i);
		int sym = tmp.message[i];

		if((sym>=65 && sym<=90)||(sym>=97 && sym <= 122)) // si c'est une lettre
		{
			pthread_t mythread;
			
			// si on doit encrypter
			if(tmp.sens=='c')
			{
				pthread_create(&mythread, NULL,encrypt,&tmp);
			}

			// si on doit decrypter
			else if(tmp.sens=='d')
			{

			}
				
			pthread_join(mythread,NULL);
			
			// cette boucle permet d'avancer jusqu'au prochain mot
			while((sym>=65 && sym<=90)||(sym>=97 && sym <= 122))
			{

				i++;
				tmp.position++;
				sym = tmp.message[i];
			}
		}

		else
			i++;
			tmp.position++;
	
	}
	printINFO(tmp);
}