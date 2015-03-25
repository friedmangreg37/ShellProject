%{
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "y.tab.h"

%}

%%
setenv		return SETENV;
printenv	return PRINTENV;
unsetenv	return UNSETENV;
alias		return ALIAS;
unalias		return UNALIAS;
cd			return CD;
bye			return BYE;
[a-zA-z]+	yylval.sval = strdup(yytext); return WORD;
%%