%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "shell.h"

extern char** environ;
extern char* yytext;
 
void yyerror(const char *str)
{
    fprintf(stderr,"error: %s\n",str);
    yyparse();
}
 
int yywrap()
{
    return 1;
} 

%}

%union {
	int i;
	char* sval;
}

%token <i>	LT GT AMP LPAREN RPAREN BAR DOT QUOTE NEWLINE
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
		SETENV WORD WORD NEWLINE
		{
			setenv($2, $3, 1);
		}
		|
		UNSETENV WORD NEWLINE
		{
			unsetenv($2);
		}
		|
		PRINTENV NEWLINE
		{
			int i = 0;
			while(environ[i])
				puts(environ[i++]);
		}
alias:
		ALIAS NEWLINE
		{
			printAliases();
		}
		|
		ALIAS WORD WORD NEWLINE
		{
			insertAlias($2, $3);
		}
		|
		UNALIAS WORD NEWLINE
		{
			removeAlias($2);
		}
directory:
		CD WORD NEWLINE
		{
			char* dir = $2;
			chdir(dir);
		}
		|
		CD NEWLINE
		{
			char* home = getenv("HOME");
			chdir(home);
		}
		|
		PWD NEWLINE
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
		BYE NEWLINE
		{
			printf("Exiting shell now\n");
			exit(0);
		}
		;
%%