#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tchatche_serveur.h"
#include "conversions.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

void *server_response(void *args);
void print_response(message *m);
void send_to_server(message *m);

char *id, fifo_client[100];
int fd_client; 

int main(int argc, char *argv[]){
	
	char *request = (char *)malloc(100*sizeof(*request)), *fifo_client = (char *)malloc(100*sizeof(*fifo_client)), *pseudo = (char *)malloc(100*sizeof(*pseudo)); 
	int fd_server;
	pthread_t thr;
	message *m = init_message();

	if(pthread_create(&thr,NULL,server_response,NULL) != 0){
		fprintf(stderr,"Erreur pthread_create.\n");
		exit(EXIT_FAILURE);
	}

	if((fd_server = open("fifo_server", O_WRONLY)) == -1){
		fprintf(stderr,"open fifo_server\n");
		exit(EXIT_FAILURE);
	}


	fprintf(stdout,"entrez votre pseudo : ");
	fgets(pseudo,100,stdin);
	strtok(pseudo,"\n");

	sprintf(fifo_client,"fifo_client.%d", (int)getpid());
	m = build_message("HELO",pseudo,fifo_client,"","","");
	request = format_protocole_client(m);

	if(write(fd_server,request,strlen(request)) == -1){
		fprintf(stderr,"Impossible d'écrire au serveur.\n");
		exit(EXIT_FAILURE);
	}

	while(1){

		fgets(request,100,stdin);
		strtok(request,"\n");

		if(!strcmp(request,"quit"))
			m = build_message("BYEE","","",id,"","");	
		
		else
			m = build_message("BCST","","",id,request,"");

		request = format_protocole_client(m);

		if(write(fd_server,request,strlen(request)) == -1){
			fprintf(stderr,"Impossible d'écrire au serveur.\n");
			continue;
		}
	}

	return 0;
}

void *server_response(void *args){
	
	char *response = (char *)malloc(100*sizeof(*response)); 
	/*
	*fifo_client = (char *)malloc(100*sizeof(*fifo_client)), 
	*/
	message *m;

	strcpy(fifo_client,"fifo_client.");
	sprintf(fifo_client,"%s%d", fifo_client, (int)getpid());

	if(mkfifo(fifo_client, 0666) == -1){
		fprintf(stderr,"mkfifo fifo_client.%d\n", (int)getpid());
		exit(EXIT_FAILURE);
	}

	if((fd_client = open(fifo_client, O_RDONLY)) == -1){
		fprintf(stderr,"open fifo_client.%d\n", (int)getpid());
		exit(EXIT_FAILURE);
	}

	while(1){
		if(read(fd_client,response,100) == -1){
			fprintf(stderr,"Impossible de lire le message du serveur.\n");
			continue;
		}	

		m = parse_client(response);
		print_response(m);
	}
}

void print_response(message *m){

	if(!strcmp(m -> type,"OKOK")){
		id = m -> id;
		fprintf(stdout,"connected\n");
	}

	/*
	else if(!strcmp(m -> type,"BADD")){
		fprintf(stdout,"Ce pseudo est déjà utilisé.\n");
		badd = 1;	
	}
	*/
		
	else if(!strcmp(m -> type,"BCST")){
		fprintf(stdout,"[%s] %s\n", m -> pseudo, m -> message);
	}

	else if(!strcmp(m -> type,"BYEE")){
		fprintf(stdout,"You've been deconected\n");
		unlink(fifo_client);
		sleep(1);
		exit(EXIT_SUCCESS);
	}
}

/*
void *server_response(void *arg);

int main(int argc, char *argv[]){
	
	char *request = (char *)malloc(100*sizeof(*request)), *fifo_client = (char *)malloc(100*sizeof(*fifo_client));
	int fd_server;
	pthread_t server_listening;
	message *m = init_message();

	sprintf(fifo_client,"fifo_client.%d", (int)getpid());
	m -> type = "HELO";
	m -> tube = fifo_client;
	m -> length = "0039";

	if(pthread_create(&server_listening,NULL,server_response,NULL) != 0){
		fprintf(stderr,"Erreur pthread_create\n");
		exit(EXIT_FAILURE);
	}

	if((fd_server = open("fifo_server", O_WRONLY)) == -1){
		fprintf(stderr,"open fifo_serveur\n");
		exit(EXIT_FAILURE);
	}

	while(1){
	fgets(request,100,stdin);
	strtok(request,"\n");
	m -> pseudo = request;
	request = format_protocole_client(m);
	fprintf(stdout,"request = %s\n", request);

	if(write(fd_server,request,strlen(request)) == -1){
		fprintf(stderr,"writer fifo_serveur\n");
		exit(EXIT_FAILURE);
	}
	}

	return 0;
}

void *server_response(void *arg){
	
	char *fifo_client = (char *)malloc(100*sizeof(*fifo_client)), *response = (char *)malloc(100*sizeof(*response));
	int fd_client;

	strcpy(fifo_client,"fifo_client.");
	sprintf(fifo_client,"%s%d", fifo_client, (int)getpid());

	if(mkfifo(fifo_client, 0666) == -1){
		fprintf(stderr,"mkfifo %s\n", fifo_client);
		exit(EXIT_FAILURE);
	}

	if((fd_client = open(fifo_client, O_RDONLY)) == -1){
		fprintf(stderr,"open %s\n", fifo_client);
		exit(EXIT_FAILURE);
	}
	
	while(1){
		if(read(fd_client,response,100) == -1){
			fprintf(stderr,"read %s\n", fifo_client);
			exit(EXIT_FAILURE);
		}
		
		fprintf(stdout,"%s\n", response);
	}
}

*/
