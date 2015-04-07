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
    //fprintf(stderr,"error: %s\n",str);
    //yyparse();
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

%token <i>	LT GT AMP LPAREN RPAREN BAR DOT
%token <i>	SETENV UNSETENV PRINTENV CD BYE ALIAS UNALIAS PWD
%token <sval>	WORD 

%%
commands:
		| commands command
		;
command:
		| builtin
		| builtin LT WORD
			{ printf("Error: illegal input redirection\n"); }
builtin:
		SETENV WORD WORD
		{
			bicmd = SETENVIRON;	//set builtin command
			bistr = $2;
			bistr2 = $3;
		}
		|
		UNSETENV WORD
		{
			bicmd = UNSETENVIRON;	//set builtin command
			bistr = $2;
		}
		|
		PRINTENV
		{
			bicmd = PRINTENVIRON;	//set builtin command
			bioutf = 0;		//output redirection is false
		}
		|
		PRINTENV GT WORD
		{
			bicmd = PRINTENVIRON;
			bioutf = 1;		//output redirection is true
			bistr = $3;
		}
		|
		ALIAS
		{
			bicmd = PRINTALIAS;
		}
		|
		ALIAS GT WORD
		{
			bicmd = PRINTALIAS;
			bioutf = 1;	//output redirection is true
			bistr = $3;		//filename
		}
		|
		ALIAS WORD WORD
		{
			bicmd = SETALIAS;
			bistr = $2;
			bistr2 = $3;
		}
		|
		UNALIAS WORD
		{
			bicmd = UNSETALIAS;
			bistr = $2;
		}
		|
		CD WORD
		{
			bicmd = CHANGEDIR;
			bistr = $2;
		}
		|
		CD
		{
			bicmd = CHANGEDIR;
			bistr = NULL;
		}
		|
		BYE
		{
			bicmd = GOODBYE;
		}
		;
%%