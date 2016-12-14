typedef struct message{
	char *length;
	char *type;
	char *pseudo;
	char *tube;
	char *id;
	char *message;
	char *n;
}message;

message *init_message();

void free_message(message *m);

void print_message(message *m);

message *build_message(char *type, char *pseudo, char *tube, char *id, char *message, char *n);

message *parse_client(char *msg);

message *parse_server(char *msg);

char *format_protocole_server(message *m);

char *format_protocole_client(message *m);

char *nb_to_protocole(int n);

char *lnb_to_protocole(int n);

int type_message(char *message);

char *type(char *message);

char *helo_to_protocole(char *pseudo, char *pipe);

char *helo_to_pseudo(char *helo_message);

char *helo_to_pipe(char *helo_message);

int message_to_total_length(char *message);

int message_to_body_length(char *message);

char *string_to_protocole(char *s, char *type);

char *string_format(char *s);
