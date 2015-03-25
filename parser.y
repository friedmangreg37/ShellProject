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
	//clearenv();
  	yyparse();
} 

%}

%union {
	char* sval;
}

%token <sval> SETENV UNSETENV PRINTENV ALIAS UNALIAS CD BYE WORD
%type <sval> set_var unset_var print_var alias unalias change_dir goodbye

%%
commands:
		| commands command
		;
command:
		set_var
		|
		unset_var
		|
		print_var
		|
		alias
		|
		unalias
		|
		change_dir
		|
		goodbye
		;
set_var: 
		SETENV WORD WORD
		{
			setenv($2, $3, 1);
		}
		;
unset_var:
		UNSETENV WORD
		{
			unsetenv($2);
		}
		;
print_var:
		PRINTENV 
		{
			int i = 0;
			while(environ[i])
				puts(environ[i++]);
		}
		;
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
		;
unalias:
		UNALIAS WORD
		{
			removeAlias($2);
		}
		;
change_dir:
		CD
		{
			printf("Change directory to home\n");
		}
		|
		CD WORD
		{
			char* dir = $2;
			printf("%s\n", dir);
			chdir(dir);
		}
		;
goodbye:
		BYE
		{
			printf("Exiting shell now\n");
			exit(0);
		}
		;
%%