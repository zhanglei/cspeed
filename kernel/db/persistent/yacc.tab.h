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
     ON = 262,
     GROUPBY = 263,
     HAVING = 264,
     ORDERBY = 265,
     NAME = 266,
     COMMA = 267,
     EQUAL = 268,
     AS = 269,
     ORDER_RULE = 270,
     AND = 271,
     LIMIT = 272,
     LPARATHES = 273,
     RPARATHES = 274,
     VALUES = 275,
     UPDATE = 276,
     SET = 277,
     DELETE = 278,
     INSERT = 279
   };
#endif
/* Tokens.  */
#define SELECT 258
#define FROM 259
#define JOIN 260
#define WHERE 261
#define ON 262
#define GROUPBY 263
#define HAVING 264
#define ORDERBY 265
#define NAME 266
#define COMMA 267
#define EQUAL 268
#define AS 269
#define ORDER_RULE 270
#define AND 271
#define LIMIT 272
#define LPARATHES 273
#define RPARATHES 274
#define VALUES 275
#define UPDATE 276
#define SET 277
#define DELETE 278
#define INSERT 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 25 "yacc.y"
{
	char *val;
	int flag;
}
/* Line 1529 of yacc.c.  */
#line 102 "yacc.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

