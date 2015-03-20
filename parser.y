%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#ifndef YYSTYPE
#define YYSTYPE char*
#endif
 
void yyerror(const char *str)
{
        fprintf(stderr,"error: %s\n",str);
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

%token CD BYE
%token WORD

%%
commands:
		| commands command
		;
command:
		change_dir
		|
		goodbye
		;
change_dir:
		CD 	{ printf("Change directory to home\n"); }
		|
		CD WORD	{ printf("Change directory to %s\n", $2); }
		;
goodbye:
		BYE
		{
			printf("Exiting shell now\n");
			exit(0);
		}
		;
%%