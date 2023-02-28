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
     NUMBER = 258,
     STRING = 259,
     NAME = 260,
     FUNC = 261,
     SYSTEM = 262,
     FUNCDEV = 263,
     INTERVAL = 264,
     INSERT = 265,
     ARROW = 266,
     DATA = 267,
     ADD = 268,
     GET = 269,
     SET = 270,
     REMOVE = 271,
     SLEEP = 272,
     ARRAY = 273,
     INTEGER = 274,
     CHAR = 275,
     DEVICE = 276,
     EOL = 277,
     TERM = 278,
     IF = 279,
     THEN = 280,
     ELSE = 281,
     WHILE = 282,
     DO = 283,
     CMD = 284,
     HELP = 285,
     RET = 286,
     REPEAT = 287
   };
#endif
/* Tokens.  */
#define NUMBER 258
#define STRING 259
#define NAME 260
#define FUNC 261
#define SYSTEM 262
#define FUNCDEV 263
#define INTERVAL 264
#define INSERT 265
#define ARROW 266
#define DATA 267
#define ADD 268
#define GET 269
#define SET 270
#define REMOVE 271
#define SLEEP 272
#define ARRAY 273
#define INTEGER 274
#define CHAR 275
#define DEVICE 276
#define EOL 277
#define TERM 278
#define IF 279
#define THEN 280
#define ELSE 281
#define WHILE 282
#define DO 283
#define CMD 284
#define HELP 285
#define RET 286
#define REPEAT 287




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 12 "parser.y"
{
  struct ast *a;
  double d;
  char * str;
  struct symbol *s;       /* Specifica quale simbolo si sta passando*/
  struct argsList *sl;
  int  func;              /* Specifica quale funzione built-in deve essere eseguita */
}
/* Line 1529 of yacc.c.  */
#line 122 "parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

