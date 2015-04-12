#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAXCMDS 50
#define MAXARGS 300
#define MAXALIAS 100
#define MAXCACHE 100
#define MAXPATH 50
#define MAXPIPES 20

#define OK 0
#define NONBUILTIN 1
#define ERRORS 2
#define DONE 3

#define BADFD -2
#define EOFILE 2

//built-in commands:
#define PRINTENVIRON 1 
#define GOODBYE 2
#define SETENVIRON 3
#define UNSETENVIRON 4
#define PRINTALIAS 5
#define SETALIAS 6
#define UNSETALIAS 7
#define CHANGEDIR 8

#define PIPE_ERR -1 	//error for exceeding max number of pipes
#define QUOTES_ERR -2	//error for mismatched quotes within string

#define EXECERROR -1 	//error when failing to exec - need to exit forked process
#define OTHERERROR -2	//another type of error occurring - set err_msg appropriately

//command line arguments
typedef struct comargs {
	char* args[MAXARGS];	//array for arguments of a command
} ARGTAB;

//command line
typedef struct com {
	char* comname;	//name of this command
	int infd;		//input file descriptor
	char* infn;		//input redirect filename
	int outfd;		//output file descriptor
	char* outfn;	//output redirect filename
	int nargs;		//number of arguments for this command
	ARGTAB* atptr;	//pointer to the argument array
} COMMAND;

typedef struct aliasNode {
	char* name;		//name of the alias
	char* word;		//what the name stands for
	struct aliasNode* next;		//next alias in linked list
} aliasNode;

extern aliasNode headAliasNode;	//head node of alias node linked list - null node
extern aliasNode* aliasHead;	//pointer to head of alias linked list
extern COMMAND comtab[];	//table to hold the commands in a line of input
extern int currcmd;		//current command for entry in command table
extern ncmds;			//number of piped commands in the line of input
extern int currarg;		//current argument in command table entry's arbtab
extern char** environ;	//array of environment variables
extern int bicmd;		//specifies which built-in command was read - 0 if not built-in
extern int bioutf;		//Is output being redirected for a built-in?
extern char* bistr;		//string to be used for doing built-ins
extern char* bistr2;	//second string for use with built-ins
extern int nPipes;		//number of pipes in a command line
extern int cmds[MAXPIPES];	//array holding offset of each piped command within command line string
extern int inredir;		//Is input redirected?
extern int outredir;	//Is output redirected?
extern int errredir;	//Is error redirected?
extern FILE* fperror;	//file pointer for error
extern char* err_msg;	//string for error message
extern int background;		//Should command run in background?
extern char* prompt_string;		//string to output when printing prompt
extern int append;		//Do we append output redirection?

#define Allocate(t) (t*)malloc(sizeof(t))