#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conversions.h"
#include "tchatche_serveur.h"

message *m;

void init_string(char *s){
	int i, k = (int)strlen(s);

	for(i=0;i<k;i++)
		s[i] = ' ';
}

message *init_message(){

	message *m = (message *)malloc(sizeof(*m));

	m -> length = (char *)malloc(4*sizeof(*(m -> length)));
	m -> type = (char *)malloc(4*sizeof(*(m -> type)));
	m -> pseudo = (char *)malloc(20*sizeof(*(m -> pseudo)));
	m -> tube = (char *)malloc(20*sizeof(*(m -> tube)));
	m -> id = (char *)malloc(4*sizeof(*(m -> id)));
	m -> message = (char *)malloc(200*sizeof(*(m -> message)));
	m -> n = (char *)malloc(4*sizeof(*(m -> n)));

	return m;
}

void free_message(message *m){
	free(m -> length);
	free(m -> type);
	free(m -> pseudo);
	free(m -> tube);
	free(m -> id);
	free(m -> message);
	free(m -> n);
	free(m);
}

message *build_message(char *type, char *pseudo, char *tube, char *id, char *message, char *n){

	int nb = 4; 
	m = init_message();

	m -> type = type;
	m -> pseudo = pseudo;
	m -> tube = tube;
	m -> id = id;
	m -> message = message;
	m -> n = n;

	if((int)strlen(m -> pseudo) != 0){
		nb += 4;
	}
	else if((int)strlen(m -> tube) != 0){
		nb += 4;
	}
	else if((int)strlen(m -> message) != 0){
		nb += 4;
	}

	m -> length = nb_to_protocole(nb+(int)strlen(type) + (int)strlen(pseudo) + (int)strlen(tube) + (int)strlen(id) + (int)strlen(message) + (int)strlen(n));

	return m;
}

message *parse_server(char *msg){

	message *m = init_message();
	char nombre[4];
	int i, k, nb;

	for(i=0;i<4;i++)
		(m -> length)[i] = msg[i];
	for(k=0;i<8;i++,k++)
		m -> type[k] = msg[i];

	if(!strcmp(m -> type,"HELO")){
		for(k=0;i<12;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> pseudo[k] = msg[i];
		for(k=0;i<nb+4;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> tube[k] = msg[i];
		m -> id = "";
		m -> message = "";
		m -> n = "";
	}

	else if(!strcmp(m -> type,"BYEE") || !strcmp(m -> type,"LIST") || !strcmp(m -> type,"SHUT")){
		for(k=0;i<12;i++,k++)
			m -> id[k] = msg[i];
	}

	else if(!strcmp(m -> type,"BCST")){
		for(k=0;i<12;i++,k++)
			m -> id[k] = msg[i];
		for(k=0;i<16;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> message[k] = msg[i];
	}
	
	else if(!strcmp(m -> type,"PRVT")){
		for(k=0;i<12;i++,k++)
			m -> id[k] = msg[i];
		for(k=0;i<16;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> pseudo[k] = msg[i];
		for(k=0;i<nb+4;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> message[k] = msg[i];
	}
	
	return m;
}


message *parse_client(char *msg){

	message *m = init_message();
	char nombre[4];
	int i, k, nb;

	for(i=0;i<4;i++)
		(m -> length)[i] = msg[i];
	for(k=0;i<8;i++,k++)
		m -> type[k] = msg[i];

	if(!strcmp(m -> type,"OKOK") || !strcmp(m -> type,"BYEE")){
		for(k=0;i<12;i++,k++)
			m -> id[k] = msg[i];
	}

	else if(!strcmp(m -> type,"BCST") || !strcmp(m -> type,"PRVT")){
		for(k=0;i<12;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> pseudo[k] = msg[i];
		for(k=0;i<nb+4;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> message[k] = msg[i];
	}

	else if(!strcmp(m -> type,"LIST")){
		for(k=0;i<12;i++,k++)
			m -> n[k] = msg[i];
		for(k=0;i<16;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> pseudo[k] = msg[i];
	}

	else if(!strcmp(m -> type,"SHUT")){
		for(k=0;i<12;i++,k++)
			nombre[k] = msg[i];
		nb = atoi(nombre)+i;
		for(k=0;i<nb;i++,k++)
			m -> pseudo[k] = msg[i];
	}

	return m;
}

void print_message(message *m){
	fprintf(stdout,"m -> length = %s\n", m -> length);
	fprintf(stdout,"m -> type = %s\n", m -> type);
	fprintf(stdout,"m -> pseudo = %s\n", m -> pseudo);
	fprintf(stdout,"m -> tube = %s\n", m -> tube);
	fprintf(stdout,"m -> id = %s\n", m -> id);
	fprintf(stdout,"m -> message = %s\n", m -> message);
	fprintf(stdout,"m -> n = %s\n\n", m -> n);
}

char *format_protocole_server(message *m){
	char *string = (char *)malloc(2000*sizeof(*string));
	
	sprintf(string,"%s%s", m -> length, m -> type);

	if(!strcmp(m -> type,"OKOK") || !strcmp(m -> type,"BYEE")){
		sprintf(string,"%s%s", string, m -> id);	
	}

	else if(!strcmp(m -> type,"BCST") || !strcmp(m -> type,"PRVT")){
		sprintf(string,"%s%s%s%s%s", string, nb_to_protocole((int)strlen(m -> pseudo)), m -> pseudo, nb_to_protocole((int)strlen(m -> message)), m -> message);
	}

	else if(!strcmp(m -> type,"LIST")){
		sprintf(string,"%s%s%s%s", string, m -> n, nb_to_protocole((int)strlen(m -> pseudo)), m -> pseudo);
	}

	else if(!strcmp(m -> type,"SHUT")){
		sprintf(string,"%s%s%s", string, nb_to_protocole((int)strlen(m -> pseudo)), m -> pseudo);
	}

	return string;
}

char *format_protocole_client(message *m){
	char *string = (char *)malloc(200*sizeof(*string));

	sprintf(string,"%s%s", m -> length, m -> type);

	if(!strcmp(m -> type,"HELO")){
		sprintf(string,"%s%s%s%s%s", string, nb_to_protocole((int)strlen(m -> pseudo)), m -> pseudo, nb_to_protocole((int)strlen(m -> tube)), m -> tube);
		return string;
	}

	sprintf(string,"%s%s", string, m -> id);

	if(!strcmp(m -> type,"BCST")){
		sprintf(string,"%s%s%s", string, nb_to_protocole((int)strlen(m -> message)), m -> message);
	}

	if(!strcmp(m -> type,"PRVT")){
		sprintf(string,"%s%s%s%s%s", string, nb_to_protocole((int)strlen(m -> pseudo)), m -> pseudo, nb_to_protocole((int)strlen(m -> message)), m -> message);
	}

	return string;
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

/*
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
char *helo_to_protocole(char *pseudo, int pid){
	char *helo = (char *)malloc(200*sizeof(*helo)), *length_message = (char *)malloc(4*sizeof(*length_message)), *length_pseudo = (char *)malloc(100*sizeof(*length_pseudo)), *length_tube;
	sprintf(pseudo,"%s|%s", pseudo, pipe);
	length_message = nb_to_protocole((int)strlen(pseudo)+12);
	sprintf(helo,"%sHELO%s%s",length_message,nb_to_protocole((int)strlen(pseudo)),pseudo);
	return helo;
}
int message_to_total_length(char *message){
	char total_length[4];
	int i;
	for(i=0;i<4;i++)
		total_length[i] = message[i];
	
	return atoi(total_length);
}
int message_to_body_length(char *message){
	char body_length[4];
	int i;
	for(i=8;i<12;i++)
		body_length[i-8] = message[i];
	
	return atoi(body_length);
}
char *helo_to_pseudo(char *helo_message){
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
*/
