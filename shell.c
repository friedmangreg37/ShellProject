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

void initShell(), printPrompt(), removeAlias(char*), printAliases(), processCommand(), initScanner();
void do_it(), execute_it();
int insertAlias(char*, char*), getCommand();

int main() {
	initShell();
	int cmd;
	while(1) {
		printPrompt();
		switch(cmd = getCommand()) {
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
	aliasNode* newNode = (aliasNode*)malloc(sizeof(aliasNode));
	if(newNode == NULL)
		return -1;
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