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
FILE* fpin;
FILE* fpout;
FILE* fperror;
int inRedirect = 0;
char inFilename[80];
int cmd;
char inStr[80];
char aliasStr[160];

void initShell(), printPrompt(), removeAlias(char*), printAliases(), processCommand(), initScanner();
void do_it(), execute_it();
int insertAlias(char*, char*), getCommand();
char* findAlias(char*);

int main(int argc, char** argv) {
	initShell();
	if( (argc > 1) && (strcmp(argv[1], "<")) ) {
		inRedirect = 1;
		strcpy(inFilename, argv[2]);
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
				if(inRedirect)
					fclose(fpin);
				printf("Exiting shell now\n");
				exit(0);
			case OK:
				processCommand();
				break;
			default:
				printf("something else\n");
		}

	}
}

void initScanner() {
	bicmd = 0;
	bioutf = 0;
	bistr = NULL;
}

int getCommand() {
	initScanner();
	const char s[2] = " \t";
	char* token, *p;
	int nTokens;
	aliasStr[0] = '\0';
	if(fgets(inStr, 80, fpin) == NULL)	//end of file
		return DONE;
	//printf("%s\n", inStr);
	//char* temp = strdup(inStr);
	token = strtok(inStr, s);
	//printf("%s\n", inStr);
	nTokens = 1;
	while(token != NULL) {
		p = token;
		//deal with aliases
		strcat(aliasStr, p);
		strcat(aliasStr, " ");
		token = strtok(NULL, s);
	}
	yy_scan_string(aliasStr);
	if(yyparse())
		printf("there was an error\n");
	else
		return OK;
}

void processCommand() {
	if(bicmd)
		do_it();
	else
		execute_it();
}

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

void execute_it() {

}

void initShell() {
	fpin = stdin;
	fpout = stdout;
	fperror = stderr;
	prompt_string = ">> ";
	headAliasNode.name = NULL;
	headAliasNode.word = NULL;
	headAliasNode.next = NULL;
	aliasHead = &headAliasNode;
	bicmd = 0;
}

void printPrompt() {
	printf("%s", prompt_string);
}

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