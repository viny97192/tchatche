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

int id = 2;

int main(int argc, char *argv[]){
	
	
	/*= init_user(0,"amine","toto"), *u2 = init_user(1,"vincent","tutu"), *u3 = init_user(2,"andy","tata"), *u4 = init_user(3,"paul","titi"), *u5 = init_user(4,"mathieu","tete"), *u6 = init_user(5,"paul","tyty");
	int n;
	
	add_user(ut -> l,u2);
	add_user(ut -> l,u3);
	add_user(ut -> l,u4);
	add_user(ut -> l,u5);
	add_user(ut -> l,u6);
	print_utilisateurs(ut -> l);
	n = nb_users(ut -> l);
	printf("n = %d\n\n\n", n);

	ut -> l = delete_user(ut -> l, u5);
	print_utilisateurs(ut -> l);

	free_user(u);
	free_user(u2);
	free_user(u3);
	free_user(u4);
	free_user(u5);
	free_user(u6);
	
	free_utilisateurs(ut);
	*/

	user *u1 = init_user(1,"amine","toto");
	utilisateurs *ut = init_utilisateurs(u1);
	int fd_server, fd_client;
	char reader[BUFF_SIZE], *pseudo = (char *)malloc(100*sizeof(*pseudo)), *pipe = (char *)malloc(100*sizeof(*pipe));

	if(mkfifo("server",0737) != 0){
		perror("mkfifo ");
		exit(EXIT_FAILURE);
	}

	if((fd_server = open("server", O_RDONLY)) == -1){
		perror("open, ouverture du serveur en lecture chez le serveur");
		exit(EXIT_FAILURE);
	}

	if((fd_client = open("client", O_WRONLY)) == -1){
		perror("open, ouverture du client en écriture chez le serveur ");
		exit(EXIT_FAILURE);
	}

	read(fd_server,reader,BUFF_SIZE);

	if(type_message(reader) == 1){
		printf("type_message = 1\n");
		user *u;
		char *s, *ide = (char *)malloc(10*sizeof(*ide));
		
		pseudo = helo_to_pseudo(reader);
		pipe = helo_to_pipe(reader);
		u = init_user(-1,pseudo,pipe); 

		if(taken_login(ut -> l,u)){
			s = string_to_protocole("pseudo déjà utilisé","BADD");
			write(fd_client,s,BUFF_SIZE);
			free(u);
			exit(EXIT_FAILURE);
		}

		else{
			u -> id = id++;
			add_user(ut -> l,u);
			sprintf(ide,"%d",u -> id);
			s = string_to_protocole(ide,"OKOK");
			write(fd_client,s,BUFF_SIZE);
		}	
	}

	print_utilisateurs(ut -> l);	

	return 0;
}

user *init_user(int id, char *pseudo, char *pipe){
	user *u = (user *)malloc(sizeof(*u));

	u -> id = id;
	u -> pseudo = pseudo;
	u -> pipe = pipe;

	return u;
}

user_list *init_user_list(user *usr){
	user_list *l = (user_list *)malloc(sizeof(*l));

	l -> val = *usr;
	l -> next = NULL;

	return l;
}

utilisateurs *init_utilisateurs(user *usr){
	
	utilisateurs *ut = (utilisateurs *)malloc(sizeof(*ut));

	ut -> l = init_user_list(usr);
	ut -> nb_users = 0;
	ut -> l -> next = NULL;

	return ut;
}

void free_user(user *u){
	free(u);
}

void free_utilisateurs(utilisateurs *ut){
	
	user_list *tmp;

	if(ut == NULL || ut -> l == NULL)
		return;

	if(ut -> l -> next == NULL){
		free(ut -> l);
		free(ut);
		return;
	}
	
	while(ut -> l != NULL){
		tmp = ut -> l -> next;
		free(ut -> l);
		ut -> l = tmp;
	}
	
	free(ut);
}

void print_user(user *u){
	fprintf(stdout,"id \t\t: %d\npseudo \t\t: %s\ntube \t\t: %s\n\n", u -> id, u -> pseudo, u -> pipe);
}

void print_utilisateurs(user_list *l){
	if(l == NULL){
		fprintf(stderr,"Attention, aucun utilisateur n'est connecté.\n");
		return;
	}

	if(l -> next == NULL){
		print_user(&(l -> val));
		return;
	}

	print_user(&(l -> val));
	print_utilisateurs(l -> next);
}

int add_user(user_list *l, user *usr){
	int taken;

	if(l == NULL){
		fprintf(stderr,"Erreur, le serveur n'a pas été initalisé.\n");
		return -2;
	}

	if(usr == NULL){
		fprintf(stderr,"Erreur, l'utilisateur n'a pas été initalisé.\n");
		return -1;
	}

	if((taken = taken_login(l, usr))){
		return taken;
	}

	if(l -> next == NULL){
		user_list *lusr = init_user_list(usr);
		l -> next = lusr;
		return 4;
	}

	return add_user(l -> next,usr);
}

user_list *delete_user(user_list *l, user *usr){

	user_list *tmp = l, *tmp2;

	if(l == NULL){
		fprintf(stderr,"Erreur, aucun utilisateur n'est connecté.\n");
		return NULL;
	}

	if(usr == NULL){
		fprintf(stderr,"Erreur, l'utilisateur n'a pas été initalisé.\n");
		return l;
	}

	if(!taken_login(l,usr)){
		fprintf(stderr,"Attention, cet utilisateur n'est pas connecté.\n");
		return l;
	}

	if(l -> next == NULL){
		free(l);	
		return NULL;
	}

	if(!strcmp(l -> val.pseudo,usr -> pseudo)){
		l = l -> next;
		free(tmp);
		return l;
	}

	if(!strcmp(l -> next -> val.pseudo,usr -> pseudo)){
		tmp = l -> next;
		l -> next = l -> next -> next;
		free(tmp);
		return l;
	}

	while(tmp -> next -> next != NULL){
		if(!strcmp(tmp -> next -> val.pseudo,usr -> pseudo)){
			tmp2 = tmp -> next;
			tmp -> next = tmp -> next -> next;
			free(tmp2);
			return l;
		}

		tmp = tmp -> next;
	}

	free(tmp -> next);
	tmp -> next = NULL;
	return l;
}

int nb_users(user_list *l){
	if(l == NULL)
		return 0;
	if(l -> next == NULL)
		return 1;
	return 1 + nb_users(l -> next);
}

int compare(user *usr1, user *usr2){
	int taken = 0;

	if(usr1 == NULL||usr2 == NULL)
		taken = -1;
	
	else if(!strcmp(usr1 -> pseudo, usr2 -> pseudo) && !strcmp(usr1 -> pipe, usr2 -> pipe))
		taken = 3;
	else if(!strcmp(usr1 -> pseudo, usr2 -> pseudo))
		taken = 1;
	else if(!strcmp(usr1 -> pipe, usr2 -> pipe))
		taken = 2;

	return taken;
}

int taken_login(user_list *l, user *usr){
	if(l == NULL){
		fprintf(stderr,"Attention, liste d'utilisateurs non initialisée.\n");
		return -1;
	}

	if(usr == NULL){
		fprintf(stderr,"Attention, utilisateur non initialisé.\n");
		return -1;
	}

	if(l -> next == NULL){
		return compare(&(l -> val), usr);	
	}

	if(!compare(&(l -> val),usr)){
		return taken_login(l -> next,usr);
	}
	return compare(&(l -> val),usr);
}

