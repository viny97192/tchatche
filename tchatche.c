#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tchatche_serveur.h"
#include "conversions.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFF_SIZE	1024

int main(int argc, char *argv[]){
	

	int fd_server, fd_client;
	char *writer = (char *)malloc(BUFF_SIZE*sizeof(*writer)), *pseudo = (char *)malloc(100*sizeof(*pseudo)), *pipe = (char *)malloc(100*sizeof(*pipe));

	if(mkfifo("client",0737) != 0){
		perror("mkfifo ");
		exit(EXIT_FAILURE);
	}

	if((fd_server = open("server", O_WRONLY)) == -1){
		perror("open, ouverture du serveur en écriture chez le client ");
		exit(EXIT_FAILURE);
	}

	if((fd_client = open("client", O_RDONLY)) == -1){
		perror("open, ouverture du client en lecture chez le client");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout,"Entrez votre pseudo : ");
	fgets(pseudo,100,stdin);
	fprintf(stdout,"Entrez le nom de votre tube de communication avec le serveur : ");
	fgets(pipe,100,stdin);
	strtok(pseudo,"\n");
	strtok(pipe,"\n");

	writer = helo_to_protocole(pseudo,pipe);

	write(fd_server,writer,BUFF_SIZE);
	read(fd_client,writer,BUFF_SIZE);
	fprintf(stdout,"%s\n",writer);

	return 0;
}

