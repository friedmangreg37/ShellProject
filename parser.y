%{
#include "shell.h"

extern char* yytext;
COMMAND *p;
char * match;
glob_t globbuf;
 
void yyerror(const char *str)
{

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

%token <i>	LT GT GGT AMP LPAREN RPAREN BAR ERRORTOOUT TILDE
%token <i>	SETENV UNSETENV PRINTENV CD BYE ALIAS UNALIAS
%token <sval>	WORD MATCH QUEST ERRORREDIR

%type <sval> cmd

%start commands


%%
commands:
		| command
		| command error_redir
		| command background
		| command error_redir background
		;
command:
		| builtin
		| builtin LT WORD
			{ 
				err_msg = "illegal input redirection";
				return 1;	//return error value
			}
		| piped
		| other
		| other input
		| piped input
		| other output
		| piped output
		| other input output
		| piped input output
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
		PRINTENV GGT WORD
		{
			bicmd = PRINTENVIRON;
			bioutf = 1;		//output redirection is true
			bistr = $3;
			append = 1;		//append is true
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
		ALIAS GGT WORD
		{
			bicmd = PRINTALIAS;
			bioutf = 1;	//output redirection is true
			bistr = $3;
			append = 1;	//append is true
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
		CD TILDE
		{
			match = $2; 
			int i = glob(getenv("HOME"),GLOB_TILDE, NULL, &globbuf);
			if(i == 0){
         		for(int j = 0; j < globbuf.gl_pathc; j++)
         		{
         			printf("%s\n", globbuf.gl_pathv[j]);
         		}
         	}
         	else {
         		printf("Error: grammar error\n");
    		}
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

piped:
		other BAR other
		{
			ncmds = currcmd;
		}
		|
		piped BAR other
		{
			ncmds = currcmd;
		}
		;

other: 
		cmd
		{
			bicmd = 0;
			comtab[currcmd].comname = $1;
			comtab[currcmd].nargs = 1;
			(p = &comtab[currcmd])-> atptr = Allocate(ARGTAB);
			p->atptr->args[0] = $1;
			currcmd++;
			ncmds = currcmd;
		}
		|
		cmd arguments
		{
			bicmd = 0;
			comtab[currcmd].comname = $1;
			comtab[currcmd].nargs = currarg;
			comtab[currcmd].atptr->args[0] = $1;
			currcmd++;
			ncmds = currcmd;
		}
		;
		
cmd:	
		WORD 
		{
			$$ = $1;
		}
		;
	
arguments:
		MATCH
		{ 	
			
			(p = &comtab[currcmd])-> atptr = Allocate(ARGTAB);
			currarg = 1;
			match = $1;
			int i = glob(match,GLOB_DOOFFS, NULL, &globbuf);
         	if(i == 0){
         		for(int j = 0; j < globbuf.gl_pathc; j++)
         		{
         			p->atptr->args[currarg++] = globbuf.gl_pathv[j];
         		}
         	}
         	else {
         		printf("Error: grammar error\n");
    		}	
		}
		|
		QUEST
		{ 	
			(p = &comtab[currcmd])-> atptr = Allocate(ARGTAB);
			currarg = 1;
			match = $1;
			int i = glob(match,GLOB_DOOFFS, NULL, &globbuf);
         	if(i == 0){
         		for(int j = 0; j < globbuf.gl_pathc; j++)
         		{
         			p->atptr->args[currarg++] = globbuf.gl_pathv[j];
         		}
         	}
         	else {
         		printf("Error: grammar error\n");
    		}	
		}
		|
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
		|
		arguments MATCH
		{
			p->atptr->args[currarg++] = $2;
		}
		;
		
input:	LT WORD
		{
			comtab[0].infn = $2;
			comtab[0].infd = BADFD;
			inredir = 1;
		}
		;

output:	GT WORD
		{
			comtab[currcmd-1].outfn = $2;	//save name of file
			comtab[currcmd-1].outfd = BADFD;	//tell shell output redirected
			outredir = 1;
		}
		|
		GGT WORD
		{
			comtab[currcmd-1].outfn = $2;
			comtab[currcmd-1].outfd = BADFD;
			outredir = 1;
			append = 1;
		}
		;
error_redir:	ERRORTOOUT
				{
					fperror = stdout;	//connect stderr to stdout
				}
				|
				ERRORREDIR
				{
					fperror = fopen($1, "w");		//try to open the file
					if(fperror == NULL) {
						err_msg = strerror(errno);
						return 1;
					}
					errredir = 1;
				}
				;

background:		AMP
				{
					background = 1;
					printf("do something in background\n");
				}
%%