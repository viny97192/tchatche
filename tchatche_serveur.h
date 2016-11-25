typedef struct user_list user_list;

typedef struct user{
	int id;
	char *pseudo;
	char *pipe;
}user;

struct user_list{
	user val;
	user_list *next;
};

typedef struct utilisateurs{
	user_list *l;
	/*
	int capacite;
	*/
	int nb_users;	
}utilisateurs;

/*
typedef struct message{
	int length;
	char type[4];
	char *string;
}message;
*/

user *init_user(int id, char *pseudo, char *pipe);

user_list *init_user_list(user *usr);

utilisateurs *init_utilisateurs(user *usr);

void free_user(user *u);

void free_utilisateurs(utilisateurs *ut);

void print_user(user *u);

void print_utilisateurs(user_list *l);

int add_user(user_list *l, user *usr);

user_list *delete_user(user_list *l, user *usr);

int nb_users(user_list *l);

int compare(user *usr1, user *usr2);

int taken_login(user_list *l, user *usr);

