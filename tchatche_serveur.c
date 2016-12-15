#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tchatche_serveur.h"
#include "conversions.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void response_to_client(message *m);

int id =  1;
utilisateurs *ut;

int main(int argc, char *argv[]){

	char *fifo_client = (char *)malloc(100*sizeof(*fifo_client)), *request = (char *)malloc(100*sizeof(*request)); 
	int fd_server, fd_dummy;
	message *m; 
	
	if(mkfifo("fifo_server", 0666) == -1){
		fprintf(stderr,"mkfifo fifo_server\n");
		exit(EXIT_FAILURE);
	}

	if((fd_server = open("fifo_server", O_RDONLY)) == -1){
		fprintf(stderr,"open fifo_server lecture");
		exit(EXIT_FAILURE);
	}

	if((fd_dummy = open("fifo_server", O_WRONLY)) == -1){
		fprintf(stderr,"open fifo_server écriture");
		exit(EXIT_FAILURE);
	}

	strcpy(fifo_client,"fifo_client.");

	while(1){

		if(read(fd_server,request,100) == -1){
			fprintf(stderr,"Impossible de lire la requete du client.\n");
			exit(EXIT_FAILURE);
		}

		m = parse_server(request);
		response_to_client(m);
		strcpy(request,"");

	}

	unlink("fifo_serv");
	return 0;
}

void response_to_client(message *m){

	char *response = (char *)malloc(100*sizeof(*response));
	message *m2;

	if(!strcmp(m -> type,"HELO")){
		user *u = init_user(id, m -> pseudo, m -> tube);

		if((u -> fd = open(u -> pipe, O_WRONLY)) == -1){
			fprintf(stderr,"open %s", u -> pipe);
			return;
		}

		if(id == 1 ||  ut -> nb_users == 0){
			ut = init_utilisateurs(u);		
			ut -> nb_users++;
			id++;
		}

		else{
			add_user(ut -> l, u);
			id++;
		}

		m2 = build_message("OKOK","","",nb_to_protocole(u -> id),"","");
		response = format_protocole_server(m2);

		if(write(u -> fd,response,strlen(response)) == -1){
			fprintf(stderr,"write %s", u -> pipe);
			return;
		}
	}

	else if(!strcmp(m -> type,"BCST")){
	
		user_list *tmp = ut -> l;

		m2 = build_message("BCST",get_pseudo(ut -> l, atoi(m -> id)),"","",m -> message,"");
		response = format_protocole_server(m2);
			
		while(tmp != NULL){
			if(tmp -> val.id != atoi(m -> id)){
				if(write(tmp -> val.fd,response,strlen(response)) == -1){
					fprintf(stderr,"Impossible d'écrire dans le tube %s\n", tmp -> val.pipe);	
					continue;
				}
			}
			tmp = tmp -> next;
		}
	}

	else if(!strcmp(m -> type,"BYEE")){
		int fd = get_fd(ut -> l, atoi(m -> id));
		response = format_protocole_server(m);
		
		if(write(fd,response,strlen(response)) == -1){
			fprintf(stderr,"write BYEE\n");
			return;
		}

		if(close(fd) == -1){
			fprintf(stderr,"close BYEE\n");
			return;
		}

		ut -> l = delete_user(ut -> l, atoi(m -> id));
		ut -> nb_users--;
	}

	else if(!strcmp(m -> type,"PRVT")){
		int fd = get_fd(ut -> l, atoi(m -> id)), fd2 = get_fd2(ut -> l, m -> pseudo);	
		
		m2 = build_message("PRVT",get_pseudo(ut -> l, atoi(m -> id)),"","",m -> message,"");
		response = format_protocole_server(m2);

		if(write(fd2,response,strlen(response)) == -1){
			fprintf(stderr,"write %s", m -> pseudo);
			return;
		}

		m2 = build_message("OKOK","","",m -> id,"","");
		response = format_protocole_server(m2);

		if(write(fd,response,strlen(response)) == -1){
			fprintf(stderr,"write %s", m -> pseudo);
			return;
		}

	}
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

void add_user(user_list *l, user *usr){

	if(l == NULL){
		fprintf(stderr,"Erreur, le serveur n'a pas été initalisé.\n");
		return;
	}

	if(usr == NULL){
		fprintf(stderr,"Erreur, l'utilisateur n'a pas été initalisé.\n");
		return;
	}

	if(l -> next == NULL){
		user_list *lusr = init_user_list(usr);
		l -> next = lusr;
		return;
	}

	add_user(l -> next,usr);
}

user_list *delete_user(user_list *l, int id){

	user_list *tmp = l, *tmp2;

	if(l == NULL){
		fprintf(stderr,"Erreur, aucun utilisateur n'est connecté.\n");
		return NULL;
	}

	if(id <= 0){
		fprintf(stderr,"Erreur, identifiant négatif.\n");
		return l;
	}

	/*
	if(!taken_login(l,usr -> pseudo)){
		fprintf(stderr,"Attention, cet utilisateur n'est pas connecté.\n");
		return l;
	}
	*/

	if(l -> next == NULL){
		free(l);	
		return l;
	}

	if(l -> val.id == id){
		l = l -> next;
		free(tmp);
		return l;
	}

	if(l -> next -> val.id == id){
		tmp = l -> next;
		l -> next = l -> next -> next;
		free(tmp);
		return l;
	}

	while(tmp -> next -> next != NULL){
		if(tmp -> next -> val.id == id){
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

char *get_pseudo(user_list *l, int id){	
	if(l == NULL){
		fprintf(stderr,"Attention liste non initialisée.\n");
		return NULL;
	}
	
	if(id <= 0){
		fprintf(stderr,"Attention identifiant négatif.\n");
		return NULL;
	
	}

	if(id == l -> val.id)
		return l -> val.pseudo;
	
	if(l -> next == NULL && id != l -> val.id)
		return NULL;

	return get_pseudo(l -> next, id);
}

int get_fd(user_list *l, int id){
	if(l == NULL){
		fprintf(stderr,"Attention liste non initialisée.\n");
		return -1;
	}
	
	if(id <= 0){
		fprintf(stderr,"Attention identifiant négatif.\n");
		return -1;
	
	}

	if(id == l -> val.id){
		/*
		fprintf(stdout,"l -> val.fd = %d\n", l -> val.fd);
		*/
		return l -> val.fd;
	}
	
	if(l -> next == NULL && id != l -> val.id)
		return -1;

	return get_fd(l -> next, id);
}

int get_fd2(user_list *l, char *pseudo){
	if(l == NULL){
		fprintf(stderr,"Attention liste non initialisée.\n");
		return -1;
	}
	
	if(pseudo == NULL){
		fprintf(stderr,"Attention pseudo non initialisé.\n");
		return -1;
	
	}

	if(!strcmp(l -> val.pseudo,pseudo)){
		return l -> val.fd;
	}
	
	if(l -> next == NULL && strcmp(l -> val.pseudo,pseudo))
		return -1;

	return get_fd2(l -> next, pseudo);
}

char *get_pipe(user_list *l, int id){
	if(l == NULL){
		fprintf(stderr,"Attention liste non initialisée.\n");
		return NULL;
	}
	
	if(id <= 0){
		fprintf(stderr,"Attention identifiant négatif.\n");
		return NULL;
	
	}

	if(id == l -> val.id){
		/*
		fprintf(stdout,"l -> val.fd = %d\n", l -> val.fd);
		*/
		return l -> val.pipe;
	}
	
	if(l -> next == NULL && id != l -> val.id)
		return NULL;

	return get_pipe(l -> next, id);
}
/*
int compare(user *usr1, user *usr2){
	int taken = 0;

	if(usr1 == NULL || usr2 == NULL)
		taken = -1;
	
	else if(!strcmp(usr1 -> pseudo, usr2 -> pseudo) && !strcmp(usr1 -> pipe, usr2 -> pipe))
		taken = 3;
	else if(!strcmp(usr1 -> pseudo, usr2 -> pseudo))
		taken = 1;
	else if(!strcmp(usr1 -> pipe, usr2 -> pipe))
		taken = 2;

	return taken;
}
*/

int taken_login(user_list *l, char *login){
	if(l == NULL){
		fprintf(stderr,"Attention, liste d'utilisateurs non initialisée.\n");
		return -1;
	}

	if(login == NULL){
		fprintf(stderr,"Attention, pseudo non initialisé.\n");
		return -1;
	}

	if(l -> next == NULL)
		return strcmp(l -> val.pseudo,login);
	
	if(!strcmp(l -> val.pseudo,login))
		return 0;
	return taken_login(l -> next,login);
}
