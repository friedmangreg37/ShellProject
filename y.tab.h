/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LT = 258,
     GT = 259,
     GGT = 260,
     AMP = 261,
     LPAREN = 262,
     RPAREN = 263,
     BAR = 264,
     ERRORTOOUT = 265,
     TILDE = 266,
     SETENV = 267,
     UNSETENV = 268,
     PRINTENV = 269,
     CD = 270,
     BYE = 271,
     ALIAS = 272,
     UNALIAS = 273,
     WORD = 274,
     MATCH = 275,
     QUEST = 276,
     ERRORREDIR = 277
   };
#endif
/* Tokens.  */
#define LT 258
#define GT 259
#define GGT 260
#define AMP 261
#define LPAREN 262
#define RPAREN 263
#define BAR 264
#define ERRORTOOUT 265
#define TILDE 266
#define SETENV 267
#define UNSETENV 268
#define PRINTENV 269
#define CD 270
#define BYE 271
#define ALIAS 272
#define UNALIAS 273
#define WORD 274
#define MATCH 275
#define QUEST 276
#define ERRORREDIR 277




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 21 "parser.y"
{
	int i;
	char* sval;
}
/* Line 1529 of yacc.c.  */
#line 98 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

