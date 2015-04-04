#define MAXCMDS 50
#define MAXARGS 300
#define MAXALIAS 100
#define MAXCACHE 100
#define MAXPATH 50

#define OK 0
#define SYSERR 1
#define OLD_ERR 3
#define SYSCALLERR -1

#define BADFD -2
#define EOFILE 2

#define THE_ONLY_ONE 1
#define FIRST 2
#define LAST 3

#define REGISTRY "REGISTRY"

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
extern int bicmd;
extern int bioutf;
extern char* bistr;
extern char* bistr2;
extern int debug;
extern int IwasSet;
extern int err;
extern int error_somewhere;
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