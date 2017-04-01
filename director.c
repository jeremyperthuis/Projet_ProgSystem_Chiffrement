#include "director.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>






TABinfo decoupage(char* argv)
{
	/*On recupere l'index dans un buffer*/
	char buf[NB_MAX_FICHIERS*64];
	int fd1=open(argv,O_RDONLY);
	read(fd1,&buf,NB_MAX_FICHIERS*64);
	
	TABinfo tab;
	
	char c;
	
	int i=0, j=0, nb=0;
	
	while((c=buf[i])!='\0')
	{		
		if(c==';')	// on atteint le premier ';'
		{	
			i++;
			
			tab.Inf[nb].path[j]='\0';
			j=0;

 			while((c=buf[i])!=';') 
 			{
 				tab.Inf[nb].decalage[j]=c;
 				j++;
 				i++;
 			}
 			tab.Inf[nb].decalage[j]='\0';
 			i++;
 			c=buf[i];
 			tab.Inf[nb].sens=c;
 			i+=2;
 			nb++;
 			j=0;
 			c=buf[i];
		}

		
		tab.Inf[nb].path[j]=c;
		j++;
		i++;
	}
	printf("tab[0].path: %s\n",tab.Inf[0].path);
	printf("tab[0].decalage: %s\n",tab.Inf[0].decalage);
	printf("tab[0].sens: %c\n",tab.Inf[0].sens);
	
	printf("tab[1].path: %s\n",tab.Inf[1].path);
	printf("tab[1].decalage: %s\n",tab.Inf[1].decalage);
	printf("tab[1].sens: %c\n",tab.Inf[1].sens);

	printf("tab[2].path: %s\n",tab.Inf[2].path);
	printf("tab[2].decalage: %s\n",tab.Inf[2].decalage);
	printf("tab[2].sens: %c\n",tab.Inf[2].sens);
	
	return tab;
}