int type_message(char *message);

char *type(char *message);

char *helo_to_protocole(char *pseudo, char *pipe);

char *nb_to_protocole(int n);

char *lnb_to_protocole(int n);

char *helo_to_pseudo(char *helo_message);

char *helo_to_pipe(char *helo_message);

int message_to_total_length(char *message);

int message_to_body_length(char *message);

char *string_to_protocole(char *s, char *type);

char *string_format(char *s);
