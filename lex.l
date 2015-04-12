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
">>"			return GGT;
"&"				return AMP;
"(" 			return LPAREN;
")"				return RPAREN;
"|"				return BAR;
"/"				return FSLASH;
"2>&1"			return ERRORTOOUT;
"2>"[a-zA-z0-9\/.-]+	{
							char* temp = strdup(yytext);
							yylval.sval = &temp[2];
							yylval.sval[strlen(temp)-2] = '\0';
							return ERRORREDIR;
						}
setenv			return SETENV;
printenv		return PRINTENV;
unsetenv		return UNSETENV;
alias			return ALIAS;
unalias			return UNALIAS;
cd				return CD;
bye				return BYE;
[a-zA-z0-9\/.-]+	yylval.sval = strdup(yytext); return WORD;
\".*\"			{
					char* temp = strdup(yytext);
					yylval.sval = &temp[1];
					yylval.sval[strlen(temp)-2] = '\0';
					return WORD;
				}
\$\{.*\}		{ 
					char* temp = strdup(yytext);
					yylval.sval = &temp[2];
					yylval.sval[strlen(temp)-3] = '\0';
					if(getenv(yylval.sval))
						yylval.sval = getenv(yylval.sval);
					return WORD;
				}
\*.+			{yylval.sval = strdup(yytext); return MATCH;}
[?]*			{yylval.sval = strdup(yytext); return QUEST;}
[ \t]			/* ignore whitespace */
[\n]			{ return -1; }
%%