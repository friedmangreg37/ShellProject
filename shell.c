#include "shell.h"

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
char inStr[80];		//input string to read from command line
char aliasStr[160];		//expanded input string after alias checking
char* err_msg;
COMMAND comtab[20];
int currcmd;
int currarg;
int ncmds;
int inredir;
int outredir;
int errredir;
int append;
int background;
int match;

void initShell(), printPrompt(), removeAlias(char*), printAliases(FILE*), initScanner();
int processCommand(), do_it(), execute_it();
int insertAlias(char*, char*), getCommand();
char* findAlias(char*), *getCommandName(char*);

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
			case OK:	//recognized pattern in yacc
				ret = processCommand();
				if(ret < 0) {		//if there was an error display it
					fprintf(fperror, "Error: %s\n", err_msg);
					if(ret == EXECERROR)
						exit(0);	//need to exit from forked process
				}
				break;
			case ERRORS:	//error in yyparse
				fprintf(fperror, "Error: %s\n", err_msg);
				break;
		}
		if(errredir)		//if error was redirected close the file
			fclose(fperror);
	}
}

void initShell() {
	prompt_string = ">> ";	
	headAliasNode.name = NULL;	//initialize alias list to empty
	headAliasNode.word = NULL;
	headAliasNode.next = NULL;
	aliasHead = &headAliasNode;
}

//called before each scan
void initScanner() {
	fpin = stdin;		//set default file pointer for input to stdin
	fpout = stdout;		//default fp for output is stdout
	fperror = stderr;	//default fp for error is sderr
	bicmd = 0;	//initially false that built-in is read
	bioutf = 0;	//false that output redirected
	bistr = NULL;	//null string
	bistr2 = NULL;
	err_msg = NULL;		//no error message initally
	currcmd = 0;		//reset current command
	currarg = 0;		//reset current argument
	ncmds = 0;			//reset number of commands
	inredir = 0;		//input not redirected by default
	outredir = 0;		//output not redirected
	errredir = 0;		//error not redirected
	append = 0;			//don't append redirection by default
	background = 0;		//background initially false
	match = 0; 			//wildcard matching initially false
	//reset command table:
	int i;
	for(i = 0; i < MAXPIPES; i++) {
		comtab[i].comname = NULL;	//no command name
		comtab[i].infd = 0;		//stdin by default
		comtab[i].infn = NULL;	//no input file yet
		comtab[i].outfd = 1;	//stdout by default
		comtab[i].outfn = NULL;	//no output file yet
		comtab[i].nargs = 0;	//number of arguments 0 by default
		comtab[i].atptr = NULL;	//no arguments yet
	}
}

//function to get the command(s) to be executed from yacc
//will expand aliases then pass to yacc
//returns OK if no errors from yacc or error value if there are errors
int getCommand() {
	initScanner();
	//read in line of input:
	if(fgets(inStr, 80, fpin) == NULL)	//end of file
		return DONE;
	char* p = inStr;
	int length = 0;
	//get first word
	while( (p != NULL) && (*p != ' ') && (*p != '\t') && (*p != '\n')) {
		length++;
		p++;
	}
	char first[length+1];
	int i;
	for(i = 0; i < length; i++)
		first[i] = inStr[i];
	first[length] = '\0';

	//expand aliases:
	aliasStr[0] = '\0';
	char* temp;
	char* newFirst = first;		//new string after alias replacement
	if(newFirst != NULL) {
		temp = findAlias(newFirst);
		//check if there's an alias for the first word
		while(temp != NULL) {
			newFirst = temp;	//replace with its alias
			temp = findAlias(newFirst);	//and check again
		}
	}
	//add expanded first word to the string to pass to yacc
	strcat(aliasStr, newFirst);

	//update input string with alias expansion for piped commands:
	char* pipe = strchr(p, '|');	//search the rest of the string for pipes
	while(pipe != NULL) {
		length = pipe - p + 2;
		//make sure surrounded by spaces:
		if(*(p+length-1) == ' ' && *(p+length-3) == ' ') {
			char difference[length+1];
			for(i = 0; i < length; ++i)
				difference[i] = p[i];
			difference[length] = '\0';
			strcat(aliasStr, difference);
			pipe += 2;	//move to beginning of next command
			p = pipe;
			length = 0;
			//get command name
			while( (p != NULL) && (*p != ' ') && (*p != '\t') && (*p != '\n')) {
				length++;
				p++;
			}
			char next[length+1];
			for(i = 0; i < length; ++i)
				next[i] = pipe[i];
			next[length] = '\0';

			//replace with alias(es), if any
			char* nextTemp;
			char* newNext = next;
			if(newNext != NULL) {
				nextTemp = findAlias(newNext);
				while(nextTemp != NULL) {
					newNext = nextTemp;
					nextTemp = findAlias(newNext);
				}
			}
			strcat(aliasStr, newNext);	//add it to the string
			pipe = strchr(p, '|');		//and look for another pipe
		}else {		//if not surrounded by spaces, move on past it without checking for aliases
			char difference[length+1];
			for(i = 0; i < length; ++i)
				difference[i] = p[i];
			difference[length] = '\0';
			strcat(aliasStr, difference);
			p = pipe+2;
			pipe = strchr(p, '|');
		}
	}

	strcat(aliasStr, p);	//append the rest of the string
	
	yy_scan_string(aliasStr);	//pass the expanded string to lex

	int y = yyparse();

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
				if(append)		//append to end of file if >> 
					fp = fopen(bistr, "a");
				else		//don't append if >
					fp = fopen(bistr, "w");
				if(fp == NULL) {	//fail to open file
					err_msg = strerror(errno);
					return OTHERERROR;
				}
			}
			int i = 0;
			while(environ[i]) {
				if(bioutf) {	//if output redirected, output to specified file
					fputs(environ[i++], fp);
					fputs("\n", fp);
				}
				else	//otherwise output to stdout
					puts(environ[i++]);
			}
			if(bioutf)	
				fclose(fp);		//close file if output was redirected
			break;
		case PRINTALIAS:
			fp = stdout;
			if(bioutf) {	//output is redirected
				if(append)	
					fp = fopen(bistr, "a");
				else
					fp = fopen(bistr, "w");
				if(fp == NULL) {
					err_msg = strerror(errno);
					return OTHERERROR;
				}
			}
			printAliases(fp);
			if(bioutf)
				fclose(fp);
			break;
		case SETALIAS:
			ret = insertAlias(bistr, bistr2);
			if(ret == -1)	//there was an error
				return OTHERERROR;
			break;
		case UNSETALIAS:
			removeAlias(bistr);
			break;
		case CHANGEDIR:
			if(bistr)	//if path was specified, change to it
				ret = chdir(bistr);
			else	//otherwise go home (only "cd" entered)
				ret = chdir(getenv("HOME"));
			if(ret == -1) {	//error
				err_msg = strerror(errno);
				return OTHERERROR;
			}
			break;
		default:
			break;
	}
	return 0;
}

//function to execute non built-in commands - return 0 if no error, negative if error
int execute_it() {
	int i;
	char* theCommand;
	int* status;
	int theinfd = STDIN_FILENO;		//save the first command's infd
	int theoutfd = STDOUT_FILENO;	//save the last command's outfd
	pid_t pid = fork();
	if(pid == 0) {
		if(inredir) {		//input redirected for first command
			int fd1 = open(comtab[0].infn, O_RDONLY, 0);	//try to open the file
			if(fd1 == -1) {	//if fail, return error
				err_msg = strerror(errno);
				return EXECERROR;
			}
			if(dup2(fd1, theinfd) == -1) {		//replace stdin with new file
				err_msg = strerror(errno);
				return EXECERROR;
			}
			if(close(fd1) == -1) {		//close the extra
				err_msg = strerror(errno);
				return EXECERROR;
			}
		}
		if(outredir) {		//output redirected for last command
			int fd1;
			if(append)
				fd1 = open(comtab[ncmds-1].outfn, O_WRONLY | O_APPEND | O_CREAT);
			else
				fd1 = open(comtab[ncmds-1].outfn, O_WRONLY | O_CREAT | O_TRUNC);
			if(fd1 == -1) {
				err_msg = strerror(errno);
				return EXECERROR;
			}
			if(dup2(fd1, theoutfd) == -1) {	//replace stdout with new file
				err_msg = strerror(errno);
				return EXECERROR;
			}
			if(close(fd1) == -1) {		//close the extra
				err_msg = strerror(errno);
				return EXECERROR;
			}
		}
		int in, fd[2];
		in = theinfd;
		for(i = 0; i < ncmds-1; ++i) {
			pipe(fd);
			pid_t execpid;
			if(execpid = fork() == 0) {
				if(in != theinfd) {		//if not first command
					if(dup2(in, theinfd) == -1) {		//redirect input to previous command
						err_msg = strerror(errno);
						return EXECERROR;
					}
					if(close(in) == -1) {	//close the extra
						err_msg = strerror(errno);
						return EXECERROR;
					}
				}
				if(fd[1] != theoutfd) {		//if not last command
					if(dup2(fd[1], theoutfd) == -1) {	//redirect output to next command
						err_msg = strerror(errno);
						return EXECERROR;
					}
					if(close(fd[1]) == -1) {
						err_msg = strerror(errno);
						return EXECERROR;
					}
				}
				theCommand = getCommandName(comtab[i].comname);		//find command name to pass to exec
				if(theCommand == NULL)	//couldn't find the command
					return EXECERROR;
				int execReturn = execve(theCommand, comtab[i].atptr->args, environ);
				if(execReturn == -1) {
					err_msg = "failed to execute command";
					return EXECERROR;
				}
			}
			if(close(fd[1]) == -1) {
				err_msg = strerror(errno);
				return EXECERROR;
			}
			in = fd[0];		//set in to current command before moving to next one
		}
		if(in != theinfd) 
			if(dup2(in, theinfd) == -1) {
				err_msg = strerror(errno);
				return EXECERROR;
			}
		i = ncmds-1;	//execute last command
		theCommand = getCommandName(comtab[i].comname);
		if(theCommand == NULL)
			return EXECERROR;
		int execReturn = execve(theCommand, comtab[i].atptr->args, environ);
		if(execReturn == -1) {
			err_msg = "failed to execute command";
			return EXECERROR;
		}
		exit(0);
	}else if(pid < 0) {
		err_msg = "process failed to fork";
		return OTHERERROR;
	}else {
		wait(status);
	}

	//no errors if we're here so return 0
	return 0;
}

//function to find command name to use with execve
//will return NULL if command does not exist
char* getCommandName(char* theCommand) {
	if( strchr(theCommand, '/') != NULL)	//if there's a '/' don't change anything
		return theCommand;

	char* str = getenv("PATH");
  	const char s[2] = ":";	//search for ':' which separate different paths
   	char *token;
   	// get the first token
   	token = strtok(str, s);
   	// walk through other tokens
   	while( token != NULL ) 
   	{
   		char* temp = NULL;
		//allocate space for the command name:
		if( (temp = (char *) malloc(sizeof(char) * (strlen(token) + strlen(theCommand)) + 1)) == NULL) {
			err_msg = "failed to allocate memory";
			return NULL;
		}
		strcat(temp, token);	//add path to command name
		strcat(temp, "/");
		strcat(temp, theCommand);	//concatenate the command

      	if(access(temp, X_OK) == 0)		//if command exists in this path, return the path and command name
      		return temp;
    
      	token = strtok(NULL, s);
   	}
   	//didn't find the command so tell calling function the command
   	err_msg = "command not found";
   	return NULL;
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
	aliasNode* newNode = Allocate(aliasNode);
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