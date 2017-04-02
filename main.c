#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "director.h"





int main(int argc, char* argv[])
{
	int nombre_msg;
	TABinfo t = decoupage(argv[1],&nombre_msg);
	printf("nombre message :%d\n",nombre_msg);
	printTABinfo(t,nombre_msg);
	creation_processus(nombre_msg);

}