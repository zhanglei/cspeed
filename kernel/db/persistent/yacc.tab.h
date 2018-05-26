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
     SELECT = 258,
     FROM = 259,
     JOIN = 260,
     WHERE = 261,
     IN = 262,
     ON = 263,
     GROUPBY = 264,
     HAVING = 265,
     ORDERBY = 266,
     NAME = 267,
     COMMA = 268,
     EQUAL = 269,
     AS = 270,
     ORDER_RULE = 271,
     AND = 272,
     LIMIT = 273,
     LPARATHES = 274,
     RPARATHES = 275,
     VALUES = 276,
     UPDATE = 277,
     SET = 278,
     DELETE = 279,
     INSERT = 280
   };
#endif
/* Tokens.  */
#define SELECT 258
#define FROM 259
#define JOIN 260
#define WHERE 261
#define IN 262
#define ON 263
#define GROUPBY 264
#define HAVING 265
#define ORDERBY 266
#define NAME 267
#define COMMA 268
#define EQUAL 269
#define AS 270
#define ORDER_RULE 271
#define AND 272
#define LIMIT 273
#define LPARATHES 274
#define RPARATHES 275
#define VALUES 276
#define UPDATE 277
#define SET 278
#define DELETE 279
#define INSERT 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 79 "yacc.y"
{
	char *val;
	int flag;
}
/* Line 1529 of yacc.c.  */
#line 104 "yacc.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

