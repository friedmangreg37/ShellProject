#define MAXCMDS 50
#define MAXARGS 300
#define MAXALIAS 100
#define MAXCACHE 100
#define MAXPATH 50
#define MAXPIPES 10

#define OK 0
#define NONBUILTIN 1
#define ERRORS 2
#define DONE 3

#define BADFD -2
#define EOFILE 2

#define THE_ONLY_ONE 1
#define FIRST 2
#define LAST 3

#define REGISTRY "REGISTRY"

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

#define DOUBLE_QUOTE '\"'

//command line arguments
typedef struct comargs {
	char* args[MAXARGS];
} ARGTAB;

//command line
typedef struct com {
	char* comname;
	int remote;
	int infd;
	char* infn;		//input redirect filename
	int outfd;
	char* outfn;	//output redirect filename
	int nargs;
	ARGTAB* atptr;
} COMMAND;

typedef struct aliasNode {
	char* name;
	char* word;
	struct aliasNode* next;
} aliasNode;

//cache table
typedef struct cache {
	char* cmd;
	char* hostport;
} CACHE;

extern aliasNode headAliasNode;	//head node of alias node linked list - null node
extern aliasNode* aliasHead;	//pointer to head of alias linked list
extern COMMAND comtab[];	//table to hold the commands in a line of input
extern CACHE cachetab[];
extern char* pathtab[];
extern int currcmd;		//current command for entry in command table
extern ncmds;			//number of piped commands in the line of input
extern int currcache;
extern int lastcmd;
extern int recursive;
extern int pathlength;
extern int currarg;		//current argument in command table entry's arbtab
extern int login;
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
extern int debug;
extern int IwasSet;
extern int err;
extern int error_somewhere;
extern char* err_msg;	//string for error message
extern int ap;
extern int cmd_has_tail;
extern int background;
extern int concurrp;
extern int eventcount;
extern char home[];
extern char* shname;
extern char* prompt_string;
extern int user_defined_prompt;
extern char srcf[];
extern char distf[];
extern int append;		//Do we append output redirection?
extern int loop;

#define Allocate(t) (t*)malloc(sizeof(t))