#define MAXCMDS 50
#define MAXARGS 300
#define MAXALIAS 100
#define MAXCACHE 100
#define MAXPATH 50

#define OK 0
#define ERRORS 1
#define DONE 2

#define BADFD -2
#define EOFILE 2

#define THE_ONLY_ONE 1
#define FIRST 2
#define LAST 3

#define REGISTRY "REGISTRY"

//built-in commands:
#define PRINTENVIRON 1 
#define GOODBYE 2

//command line arguments
typedef struct comargs {
	char* args[MAXARGS];
} ARGTAB;

//command line
typedef struct com {
	char* comname;
	int remote;
	int infd;
	int outfd;
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

extern aliasNode headAliasNode;
extern aliasNode* aliasHead;
extern COMMAND comtab[];
extern CACHE cachetab[];
extern char* pathtab[];
extern int currcmd;
extern int currcache;
extern int lastcmd;
extern int recursive;
extern int pathlength;
extern int currarg;
extern int login;
extern int builtin;
extern int bicmd;		//specifies which built-in command was read - 0 if not built-in
extern int bioutf;		//Is output being redirected for a built-in?
extern char* bistr;		//string to be used for doing built-ins
extern char* bistr2;	//second string for use with built-ins
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
extern int append;
extern int loop;

#define Allocate(t) (t*)malloc(sizeof(t))