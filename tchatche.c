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

char *id, fifo_client[100], pseudo[100];
int fd_client, connected = 0; 

int main(int argc, char *argv[]){
	
	char *request = (char *)malloc(100*sizeof(*request)), *fifo_client = (char *)malloc(100*sizeof(*fifo_client)), *private_pseudo = (char *)malloc(100*sizeof(*private_pseudo)); 
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


	fprintf(stdout,"enter pseudo : ");
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
		
		else if(!strcmp(request,"private")){
			fprintf(stdout,"enter pseudo : ");
			fgets(private_pseudo,100,stdin);	
			strtok(private_pseudo,"\n");
			fgets(request,100,stdin);
			strtok(request,"\n");
			m = build_message("PRVT",private_pseudo,"",id,request,"");
		}

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
		if(connected == 0){
			id = m -> id;
			connected += 1;
			fprintf(stdout,"connected\n");
		}
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

	else if(!strcmp(m -> type,"PRVT")){
		fprintf(stdout,"[%s --> %s] %s\n", m -> pseudo, pseudo, m -> message);
	}

}

