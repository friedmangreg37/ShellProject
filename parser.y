%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

extern char** environ;
extern char* yytext;
 
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
	//clearenv();
  	yyparse();
} 

%}

%union {
	char* sval;
}

%token <sval> SETENV UNSETENV PRINTENV CD BYE WORD
%type <sval> set_var unset_var print_var change_dir goodbye

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
change_dir:
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