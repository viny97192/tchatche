typedef struct user_list user_list;

typedef struct user{
	int id;
	int fd;
	char *pseudo;
	char *pipe;
}user;

struct user_list{
	user val;
	user_list *next;
};

typedef struct utilisateurs{
	user_list *l;
	int nb_users;	
}utilisateurs;

user *init_user(int id, char *pseudo, char *pipe);

user_list *init_user_list(user *usr);

utilisateurs *init_utilisateurs(user *usr);

void free_user(user *u);

void print_user_list(user_list *l);

void free_utilisateurs(utilisateurs *ut);

void print_utilisateurs(user_list *l);

void add_user(user_list *l, user *usr);

user_list *delete_user(user_list *l, int id);

int nb_users(user_list *l);

char *get_pseudo(user_list *l, int id);

int get_fd(user_list *l, int id);

char *get_pipe(user_list *l, int id);

int compare(user *usr1, user *usr2);

int taken_login(user_list *l, char *login);
