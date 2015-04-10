%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "shell.h"

extern char** environ;
extern char* yytext;
COMMAND *p;

 
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

%type <sval> cmd

%start commands


%%
commands:
		| commands command
		;
command:
		| builtin
		| builtin LT WORD
			{ printf("Error: illegal input redirection\n"); }
		| other 	
		| other LT WORD
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

other: 
		cmd
		{
			bicmd = 0;
			comtab[currcmd].comname = $1;
			comtab[currcmd].atptr = NULL;
			comtab[currcmd].nargs = 0;
		}
		|
		cmd arguments
		{
			bicmd = 0;
			comtab[currcmd].comname = $1;
			comtab[currcmd].nargs = currarg;
		}
		;
		
cmd:	
		WORD 
		{
			$$ = $1;
		}
		;
	
arguments:
		WORD
		{
			(p = &comtab[currcmd])-> atptr = Allocate(ARGTAB);
			currarg = 1;
			p->atptr->args[currarg++] = $1;
		}
		|
		arguments WORD
		{
			p->atptr->args[currarg++] = $2;
		}
		;

words: 
		WORD WORD
		|
		words WORD
		;
		

		
		

%%