Shell Project
==========================

Students
--------

Name             | Gatorlink
-----------------|----------
Greg Friedman    | friedmangreg37@ufl.edu
Krystal Mejia    | krystalmejia24@ufl.edu



Things to be done
------------------

1. sometimes gets some weird errors because tokenizer cuts off at 'r' for some reason - just make clean and try again
	* hasn't shown up again

2. redirect output

3. setting err_msg - some in comments (insertAlias)

4. quotations and metacharacters
	* created a token.c file to read quotes and metacharacters. will push once i implement into shell.c

5. non built-in commands - should be able to just use exec sys calls  - greg has figured something out

6. piping - only for non built-ins so figure those out first

7. & - running in background

8. redirecting error

9. 2.3 - other commands section to check what needs to be done for redirection, piping, etc

10. segmentation fault when running printenv 
	* krystals working on it
		
11. ERROR: pwd command is sometimes printing the path next to the >> cursor and reading the following command without >> cursor
	(try typing pwd twice in a row to see error)

12. look for \(metacharacter)
 
13. Environment variables - read from ${ to next} and substitute value for variable
