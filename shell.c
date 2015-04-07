#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "shell.h"
#include <errno.h>
#include <string.h>

char* prompt_string;
int currcmd;
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
char aliasStr[160];
char* err_msg;

void initShell(), printPrompt(), removeAlias(char*), printAliases(), processCommand(), initScanner();
void do_it(), execute_it();
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
		switch(cmd = getCommand()) {
			case DONE:
				if(inRedirect)	//if input was redirected, close file
					fclose(fpin);
				printf("Exiting shell now\n");
				exit(0);
			case OK:
				processCommand();
				break;
			case ERRORS:
				fprintf(fperror, "Error: %s\n", err_msg);
				break;
		}

	}
}

void initScanner() {
	bicmd = 0;	//initially false that built-in is read
	bioutf = 0;	//false that output redirected
	bistr = NULL;	//null string
	bistr2 = NULL;	//null string
}

int getCommand() {
	initScanner();
	const char s[2] = " \t";	//delineator for strtok
	char* token, *p;
	int nTokens;
	aliasStr[0] = '\0';		//string after aliases expanded - initally empty
	if(fgets(inStr, 80, fpin) == NULL)	//end of file
		return DONE;
	token = strtok(inStr, s);
	nTokens = 1;
	char* temp;
	//expand aliases:
	if(token != NULL) {
		temp = findAlias(token);
		//check if there's an alias for the first word
		while(temp != NULL) {
			token = temp;	//replace with its alias
			temp = findAlias(token);	//and check again
		}
	}
	while(token != NULL) {
		p = token;
		strcat(aliasStr, p);	//append to string
		strcat(aliasStr, " ");	//add space between tokens
		token = strtok(NULL, s);	//get next
	}
	yy_scan_string(aliasStr);	//pass the expanded string to lex
	int y = yyparse();
	if(y == 1) {
		char* args[] = {"ls", (char*) 0};
		//execve("/bin/ls", args, environ);
		//printf("%d\n", ret);
		// err_msg = "syntax error";
		// return ERRORS;
		pid_t pid = fork();
		int* status;	//place where wait will store status
		if(pid == 0) {
			int execReturn = execve("/bin/ls", args, environ);
			if(execReturn == -1) {
				err_msg = "failed to execute command";
				return ERRORS;
			}
			exit(0);
		}
		else if(pid < 0) {
			err_msg = "process failed to fork";
			return ERRORS;
		}
		else {
			printf("parent process\n");
			wait(status);
		}
		return OK;
	}else if(y == 2) { //memory exhaustion error
		err_msg = "memory exhaustion error";
		return ERRORS;
	}
	else
		return OK;
}

void processCommand() {
	if(bicmd)
		do_it();
	else
		execute_it();
}

//function to perform built-in commands
void do_it() {
	FILE* fp;
	switch(bicmd) {
		//case alias - if adding alias, return error if -1 returned
		case GOODBYE:
			printf("Exiting shell now\n");
			exit(0);
		case PRINTENVIRON:
			if(bioutf) {	//if true that there was output redirection
				fp = fopen(bistr, "a");
				if(fp == NULL)
					printf("%s", strerror(errno));
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
		default:
			break;
	}
}

//function to execute non built-in commands
void execute_it() {

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
		//error message for this
		printf("Cannot create alias for itself\n");
		return -1;
	}
	//check if alias already exists for theName
	if(aliasExists(theName)) {
		//set error message for the following:
		printf("Alias already exists for %s\n", theName);
		return -1;
	}
	//check for circular aliasing
	char* tempName = theWord;
	char* tempWord = findAlias(tempName);
	while(tempWord != NULL) {
		if(strcmp(tempWord, theName) == 0) {
			//error message for circular aliasing
			printf("Error: cannot add alias - will result in infinite alias expansion\n");
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
void printAliases() {
	aliasNode* temp = aliasHead -> next;
	if(temp == NULL) {
		printf("No aliases\n");
		return;
	}
	while(temp != NULL) {
		printf("%s:%s\n", temp->name, temp->word);
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