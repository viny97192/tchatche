typedef struct user{
	int id;
	char *pseudo;
	char *pipe;
}user;

typedef struct utilisateurs{
	user *t;
	int capacite;
	int occupation;	
}utilisateurs;

/*
typedef struct message{
	int length;
	char type[4];
	char *string;
}message;
*/

user *init_user(int id, char *pseudo, char *pipe);

utilisateurs *init_utilisateurs();

void free_user(user *u);

void free_utilisateurs(utilisateurs *ut);

void print_user(user *u);

void print_utilisateurs(utilisateurs *ut);

void add_user(utilisateurs *ut, user *usr);

void delete_user(user *usr);

int type_message(char *message);

char *type(char *message);
