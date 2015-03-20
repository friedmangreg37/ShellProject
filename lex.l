%{
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "y.tab.h"

#ifndef	YYSTYPE
#define YYSTYPE char*
#endif
%}

%%
cd			return CD;
bye			return BYE;
[a-zA-z]+	return WORD;
%%