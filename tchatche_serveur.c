#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tchatche_serveur.h"
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	
	user *u = init_user(1,"amine","toto"), *u2 = init_user(2,"vincent","tutu");
	utilisateurs *ut = init_utilisateurs();

	add_user(ut,u);
	print_user(&(ut -> t[0]));
	add_user(ut,u2);
	print_user(&(ut -> t[1]));

	/*
	int fd;
	char *fifo = "tchatche_serveur";
	user connected[1];

	if((mkfifo(fifo, S_IRWXU | S_IROTH | S_IWOTH | S_IRGRP)) == -1){
		perror("mkfifo ");
		exit(EXIT_FAILURE);
	}

	if((fd = open(fifo, O_RDWR)) == -1){
		perror("open ");
		exit(EXIT_FAILURE);
	}

	while(type != 8){
			
	}	
	*/



	return 0;
}

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

user *init_user(int id, char *pseudo, char *pipe){
	user *u = (user *)malloc(sizeof(*u));

	u -> id = id;
	u -> pseudo = pseudo;
	u -> pipe = pipe;

	return u;
}

void print_user(user *u){
	fprintf(stdout,"id \t\t: %d\npseudo \t\t: %s\ntube \t\t: %s\n\n", u -> id, u -> pseudo, u -> pipe);
}

utilisateurs *init_utilisateurs(){
	
	utilisateurs *ut = (utilisateurs *)malloc(sizeof(*ut));

	ut -> t = (user *)malloc(sizeof(*(ut -> t)));
	ut -> t[0].id = -1;
	ut -> capacite = 1;
	ut -> occupation = 0;

	return ut;
}

/*
void print_utilisateurs(utilisateurs *ut){
	fprintf(stdout,"")
}
*/

void add_user(utilisateurs *ut, user *usr){
	int i;
	if(usr == NULL){
		fprintf(stderr,"Erreur, utilisateur non valide.\n");
		return;
	}

	if(ut -> capacite == ut -> occupation){
		ut -> t = realloc(ut -> t, (2*(ut -> capacite))*sizeof(user));
		ut -> capacite *= 2;
		ut -> t[ut -> occupation] = *usr;
		usr -> id = ut -> occupation;
		ut -> occupation += 1;
		return;
	}
	

	for(i=0;i<ut -> capacite;i++){
		if((ut -> t[i].id) == -1){
			ut -> t[i] = *usr;
			ut -> occupation += 1;
		}
	}
	
}
