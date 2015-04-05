%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "y.tab.h"

%}

%%
"<"				return LT;
">"				return GT;
"&"				return AMP;
"(" 			return LPAREN;
")"				return RPAREN;
"|"				return BAR;
"."				return DOT;
setenv			return SETENV;
printenv		return PRINTENV;
unsetenv		return UNSETENV;
alias			return ALIAS;
unalias			return UNALIAS;
cd				return CD;
pwd				return PWD;
bye				return BYE;
[a-zA-z0-9\/.-]+	yylval.sval = strdup(yytext); return WORD;
\"				return QUOTE;
[ \t]			/* ignore whitespace */
[\n]			{ return -1; }
%%