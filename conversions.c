#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conversions.h"

int type_message(char *message){
	
	char *s = type(message);
	int t;

	if(!strcmp(s,"HELO"))
		t = 1;
	if(!strcmp(s,"OKOK"))
		t = 2;
	if(!strcmp(s,"BADD"))
		t = 3;
	if(!strcmp(s,"BYEE"))
		t = 4;
	if(!strcmp(s,"BCST"))
		t = 5;
	if(!strcmp(s,"PRVT"))
		t = 6;
	if(!strcmp(s,"LIST"))
		t = 7;
	if(!strcmp(s,"CHAT"))
		t = 8;
	if(!strcmp(s,"DEBG"))
		t = 9;
	if(!strcmp(s,"FILE"))
		t = 10;
	if(!strcmp(s,"SHUT"))
		t = 11;

	return t;
}

char *type(char *message){
	
	char *s = (char *)malloc(4*sizeof(*s));
	int i;

	if(strlen(message) < 8){
		fprintf(stderr,"taille du message non valide.\n");
		return NULL;
	}
	
	for(i=4;i<=7;i++){
		s[i-4] = message[i];
	}	

	return s;
}

char *helo_to_protocole(char *pseudo, char *pipe){

	char *helo = (char *)malloc(200*sizeof(*helo)), *length_message = (char *)malloc(4*sizeof(*length_message));

	sprintf(pseudo,"%s|%s", pseudo, pipe);
	length_message = nb_to_protocole((int)strlen(pseudo)+12);
	sprintf(helo,"%sHELO%s%s",length_message,nb_to_protocole((int)strlen(pseudo)),pseudo);

	return helo;
}

char *nb_to_protocole(int n){
	/*
	fonction qui permet de convertir un entier représenté en décimal en sa représentation sous forme de quatres caractères.
	*/
	char *nb_protocole = (char *)malloc(4*sizeof(*nb_protocole));

	if(n < 0){
		fprintf(stderr,"nb_to_protocole : erreur nombre négatif.\n");
		return NULL;
	}

	if(n >= 10000){
		fprintf(stderr,"nb_to_protocole : erreur nombre trop grand.\n");
		return NULL;
	}

	if(n < 10)
		sprintf(nb_protocole,"000%d",n);
	else if(n >= 10 && n < 100)
		sprintf(nb_protocole,"00%d",n);
	else if(n >= 100 && n < 1000)
		sprintf(nb_protocole,"0%d",n);
	else
		sprintf(nb_protocole,"%d",n);
	
	return nb_protocole;
}

char *lnb_to_protocole(int n){
	/*
	fonction qui permet de convertir un entier long représenté en décimal en sa représentation sous forme de huits caractères.
	*/
	char *lnb_protocole = (char *)malloc(8*sizeof(*lnb_protocole));

	if(n < 0){
		fprintf(stderr,"lnb_to_protocole : erreur nombre négatif.\n");
		return NULL;
	}

	if(n >= 100000000){
		fprintf(stderr,"nb_to_protocole : erreur nombre trop grand.\n");
		return NULL;
	}

	if(n < 10)
		sprintf(lnb_protocole,"0000000%d",n);
	else if(n >= 10 && n < 100)
		sprintf(lnb_protocole,"000000%d",n);
	else if(n >= 100 && n < 1000)
		sprintf(lnb_protocole,"00000%d",n);
	else if(n >= 1000 && n < 10000)
		sprintf(lnb_protocole,"0000%d",n);
	else if(n >= 10000 && n < 100000)
		sprintf(lnb_protocole,"000%d",n);
	else if(n >= 100000 && n < 1000000)
		sprintf(lnb_protocole,"00%d",n);
	else if(n >= 1000000 && n < 10000000)
		sprintf(lnb_protocole,"0%d",n);
	else
		sprintf(lnb_protocole,"%d",n);
	
	return lnb_protocole;
}

int message_to_total_length(char *message){
	/*
	fonction qui permet d'extraire la longueur total d'un message qui est au format du protocole tchatche_serveur.
	*/
	char total_length[4];
	int i;

	for(i=0;i<4;i++)
		total_length[i] = message[i];
	
	return atoi(total_length);
}

int message_to_body_length(char *message){
	/*
	fonction qui permet d'extraire la longueur du corps d'un message qui est au format du protocole tchatche_serveur.
	*/
	char body_length[4];
	int i;

	for(i=8;i<12;i++)
		body_length[i-8] = message[i];
	
	return atoi(body_length);
}

char *helo_to_pseudo(char *helo_message){
	/*
	fonction qui permet d'extraire le pseudo d'un message de première connnexion.
	*/
	char *pseudo = (char *)malloc(100*sizeof(*pseudo));	
	int i;

	for(i=12;helo_message[i] != '|';i++)
		pseudo[i-12] = helo_message[i];
	
	return pseudo;
}

char *helo_to_pipe(char *helo_message){
	char *pipe = (char *)malloc(100*sizeof(*pipe));
	int i, n, k;

	k = (int)strlen(helo_to_pseudo(helo_message))+13;
	n = message_to_total_length(helo_message);

	fprintf(stdout,"k = %d, n = %d\n", k, n);

	for(i=k;i<n;i++)
		pipe[i-k] = helo_message[i];

	return pipe;
}

char *string_to_protocole(char *s, char *type){

	char *message = (char *)malloc(500*sizeof(*message)), *length_message = (char *)malloc(4*sizeof(*length_message));

	length_message = nb_to_protocole((int)strlen(s)+12);
	sprintf(message,"%s%s%s%s",length_message,type,nb_to_protocole((int)strlen(s)),s);

	return message;
}

char *string_format(char *s){
	
	int i, k = strlen(s);
	char *s2 = (char *)malloc((k-1)*sizeof(*s2));

	strtok(s,"\n");

	for(i=0;i<k;i++)
		s2[i] = s[i];
	
	return s2;
}
