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
----------------
1. setenv

2. unsetenv

3. printenv (as mentioned above, works on Greg's computer but no others)

4. cd

5. alias

6. unalias

7. bye

Other Commands:
--------------

1. executes other commands and their arguments with execve

2. input redirection

3. output redirection (normal and appending)

4. error redirection (to file and to stdout)

5. piping

6. aliases for any command name in pipeline

7. path name searching

Environment variable expansion
------------------------------
compile with "make" and run with "./shell" - will take input from file if run with "./shell < file.txt"
