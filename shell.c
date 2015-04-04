#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "shell.h"

char* prompt_string;
int currcmd;
aliasNode headAliasNode;
aliasNode* aliasHead;

void initShell(), printPrompt(), removeAlias(char*), printAliases();
int insertAlias(char*, char*);

int main() {
	initShell();
	while(1) {
		printPrompt();
		yyparse();
	}
}

void initShell() {
	prompt_string = ">> ";
	headAliasNode.name = NULL;
	headAliasNode.word = NULL;
	headAliasNode.next = NULL;
	aliasHead = &headAliasNode;
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
	while(temp != NULL) {
		printf("%s:%s\n", temp->name, temp->word);
		temp = temp->next;
	}
}