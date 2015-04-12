Shell Project
==========================

Students
--------

Name             | Gatorlink
-----------------|----------
Greg Friedman    | friedmangreg37@ufl.edu
Krystal Mejia    | krystalmejia24@ufl.edu



Features we have not implemented:
---------------------------------
1. metacharacters - recognize them but don't do anything with them
	
2. & - running in background - recognizes it but doesn't do anything with it

3. wildcard matching

4. possible problem: printenv works on Greg's computer but seems to segmentation fault on any other

5. tilde expansion (extra credit)

6. file name completion (extra credit)



Features we have implemented:
-----------------------------
Builtin Commands:
	- setenv
	- unsetenv
	- printenv (as mentioned above, works on Greg's computer but no others)
	- cd
	- alias
	- unalias
	- bye

Other Commands:
	- executes other commands and their arguments with execve
	- Note: if a proper command is entered but invalid arguments are given (anytime it calls execve but this doesn't work), any subsequent nonbuiltin commands will make the shell be stuck, and Control-c must be used to end it
	- input redirection
	- output redirection (normal and appending)
	- error redirection (to file and to stdout)
	- piping
	- aliases for any command name in pipeline
	- path name searching

Environment variable expansion

