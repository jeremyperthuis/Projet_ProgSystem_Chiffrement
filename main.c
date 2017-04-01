#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "director.h"

#define NB_MAX_FICHIERS 32



int main(int argc, char* argv[])
{
	TABinfo cc= decoupage(argv[1]);
	printf("bonjour\n");

	printf("cc[0].path: %s\n",cc.Inf[0].path);

}