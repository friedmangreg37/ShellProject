%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

extern char** environ;
extern char* yytext;

typedef struct aliasNode {
	char* name;
	char* word;
	struct aliasNode* next;
} aliasNode;

aliasNode headNode = { NULL, NULL, NULL };
aliasNode* head = &headNode;

int insertAlias(char* theName, char* theWord) {
	aliasNode* newNode = (aliasNode*)malloc(sizeof(aliasNode));
	if(newNode == NULL)
		return -1;
	newNode -> name = theName;
	newNode -> word = theWord;
	newNode -> next = head -> next;
	head -> next = newNode;
	return 0;
}

void removeAlias(char* theName) {
	aliasNode* curr, *prev;
	curr = head -> next;
	prev = head;
	while(curr != NULL && (strcmp(curr->name, theName) != 0)) {
		prev = curr;
		curr = curr->next;
	}
	if(curr == NULL) {
		printf("not found\n");
		return;
	}
	prev->next = curr->next;
	free(curr);
}

void printAliases() {
	aliasNode* temp = head -> next;
	while(temp != NULL) {
		printf("%s:%s\n", temp->name, temp->word);
		temp = temp->next;
	}
}
 
void yyerror(const char *str)
{
    fprintf(stderr,"error: %s\n",str);
    yyparse();
}
 
int yywrap()
{
    return 1;
} 
  
main()
{
  	yyparse();
} 

%}

%union {
	int i;
	char* sval;
}

%token <i>	LT GT AMP LPAREN RPAREN BAR DOT QUOTE
%token <i>	SETENV UNSETENV PRINTENV CD BYE ALIAS UNALIAS PWD
%token <sval>	WORD
%type <sval> env alias directory goodbye

%%
commands:
		| commands command
		;
command:
		env
		|
		alias
		|
		directory
		|
		goodbye
		;
env: 
		SETENV WORD WORD
		{
			setenv($2, $3, 1);
		}
		|
		UNSETENV WORD
		{
			unsetenv($2);
		}
		|
		PRINTENV 
		{
			int i = 0;
			while(environ[i])
				puts(environ[i++]);
		}
alias:
		ALIAS
		{
			printAliases();
		}
		|
		ALIAS WORD WORD
		{
			insertAlias($2, $3);
		}
		|
		UNALIAS WORD
		{
			removeAlias($2);
		}
directory:
		CD WORD
		{
			char* dir = $2;
			printf("%s\n", dir);
			chdir(dir);
		}
		|
		CD
		{
			char* home = getenv("HOME");
			printf("Change directory to %s\n", home);
			chdir(home);
		}
		|
		PWD
			{ 
				char * buf;
    			char * cwd;
    			buf = (char *)malloc(sizeof(char) * 1024);

    			if((cwd = getcwd(buf, 1024)) != NULL)
            		printf("pwd : %s\n", cwd);
    			else
           			perror("getcwd() error : ");
			}
goodbye:
		BYE
		{
			printf("Exiting shell now\n");
			exit(0);
		}
		;
%%