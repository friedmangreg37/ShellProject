#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "shell.h"
#include <errno.h>
#include <string.h>

char* prompt_string;
int currcmd;
int currarg;
aliasNode headAliasNode;
aliasNode* aliasHead;
char** environ;
int bicmd, bioutf;
char* bistr;
char* bistr2;
FILE* fpin;
FILE* fpout;
FILE* fperror;
int inRedirect = 0;
char inFilename[80];
int cmd;
char inStr[80];
char* err_msg;
COMMAND comtab[10];
ARGTAB args[MAXARGS];

void initShell(), printPrompt(), removeAlias(char*), printAliases(FILE*), initScanner();
int processCommand(), do_it(), execute_it();
int insertAlias(char*, char*), getCommand();
char* findAlias(char*);

int main(int argc, char** argv) {
	initShell();
	if( (argc > 1) && (strcmp(argv[1], "<")) ) {	//if input redirected
		inRedirect = 1;		//set to true
		strcpy(inFilename, argv[2]);
		//open file
		fpin = fopen(inFilename, "r");
		if(fpin == NULL) {
			printf("Input file %s error\n", inFilename);
			exit(0);
		}
	}
	while(1) {
		printPrompt();
		int ret;
		switch(cmd = getCommand()) {
			case DONE:
				if(inRedirect)	//if input was redirected, close file
					fclose(fpin);
				printf("Exiting shell now\n");
				exit(0);
			case OK:
				ret = processCommand();
				if(ret < 0) {
					fprintf(fperror, "Error: %s\n", err_msg);
					if(ret == EXECERROR)
						exit(0);	//need to exit from forked process
				}
				break;
			case ERRORS:
				fprintf(fperror, "Error: %s\n", err_msg);
				break;
		}

	}
}

//called before each scan
void initScanner() {
	bicmd = 0;	//initially false that built-in is read
	bioutf = 0;	//false that output redirected
	bistr = NULL;	//null string
	bistr2 = NULL;	//null string
	err_msg = NULL;
	//reset command table:
	int i;
	for(i = 0; i < MAXPIPES; i++) {
		comtab[i].comname = NULL;
		comtab[i].remote = 0;
		comtab[i].infd = 0;
		comtab[i].outfd = 1;
		comtab[i].nargs = 0;
		comtab[i].atptr = NULL;
	}
}

int getCommand() {
	initScanner();
	if(fgets(inStr, 80, fpin) == NULL)	//end of file
		return DONE;
	char* p = inStr;
	int length = 0;
	while( (p != NULL) && (*p != ' ') && (*p != '\t') && (*p != '\n')) {
		p++;
		length++;
	}
	
	//printf("%s\n", theCommand);
	yy_scan_string(inStr);	//pass the expanded string to lex
	int y = yyparse();

	//expand aliases:
	char* temp;
	if(comtab[0].comname != NULL) {
		temp = findAlias(comtab[0].comname);
		//check if there's an alias for the first word
		while(temp != NULL) {
			comtab[0].comname = temp;	//replace with its alias
			temp = findAlias(comtab[0].comname);	//and check again
		}
	}

	if(y == 1) {	//grammar error in yacc
		if(err_msg == NULL)
			err_msg = "grammar error";
		return ERRORS;
	}else if(y == 2) { //memory exhaustion error
		err_msg = "memory exhaustion error";
		return ERRORS;
	}else	//grammar parsing worked
		return OK;

}

//return 0 if no error, negative number if error
int processCommand() {
	if(bicmd == 0)	//if nonbuiltin, execute it
		return execute_it();
	//otherwise do the builtin
	return do_it();
}

//function to perform built-in commands - return 0 if no error, -1 if error
int do_it() {
	FILE* fp;
	int ret;	//return value
	switch(bicmd) {
		case GOODBYE:
			printf("Exiting shell now\n");
			exit(0);
		case SETENVIRON:
			setenv(bistr, bistr2, 1);
			break;
		case UNSETENVIRON:
			unsetenv(bistr);
			break;
		case PRINTENVIRON:
			if(bioutf) {	//if true that there was output redirection
				fp = fopen(bistr, "a");
				if(fp == NULL) {
					err_msg = strerror(errno);
					return -1;
				}
			}
			int i = 0;
			while(environ[i]) {
				if(bioutf) {
					fputs(environ[i++], fp);
					fputs("\n", fp);
				}
				else
					puts(environ[i++]);
			}
			if(bioutf)
				fclose(fp);
			break;
		case PRINTALIAS:
			fp = stdout;
			if(bioutf) {
				fp = fopen(bistr, "a");
				if(fp == NULL) {
					err_msg = strerror(errno);
					return -1;
				}
			}
			printAliases(fp);
			if(bioutf)
				fclose(fp);
			break;
		case SETALIAS:
			ret = insertAlias(bistr, bistr2);
			if(ret == -1)	//there was an error
				return INSERTERROR;
			break;
		case UNSETALIAS:
			removeAlias(bistr);
			break;
		case CHANGEDIR:
			if(bistr)
				chdir(bistr);
			else
				chdir(getenv("HOME"));
			break;
		default:
			break;
	}
	return 0;
}

//function to execute non built-in commands - return 0 if no error, negative if error
int execute_it() {
	char* theCommand = comtab[0].comname;
	char* bin = "/bin/";	//start of command name needs to be /bin/
	char* temp = NULL;
	//allocate space for the command name:
	if( (temp = (char *) malloc(sizeof(char) * (strlen(bin) + strlen(theCommand)) + 1)) == NULL) {
		err_msg = "failed to allocate memory";
		return MEMERROR;
	}
	strcat(temp, bin);	//add "/bin/" to command name
	strcat(temp, theCommand);	//concatenate the command
	comtab[0].comname = temp;
	char* args[2];
	args[0] = theCommand;
	args[1] = (char*) 0;
	pid_t pid = fork();
	int* status;	//place where wait will store status
	if(pid == 0) {
		int execReturn = execve(comtab[0].comname, args, environ);
		if(execReturn == -1) {
			printf("failed to execute command\n");
			return EXECERROR;
		}
		exit(0);
	}
	else if(pid < 0) {
		printf("process failed to fork\n");
		return FORKERROR;
	}
	else {
		//parent process
		wait(status);
	}
	free(temp);

	return 0;
}

void initShell() {
	fpin = stdin;		//set default file pointer for input to stdin
	fpout = stdout;		//default fp for output is stdout
	fperror = stderr;	//default fp for error is sderr
	prompt_string = ">> ";	
	headAliasNode.name = NULL;	//initialize alias list to empty
	headAliasNode.word = NULL;
	headAliasNode.next = NULL;
	aliasHead = &headAliasNode;
	bicmd = 0;		//initially false that a built-in command was read
}

void printPrompt() {
	printf("%s", prompt_string);
}

//function to add alias with specified name to head of linked list for aliases
int insertAlias(char* theName, char* theWord) {
	//don't allow alias to be created for itself
	if(strcmp(theName, theWord) == 0) {
		err_msg = "cannot create alias for itself";
		return -1;
	}
	//check if alias already exists for theName
	if(aliasExists(theName)) {
		err_msg = "alias already exists for that word";
		return -1;
	}
	//check for circular aliasing
	char* tempName = theWord;
	char* tempWord = findAlias(tempName);
	while(tempWord != NULL) {
		if(strcmp(tempWord, theName) == 0) {
			err_msg = "cannot add alias - will result in infinite alias expansion";
			return -1;
		}
		tempName = tempWord;
		tempWord = findAlias(tempName);
	}
	aliasNode* newNode = (aliasNode*)malloc(sizeof(aliasNode));
	if(newNode == NULL) {
		//set error message to indicate memory issue
		return -1;
	}
	newNode -> name = theName;
	newNode -> word = theWord;
	newNode -> next = aliasHead -> next;
	aliasHead -> next = newNode;
	return 0;
}

//function to remove alias with given name - does nothing if doesn't exist
void removeAlias(char* theName) {
	aliasNode* curr, *prev;
	curr = aliasHead -> next;
	prev = aliasHead;
	while(curr != NULL && (strcmp(curr->name, theName) != 0)) {
		prev = curr;
		curr = curr->next;
	}
	if(curr == NULL) {
		printf("Alias '%s' not found\n", theName);
		return;
	}
	prev->next = curr->next;
	free(curr);
}

//function to print all of the aliases currently defined
void printAliases(FILE* fp) {
	aliasNode* temp = aliasHead -> next;
	if(temp == NULL) {
		fprintf(fp, "No aliases\n");
		return;
	}
	while(temp != NULL) {
		fprintf(fp, "%s:%s\n", temp->name, temp->word);
		temp = temp->next;
	}
}

//function to check if alias already exists for given name
int aliasExists(char* theName) {
	aliasNode* curr = aliasHead -> next;
	while(curr != NULL) {
		if(strcmp(curr->name, theName) == 0) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}

//function find the alias for a given name - returns null if none exists
char* findAlias(char* theName) {
	char* ret = NULL;
	aliasNode* curr = aliasHead -> next;
	while(curr != NULL) {
		if(strcmp(curr->name, theName) == 0) {
			ret = curr->word;
			return ret;
		}
		curr = curr->next;
	}
	return NULL;
}