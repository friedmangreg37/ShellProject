YFLAGS        = -d

PROGRAM       = parser

OBJS          = y.tab.o lex.yy.o 

SRCS          = y.tab.c lex.yy.c

CC            = gcc 

all:		$(PROGRAM)

.c.o:		$(SRCS)
			$(CC) -c $*.c -o $@ -O

y.tab.c:	parser.y
			yacc $(YFLAGS) parser.y

lex.yy.c:	lex.l 
			lex lex.l

parser:		$(OBJS)
			$(CC) $(OBJS)  -o $@ -ll -lm

clean:;		rm -f $(OBJS) core *~ \#* *.o $(PROGRAM) \
			y.* lex.yy.* calcparse.tab.*