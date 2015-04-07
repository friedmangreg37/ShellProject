# ShellProject
Greg Friedman		friedmangreg37@ufl.edu
Krystal Mejia		krystalmejia24@ufl.edu


Things to be done:

sometimes gets some weird errors because tokenizer cuts off at 'r' for some reason - just make clean and try again

move to shell.c:
	setenv
	unsetenv
	alias
	alias word word
	unalias
	cd
	cd word
	pwd

redirect output:
	alias
	pwd

setting err_msg - some in comments (insertAlias)

quotations

non built-in commands - should be able to just use exec sys calls 

piping - only for non built-ins so figure those out first

& - running in background

redirecting error

look at 2.3 - other commands section to check what needs to be done for redirection, piping, etc

scan for \(metacharacter)

don't alias for built-ins anyways so don't worry about this