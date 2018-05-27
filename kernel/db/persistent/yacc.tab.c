/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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
     RELATION = 269,
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
#define RELATION 269
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




/* Copy the first part of user declarations.  */
#line 1 "yacc.y"

	/* Some header for parser to use. */
	#include <stdio.h>
	#include <string.h>
    #include <stdlib.h>
    #include "yacc.h"

	/* Some internal functions needed by bison & flex */
	extern int yylex();
	extern FILE* yyin;
	extern int yyerror(const char *s);

	/* The api functions to user to invoke from outside. */
	char *fake_table_name = "?!!?";
    char *query_table_name = "";
    char *sharding_key = "";
    static size_t insert_key_index = 0;
    static size_t insert_key_stop = 0;
    static size_t insert_fields_num = 0;

    /* All things need for user-input */
	typedef struct yy_buffer_state * YY_BUFFER_STATE;
	extern int yyparse();
	extern YY_BUFFER_STATE yy_scan_string(char * str);
	extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
  
  /* SQL RESULT */
  SQL_PARSER_RESULT sql_parsing_result;
	/* The api to the other appliaction. */
  SQL_PARSER_RESULT *parse_sql(char *, char *);
	/**
	 * find the string is exists or not.
	 * The following function's resource cost much.
	 * every body need to know to avoid the 
	 */
	int stringexists(char *str, char *pattern)
	{
	    size_t j = 0, start = 0;
	    for (; start < strlen(str); start++) {
	        if ( *(str + start) == *pattern ) {
	            size_t pattern_len = strlen(pattern);
	            for (; j <= pattern_len; j++) {
	                if (j == pattern_len ) return start;
	                if ( *(str + start + j) == *(pattern + j) ) continue;
	                else return -1;
	            }
	        }
	    }
	    return -1;
	}

    /**
     * Replace the tablename with query_table_name or the raw table_name
     * Each table in the RPC-DbAdapter, should be unsafe. need to filter it.
     */
	void replace_fake_table(char *str, char *dest)
	{
		dest = str;
		return ;
	}


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 63 "yacc.y"
{
	char *val;
	int flag;
}
/* Line 193 of yacc.c.  */
#line 212 "yacc.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 225 "yacc.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   95

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  46
/* YYNRULES -- Number of states.  */
#define YYNSTATES  108

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   280

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    11,    20,    22,    26,
      30,    36,    41,    48,    55,    64,    66,    70,    78,    79,
      84,    90,    97,   105,   107,   111,   112,   115,   119,   120,
     123,   127,   132,   133,   136,   141,   147,   149,   153,   159,
     164,   166,   176,   178,   180,   184,   186
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      27,     0,    -1,    28,    -1,    36,    -1,    39,    -1,    41,
      -1,     3,    29,     4,    30,    31,    33,    34,    35,    -1,
      12,    -1,    12,    15,    12,    -1,    29,    13,    12,    -1,
      29,    13,    12,    15,    12,    -1,    12,    19,    12,    20,
      -1,    12,    19,    12,    20,    15,    12,    -1,    29,    13,
      12,    19,    12,    20,    -1,    29,    13,    12,    19,    12,
      20,    15,    12,    -1,    12,    -1,    30,    13,    12,    -1,
      30,     5,    12,     8,    12,    14,    12,    -1,    -1,     6,
      12,    14,    12,    -1,    31,    17,    12,    14,    12,    -1,
       6,    12,     7,    19,    32,    20,    -1,    31,    17,    12,
       7,    19,    32,    20,    -1,    12,    -1,    32,    13,    12,
      -1,    -1,     9,    12,    -1,    33,    13,    12,    -1,    -1,
      11,    12,    -1,    11,    12,    16,    -1,    34,    13,    12,
      16,    -1,    -1,    18,    12,    -1,    18,    12,    13,    12,
      -1,    22,    37,    23,    38,    31,    -1,    12,    -1,    12,
      14,    12,    -1,    38,    13,    12,    14,    12,    -1,    24,
       4,    40,    31,    -1,    12,    -1,    25,    42,    19,    43,
      20,    21,    19,    44,    20,    -1,    12,    -1,    12,    -1,
      43,    13,    12,    -1,    12,    -1,    44,    13,    12,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   132,   132,   133,   134,   135,   145,   165,   172,   184,
     196,   209,   222,   235,   248,   265,   271,   283,   309,   315,
     338,   358,   372,   389,   393,   407,   411,   422,   436,   440,
     451,   462,   477,   481,   491,   509,   528,   536,   546,   562,
     579,   590,   619,   626,   640,   662,   678
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SELECT", "FROM", "JOIN", "WHERE", "IN",
  "ON", "GROUPBY", "HAVING", "ORDERBY", "NAME", "COMMA", "RELATION", "AS",
  "ORDER_RULE", "AND", "LIMIT", "LPARATHES", "RPARATHES", "VALUES",
  "UPDATE", "SET", "DELETE", "INSERT", "$accept", "statement",
  "select_statement", "select_fields", "table_name", "where_opts",
  "in_opts", "groupby_opts", "orderby_opts", "limit_opts",
  "udpate_statement", "update_table_name", "update_opts",
  "delete_statement", "delete_table_name", "insert_statement",
  "insert_table_name", "insert_fieds", "insert_values", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    26,    27,    27,    27,    27,    28,    29,    29,    29,
      29,    29,    29,    29,    29,    30,    30,    30,    31,    31,
      31,    31,    31,    32,    32,    33,    33,    33,    34,    34,
      34,    34,    35,    35,    35,    36,    37,    38,    38,    39,
      40,    41,    42,    43,    43,    44,    44
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     8,     1,     3,     3,
       5,     4,     6,     6,     8,     1,     3,     7,     0,     4,
       5,     6,     7,     1,     3,     0,     2,     3,     0,     2,
       3,     4,     0,     2,     4,     5,     1,     3,     5,     4,
       1,     9,     1,     1,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     2,     3,     4,     5,
       7,     0,    36,     0,     0,    42,     0,     1,     0,     0,
       0,     0,     0,    40,    18,     0,     8,     0,    15,    18,
       9,     0,    18,     0,    39,    43,     0,    11,     0,     0,
      25,     0,     0,     0,     0,    35,     0,     0,     0,     0,
       0,     0,    16,     0,    28,    10,     0,    37,     0,     0,
       0,     0,    44,     0,    12,     0,    26,     0,     0,    32,
      13,     0,     0,    19,     0,     0,     0,     0,    29,    27,
       0,     0,     6,     0,    38,    23,     0,     0,    20,    45,
       0,     0,    30,     0,    33,    14,     0,    21,     0,     0,
      41,    17,    31,     0,    24,    22,    46,    34
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     5,     6,    11,    29,    34,    86,    54,    69,    82,
       7,    13,    32,     8,    24,     9,    16,    36,    90
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -20
static const yytype_int8 yypact[] =
{
      -3,     8,    16,    23,    27,    37,   -20,   -20,   -20,   -20,
      14,    -2,   -20,    17,    29,   -20,    24,   -20,    30,    32,
      33,    34,    35,   -20,    42,    38,   -20,    31,   -20,    -1,
      15,    39,     0,    40,    41,   -20,   -10,    44,    43,    45,
      -8,    48,    49,    50,    51,    41,     1,    52,    53,    28,
      54,    46,   -20,    55,    25,   -20,    36,   -20,    56,    57,
      59,     9,   -20,    58,   -20,    60,   -20,    61,    62,    13,
      63,    67,    68,   -20,    64,    69,    70,    71,    72,   -20,
      74,    75,   -20,    77,   -20,   -20,    -6,    68,   -20,   -20,
       4,    78,   -20,    76,    80,   -20,    79,   -20,     5,    82,
     -20,   -20,   -20,    83,   -20,   -20,   -20,   -20
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -20,   -20,   -20,   -20,   -20,     3,   -19,   -20,   -20,   -20,
     -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20,   -20
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
       1,    53,    20,    48,    38,    33,    33,    96,    59,    47,
      49,    21,    39,    44,    97,    60,    74,    99,    96,     2,
      10,     3,     4,    75,   100,   105,    80,    14,    12,    18,
      41,    81,    40,    19,    42,    45,    67,    17,    68,    15,
      22,    23,    26,    25,    27,    28,    30,    31,    33,    63,
      35,    37,    46,    43,    65,    51,    70,    52,    47,    50,
      55,    56,    57,    58,    61,    62,    64,    66,    98,     0,
      71,    73,    77,    78,    79,     0,    72,    76,    83,    84,
      85,    88,    89,    87,     0,    91,    93,    94,    92,    95,
     101,   104,   102,   103,   106,   107
};

static const yytype_int8 yycheck[] =
{
       3,     9,     4,    13,     5,     6,     6,    13,     7,    17,
      20,    13,    13,    13,    20,    14,     7,    13,    13,    22,
      12,    24,    25,    14,    20,    20,    13,     4,    12,    15,
      15,    18,    29,    19,    19,    32,    11,     0,    13,    12,
      23,    12,    12,    19,    12,    12,    12,    12,     6,    21,
      12,    20,    12,    14,     8,    12,    20,    12,    17,    15,
      12,    12,    12,    12,    12,    12,    12,    12,    87,    -1,
      14,    12,    12,    12,    12,    -1,    19,    19,    15,    12,
      12,    12,    12,    19,    -1,    14,    12,    12,    16,    12,
      12,    12,    16,    13,    12,    12
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    22,    24,    25,    27,    28,    36,    39,    41,
      12,    29,    12,    37,     4,    12,    42,     0,    15,    19,
       4,    13,    23,    12,    40,    19,    12,    12,    12,    30,
      12,    12,    38,     6,    31,    12,    43,    20,     5,    13,
      31,    15,    19,    14,    13,    31,    12,    17,    13,    20,
      15,    12,    12,     9,    33,    12,    12,    12,    12,     7,
      14,    12,    12,    21,    12,     8,    12,    11,    13,    34,
      20,    14,    19,    12,     7,    14,    19,    12,    12,    12,
      13,    18,    35,    15,    12,    12,    32,    19,    12,    12,
      44,    14,    16,    12,    12,    12,    13,    20,    32,    13,
      20,    12,    16,    13,    12,    20,    12,    12
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 6:
#line 146 "yacc.y"
    {
    	size_t length = strlen((yyvsp[(1) - (8)].val)) 		/*select*/
    					 + strlen((yyvsp[(2) - (8)].val))	/*select_fields*/
    					 + strlen((yyvsp[(3) - (8)].val))   /*from*/
    					 + strlen((yyvsp[(4) - (8)].val))   /*table_name*/
    					 + strlen((yyvsp[(5) - (8)].val))   /*where_opts*/
    					 + strlen((yyvsp[(6) - (8)].val))   /*groupby_opts*/
    					 + strlen((yyvsp[(7) - (8)].val))   /*orderby_opts*/
    					 + strlen((yyvsp[(8) - (8)].val))   /*limit_opts*/
    					 + 1;
    	sql_parsing_result.sql_result = (char *)malloc(sizeof(char) * length);
    	memset(sql_parsing_result.sql_result, 0, length);
    	sprintf(sql_parsing_result.sql_result, "%s %s %s %s%s%s%s%s", (yyvsp[(1) - (8)].val), (yyvsp[(2) - (8)].val), (yyvsp[(3) - (8)].val), (yyvsp[(4) - (8)].val), (yyvsp[(5) - (8)].val), (yyvsp[(6) - (8)].val), (yyvsp[(7) - (8)].val), (yyvsp[(8) - (8)].val));
    	sql_parsing_result.sql_type = 1;
    	sql_parsing_result.select_statement.select = "SELECT";
    	sql_parsing_result.select_statement.from = "FROM";
    ;}
    break;

  case 7:
#line 166 "yacc.y"
    {
		replace_fake_table(strdup((yyvsp[(1) - (1)].val)), (yyval.val));
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 0;
		/*$$ = strdup($1);*/
	;}
    break;

  case 8:
#line 173 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (3)].val)), (yyvsp[(3) - (3)].val));
		sprintf(fields, "%s %s %s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 1;
		free(fields);
	;}
    break;

  case 9:
#line 185 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (3)].val)), (yyvsp[(3) - (3)].val));
		sprintf(fields, "%s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 0;
		free(fields);
	;}
    break;

  case 10:
#line 197 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (5)].val)) + strlen((yyvsp[(2) - (5)].val)) + strlen((yyvsp[(3) - (5)].val))
					  + strlen((yyvsp[(4) - (5)].val)) + strlen((yyvsp[(5) - (5)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (5)].val)), (yyvsp[(3) - (5)].val));
		sprintf(fields, "%s%s%s %s %s", (yyvsp[(1) - (5)].val), (yyvsp[(2) - (5)].val), (yyvsp[(3) - (5)].val), (yyvsp[(4) - (5)].val), (yyvsp[(5) - (5)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 1;
		free(fields);
	;}
    break;

  case 11:
#line 210 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (4)].val)) + strlen((yyvsp[(2) - (4)].val)) + strlen((yyvsp[(3) - (4)].val))
					  + strlen((yyvsp[(4) - (4)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (4)].val)), (yyvsp[(3) - (4)].val));
		sprintf(fields, "%s%s%s%s", (yyvsp[(1) - (4)].val), (yyvsp[(2) - (4)].val), (yyvsp[(3) - (4)].val), (yyvsp[(4) - (4)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 0;
		free(fields);
	;}
    break;

  case 12:
#line 223 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (6)].val)) + strlen((yyvsp[(2) - (6)].val)) + strlen((yyvsp[(3) - (6)].val))
					  + strlen((yyvsp[(4) - (6)].val)) + strlen((yyvsp[(5) - (6)].val)) + strlen((yyvsp[(6) - (6)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (6)].val)), (yyvsp[(3) - (6)].val));
		sprintf(fields, "%s%s%s%s %s %s", (yyvsp[(1) - (6)].val), (yyvsp[(2) - (6)].val), (yyvsp[(3) - (6)].val), (yyvsp[(4) - (6)].val), (yyvsp[(5) - (6)].val), (yyvsp[(6) - (6)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 1;
		free(fields);
	;}
    break;

  case 13:
#line 236 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (6)].val)) + strlen((yyvsp[(2) - (6)].val)) + strlen((yyvsp[(3) - (6)].val))
					  + strlen((yyvsp[(4) - (6)].val)) + strlen((yyvsp[(5) - (6)].val)) + strlen((yyvsp[(6) - (6)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (6)].val)), (yyvsp[(3) - (6)].val));
		sprintf(fields, "%s%s%s%s%s%s", (yyvsp[(1) - (6)].val), (yyvsp[(2) - (6)].val), (yyvsp[(3) - (6)].val), (yyvsp[(4) - (6)].val), (yyvsp[(5) - (6)].val), (yyvsp[(6) - (6)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 0;
		free(fields);
	;}
    break;

  case 14:
#line 249 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (8)].val)) + strlen((yyvsp[(2) - (8)].val)) + strlen((yyvsp[(3) - (8)].val))
					  + strlen((yyvsp[(4) - (8)].val)) + strlen((yyvsp[(5) - (8)].val)) + strlen((yyvsp[(6) - (8)].val))
					  + strlen((yyvsp[(7) - (8)].val)) + strlen((yyvsp[(8) - (8)].val)) + 1;
		char *fields = (char *)malloc(sizeof(char)*length);
		memset(fields, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (8)].val)), (yyvsp[(3) - (8)].val));
		sprintf(fields, "%s%s%s%s%s%s %s %s", (yyvsp[(1) - (8)].val), (yyvsp[(2) - (8)].val), (yyvsp[(3) - (8)].val), (yyvsp[(4) - (8)].val), (yyvsp[(5) - (8)].val), (yyvsp[(6) - (8)].val), (yyvsp[(7) - (8)].val), (yyvsp[(8) - (8)].val));
		(yyval.val) = strdup(fields);
		sql_parsing_result.select_statement.fields = (yyval.val);
		sql_parsing_result.select_statement.field_as = 1;
		free(fields);
	;}
    break;

  case 15:
#line 266 "yacc.y"
    {
    	replace_fake_table(strdup((yyvsp[(1) - (1)].val)), (yyval.val));
    	sql_parsing_result.select_statement.table_name = (yyval.val);
    	/*$$ = strdup($1);*/
    ;}
    break;

  case 16:
#line 272 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *table_name = (char *)malloc(sizeof(char)*length);
		memset(table_name, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (3)].val)), (yyvsp[(3) - (3)].val));
		sprintf(table_name, "%s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(table_name);
		sql_parsing_result.select_statement.table_name = (yyval.val);
		free(table_name);
		sql_parsing_result.join_query = 1;
	;}
    break;

  case 17:
#line 284 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (7)].val)) + strlen((yyvsp[(2) - (7)].val)) + strlen((yyvsp[(3) - (7)].val)) 
						+ strlen((yyvsp[(4) - (7)].val)) + strlen((yyvsp[(5) - (7)].val)) + strlen((yyvsp[(6) - (7)].val)) + strlen((yyvsp[(7) - (7)].val)) + 1;
		char *join_table = (char *)malloc(sizeof(char)*length);
		memset(join_table, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (7)].val)), (yyvsp[(3) - (7)].val));
		sprintf(
				join_table, 
				"%s %s %s %s %s %s %s",
				(yyvsp[(1) - (7)].val), 
				(yyvsp[(2) - (7)].val), 
				(yyvsp[(3) - (7)].val), 
				(yyvsp[(4) - (7)].val), 
				(yyvsp[(5) - (7)].val), 
				(yyvsp[(6) - (7)].val), 
				(yyvsp[(7) - (7)].val)
			);
		(yyval.val) = strdup(join_table);
		sql_parsing_result.select_statement.table_name = (yyval.val);
		free(join_table);
		sql_parsing_result.join_query = 1;
	;}
    break;

  case 18:
#line 309 "yacc.y"
    {
		(yyval.val) = "";
		sql_parsing_result.select_statement.where_opts = (yyval.val);
		sql_parsing_result.update_statement.sharding_key_value = NULL;
		sql_parsing_result.delete_statement.sharding_key_value = NULL;
	;}
    break;

  case 19:
#line 316 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (4)].val)) + strlen((yyvsp[(2) - (4)].val))
					  + strlen((yyvsp[(3) - (4)].val)) + strlen((yyvsp[(4) - (4)].val)) + 1;
		char *where_opts = (char *)malloc(sizeof(char) * length);
		memset(where_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(2) - (4)].val)), (yyvsp[(2) - (4)].val));
		sprintf(where_opts, " %s %s%s%s", (yyvsp[(1) - (4)].val), (yyvsp[(2) - (4)].val), (yyvsp[(3) - (4)].val), (yyvsp[(4) - (4)].val));
		(yyval.val) = strdup(where_opts);
		sql_parsing_result.select_statement.where_opts = (yyval.val);
		sql_parsing_result.update_statement.where_opts = (yyval.val);
		sql_parsing_result.delete_statement.where_opts = (yyval.val);
		/**
		 * check whether the sharding key exists
		 */
		if ( ( stringexists((yyvsp[(2) - (4)].val), sharding_key) != -1) &&
			( stringexists((yyvsp[(3) - (4)].val), "=") != -1 ) )
		{
			sql_parsing_result.update_statement.sharding_key_value = (yyvsp[(4) - (4)].val);
			sql_parsing_result.delete_statement.sharding_key_value = (yyvsp[(4) - (4)].val);
		}
		free(where_opts);
	;}
    break;

  case 20:
#line 339 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (5)].val)) + strlen((yyvsp[(2) - (5)].val)) 
					  + strlen((yyvsp[(3) - (5)].val)) + strlen((yyvsp[(4) - (5)].val)) + strlen((yyvsp[(5) - (5)].val)) + 1;
		char *where_opts = (char *)malloc(sizeof(char) * length);
		memset(where_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (5)].val)), (yyvsp[(3) - (5)].val));
		sprintf(where_opts, " %s %s %s%s%s", (yyvsp[(1) - (5)].val), (yyvsp[(2) - (5)].val), (yyvsp[(3) - (5)].val), (yyvsp[(4) - (5)].val), (yyvsp[(5) - (5)].val));
		(yyval.val) = strdup(where_opts);
		sql_parsing_result.select_statement.where_opts = (yyval.val);
		sql_parsing_result.update_statement.where_opts = (yyval.val);
		sql_parsing_result.delete_statement.where_opts = (yyval.val);
		if ( (stringexists((yyvsp[(3) - (5)].val), sharding_key) != -1) &&
			( stringexists((yyvsp[(4) - (5)].val), "=") != -1 ) )
		{
			sql_parsing_result.update_statement.sharding_key_value = (yyvsp[(5) - (5)].val);
			sql_parsing_result.delete_statement.sharding_key_value = (yyvsp[(5) - (5)].val);
		}
		free(where_opts);
	;}
    break;

  case 21:
#line 359 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (6)].val)) + strlen((yyvsp[(2) - (6)].val)) 
					  + strlen((yyvsp[(3) - (6)].val)) + strlen((yyvsp[(4) - (6)].val)) + strlen((yyvsp[(5) - (6)].val)) + strlen((yyvsp[(6) - (6)].val)) + 1;
		char *where_opts = (char *)malloc(sizeof(char) * length);
		memset(where_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (6)].val)), (yyvsp[(3) - (6)].val));
		sprintf(where_opts, " %s %s %s%s%s%s", (yyvsp[(1) - (6)].val), (yyvsp[(2) - (6)].val), (yyvsp[(3) - (6)].val), (yyvsp[(4) - (6)].val), (yyvsp[(5) - (6)].val), (yyvsp[(6) - (6)].val));
		(yyval.val) = strdup(where_opts);
		sql_parsing_result.select_statement.where_opts = (yyval.val);
		sql_parsing_result.update_statement.where_opts = (yyval.val);
		sql_parsing_result.delete_statement.where_opts = (yyval.val);
		free(where_opts);
	;}
    break;

  case 22:
#line 373 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (7)].val)) + strlen((yyvsp[(2) - (7)].val)) 
					  + strlen((yyvsp[(3) - (7)].val)) + strlen((yyvsp[(4) - (7)].val)) + strlen((yyvsp[(5) - (7)].val)) + strlen((yyvsp[(6) - (7)].val))
					  + strlen((yyvsp[(7) - (7)].val)) + 1;
		char *where_opts = (char *)malloc(sizeof(char) * length);
		memset(where_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (7)].val)), (yyvsp[(3) - (7)].val));
		sprintf(where_opts, " %s %s %s %s %s%s%s", (yyvsp[(1) - (7)].val), (yyvsp[(2) - (7)].val), (yyvsp[(3) - (7)].val), (yyvsp[(4) - (7)].val), (yyvsp[(5) - (7)].val), (yyvsp[(6) - (7)].val), (yyvsp[(7) - (7)].val));
		(yyval.val) = strdup(where_opts);
		sql_parsing_result.select_statement.where_opts = (yyval.val);
		sql_parsing_result.update_statement.where_opts = (yyval.val);
		sql_parsing_result.delete_statement.where_opts = (yyval.val);
		free(where_opts);
	;}
    break;

  case 23:
#line 390 "yacc.y"
    {
		(yyval.val) = strdup((yyvsp[(1) - (1)].val));
	;}
    break;

  case 24:
#line 394 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) 
					  + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *in_opts = (char *)malloc(sizeof(char) * length);
		memset(in_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (3)].val)), (yyvsp[(3) - (3)].val));
		sprintf(in_opts, "%s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(in_opts);
		free(in_opts);
	;}
    break;

  case 25:
#line 407 "yacc.y"
    { 
		(yyval.val) = "";
		sql_parsing_result.select_statement.groupby_opts = (yyval.val);
	;}
    break;

  case 26:
#line 412 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (2)].val)) + strlen((yyvsp[(2) - (2)].val)) + 1;
		char *groupby_opts = (char *)malloc(sizeof(char) * length);
		memset(groupby_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(2) - (2)].val)), (yyvsp[(2) - (2)].val));
		sprintf(groupby_opts, " %s %s", (yyvsp[(1) - (2)].val), (yyvsp[(2) - (2)].val));
		(yyval.val) = strdup(groupby_opts);
		sql_parsing_result.select_statement.groupby_opts = (yyval.val);
		free(groupby_opts);
	;}
    break;

  case 27:
#line 423 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *groupby_opts = (char *)malloc(sizeof(char) * length);
		memset(groupby_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (3)].val)), (yyvsp[(3) - (3)].val));
		sprintf(groupby_opts, " %s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(groupby_opts);
		sql_parsing_result.select_statement.groupby_opts = (yyval.val);
		free(groupby_opts);
	;}
    break;

  case 28:
#line 436 "yacc.y"
    {
		(yyval.val) = "";
		sql_parsing_result.select_statement.orderby_opts = (yyval.val);
	;}
    break;

  case 29:
#line 441 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (2)].val)) + strlen((yyvsp[(2) - (2)].val)) + 1;
		char *orderby_opts = (char *)malloc(sizeof(char) * length);
		memset(orderby_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(2) - (2)].val)), (yyvsp[(2) - (2)].val));
		sprintf(orderby_opts, " %s %s", (yyvsp[(1) - (2)].val), (yyvsp[(2) - (2)].val));
		(yyval.val) = strdup(orderby_opts);
		sql_parsing_result.select_statement.orderby_opts = (yyval.val);
		free(orderby_opts);
	;}
    break;

  case 30:
#line 452 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *orderby_opts = (char *)malloc(sizeof(char) * length);
		memset(orderby_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(2) - (3)].val)), (yyvsp[(2) - (3)].val));
		sprintf(orderby_opts, " %s %s %s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(orderby_opts);
		sql_parsing_result.select_statement.orderby_opts = (yyval.val);
		free(orderby_opts);
	;}
    break;

  case 31:
#line 463 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (4)].val)) + strlen((yyvsp[(2) - (4)].val))
					  + strlen((yyvsp[(3) - (4)].val)) + strlen((yyvsp[(4) - (4)].val)) + 1;
		char *orderby_opts = (char *)malloc(sizeof(char) * length);
		memset(orderby_opts, 0, length);
		replace_fake_table(strdup((yyvsp[(3) - (4)].val)), (yyvsp[(3) - (4)].val));
		sprintf(orderby_opts, " %s%s%s %s", (yyvsp[(1) - (4)].val), (yyvsp[(2) - (4)].val), (yyvsp[(3) - (4)].val), (yyvsp[(4) - (4)].val));
		(yyval.val) = strdup(orderby_opts);
		sql_parsing_result.select_statement.orderby_opts = (yyval.val);
		free(orderby_opts);
	;}
    break;

  case 32:
#line 477 "yacc.y"
    {
		(yyval.val) = "";
		sql_parsing_result.select_statement.limit_opts = (yyval.val);
	;}
    break;

  case 33:
#line 482 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (2)].val)) + strlen((yyvsp[(2) - (2)].val)) + 1;
		char *limit_opts = (char *)malloc(sizeof(char) * length);
		memset(limit_opts, 0, length);
		sprintf(limit_opts, " %s %s", (yyvsp[(1) - (2)].val), (yyvsp[(2) - (2)].val));
		(yyval.val) = strdup(limit_opts);
		sql_parsing_result.select_statement.limit_opts = (yyval.val);
		free(limit_opts);
	;}
    break;

  case 34:
#line 492 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (4)].val)) + strlen((yyvsp[(2) - (4)].val))
					  + strlen((yyvsp[(3) - (4)].val)) + strlen((yyvsp[(4) - (4)].val)) + 1;
		char *limit_opts = (char *)malloc(sizeof(char) * length);
		memset(limit_opts, 0, length);
		sprintf(limit_opts, " %s %s%s%s", (yyvsp[(1) - (4)].val), (yyvsp[(2) - (4)].val), (yyvsp[(3) - (4)].val), (yyvsp[(4) - (4)].val));
		(yyval.val) = strdup(limit_opts);
		sql_parsing_result.select_statement.limit_opts = (yyval.val);
		free(limit_opts);
	;}
    break;

  case 35:
#line 510 "yacc.y"
    {
	    size_t length = strlen((yyvsp[(1) - (5)].val))      /*udpate*/
    					 + strlen((yyvsp[(2) - (5)].val))	/*table_name*/
    					 + strlen((yyvsp[(3) - (5)].val))   /*set*/
    					 + strlen((yyvsp[(4) - (5)].val))   /*update_opts*/
    					 + strlen((yyvsp[(5) - (5)].val))   /*where_opts*/
    					 + 1;
    	sql_parsing_result.sql_result = (char *)malloc(sizeof(char) * length);
    	memset(sql_parsing_result.sql_result, 0, length);
    	sprintf(sql_parsing_result.sql_result, "%s %s %s %s%s", (yyvsp[(1) - (5)].val), (yyvsp[(2) - (5)].val), (yyvsp[(3) - (5)].val), (yyvsp[(4) - (5)].val), (yyvsp[(5) - (5)].val));
    	sql_parsing_result.sql_type = 2;
    	sql_parsing_result.update_statement.update = "UPDATE";
    	sql_parsing_result.update_statement.set = "SET";
    	sql_parsing_result.join_query = 0;
	;}
    break;

  case 36:
#line 529 "yacc.y"
    {
		(yyval.val) = strdup((yyvsp[(1) - (1)].val));
		sql_parsing_result.update_statement.table_name = (yyval.val);
	;}
    break;

  case 37:
#line 537 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *update_opts = (char *)malloc(sizeof(char) * length);
		memset(update_opts, 0, length);
		sprintf(update_opts, "%s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(update_opts);
		sql_parsing_result.update_statement.update_opts = (yyval.val);
		free(update_opts);
	;}
    break;

  case 38:
#line 547 "yacc.y"
    {
		size_t length = strlen((yyvsp[(1) - (5)].val)) + strlen((yyvsp[(2) - (5)].val)) 
					  + strlen((yyvsp[(3) - (5)].val)) + strlen((yyvsp[(4) - (5)].val)) + strlen((yyvsp[(5) - (5)].val)) + 1;
		char *update_opts = (char *)malloc(sizeof(char) * length);
		memset(update_opts, 0, length);
		sprintf(update_opts, "%s%s%s%s%s", (yyvsp[(1) - (5)].val), (yyvsp[(2) - (5)].val), (yyvsp[(3) - (5)].val), (yyvsp[(4) - (5)].val), (yyvsp[(5) - (5)].val));
		(yyval.val) = strdup(update_opts);
		sql_parsing_result.update_statement.update_opts = (yyval.val);
		free(update_opts);
	;}
    break;

  case 39:
#line 563 "yacc.y"
    {
	    size_t length = strlen((yyvsp[(1) - (4)].val))      /*delete*/
    					 + strlen((yyvsp[(2) - (4)].val))	/*from*/
    					 + strlen((yyvsp[(3) - (4)].val))   /*delete_table_name*/
    					 + strlen((yyvsp[(4) - (4)].val))   /*where_opts*/
    					 + 1;
    	sql_parsing_result.sql_result = (char *)malloc(sizeof(char) * length);
    	memset(sql_parsing_result.sql_result, 0, length);
    	sprintf(sql_parsing_result.sql_result, "%s %s %s%s", (yyvsp[(1) - (4)].val), (yyvsp[(2) - (4)].val), (yyvsp[(3) - (4)].val), (yyvsp[(4) - (4)].val));
    	sql_parsing_result.sql_type = 3;
    	sql_parsing_result.delete_statement.delete = "DELETE";
    	sql_parsing_result.delete_statement.from = "FROM";
    	sql_parsing_result.join_query = 0;
	;}
    break;

  case 40:
#line 580 "yacc.y"
    {
		(yyval.val) = strdup((yyvsp[(1) - (1)].val));
		sql_parsing_result.delete_statement.table_name = (yyval.val);
	;}
    break;

  case 41:
#line 591 "yacc.y"
    {
	    size_t length = strlen((yyvsp[(1) - (9)].val))      /*insert into*/
    					 + strlen((yyvsp[(2) - (9)].val))	/*table_name*/
    					 + strlen((yyvsp[(3) - (9)].val))   /*LPARATHES*/
    					 + strlen((yyvsp[(4) - (9)].val))   /*insert_fieds*/
    					 + strlen((yyvsp[(5) - (9)].val))   /*RPARATHES*/
    					 + strlen((yyvsp[(6) - (9)].val))   /*values*/
    					 + strlen((yyvsp[(7) - (9)].val))   /*LPARATHES*/
    					 + strlen((yyvsp[(8) - (9)].val))   /*insert_values*/
    					 + strlen((yyvsp[(9) - (9)].val))   /*RPARATHES*/
    					 + 1;
    	sql_parsing_result.sql_result = (char *)malloc(sizeof(char) * length);
    	memset(sql_parsing_result.sql_result, 0, length);
    	sprintf(sql_parsing_result.sql_result, "%s %s %s%s%s %s%s%s%s", (yyvsp[(1) - (9)].val), (yyvsp[(2) - (9)].val), (yyvsp[(3) - (9)].val), (yyvsp[(4) - (9)].val), (yyvsp[(5) - (9)].val), (yyvsp[(6) - (9)].val), (yyvsp[(7) - (9)].val), (yyvsp[(8) - (9)].val), (yyvsp[(9) - (9)].val));
    	sql_parsing_result.insert_statement.insert_into = "INSERT INTO";
    	sql_parsing_result.join_query = 0;
    	sql_parsing_result.sql_type = 4;
    	if ( insert_fields_num != 0)
    	{
    		yyerror("'INSERT' fields don't match the values.");
    	}
    	if ( stringexists((yyvsp[(4) - (9)].val), sharding_key) == -1)
    	{
    		yyerror("'INSERT' command must provide shardingKey.");
    	}
	;}
    break;

  case 42:
#line 620 "yacc.y"
    {
		(yyval.val) = strdup((yyvsp[(1) - (1)].val));
		sql_parsing_result.insert_statement.table_name = (yyval.val);
	;}
    break;

  case 43:
#line 627 "yacc.y"
    {
		insert_fields_num++;

		(yyval.val) = strdup((yyvsp[(1) - (1)].val));
		sql_parsing_result.insert_statement.fields = (yyval.val);
		if ( !insert_key_stop ){
			insert_key_index++;
			if ( (stringexists((yyvsp[(1) - (1)].val), sharding_key) != -1))
			{
				insert_key_stop = 1;
			}
		}
	;}
    break;

  case 44:
#line 641 "yacc.y"
    {
		insert_fields_num++;
		
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *insert_fieds = (char *)malloc(sizeof(char) * length);
		memset(insert_fieds, 0, length);
		sprintf(insert_fieds, "%s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(insert_fieds);
		sql_parsing_result.insert_statement.fields = (yyval.val);
		if ( !insert_key_stop ){
			insert_key_index++;
			if ( (stringexists((yyvsp[(3) - (3)].val), sharding_key) != -1))
			{
				insert_key_stop = 1;
			}
		}
		free(insert_fieds);
	;}
    break;

  case 45:
#line 663 "yacc.y"
    {
		insert_fields_num--;
		insert_key_index--;
		if (insert_key_index == 0)
		{
			sql_parsing_result.insert_statement.sharding_key_value = (yyvsp[(1) - (1)].val);
		}
		size_t length = strlen((yyvsp[(1) - (1)].val)) + 1;
		char *insert_values = (char *)malloc(sizeof(char) * length);
		memset(insert_values, 0, length);
		sprintf(insert_values, "%s", (yyvsp[(1) - (1)].val));
		(yyval.val) = strdup(insert_values);
		sql_parsing_result.insert_statement.values = (yyval.val);
		free(insert_values);
	;}
    break;

  case 46:
#line 679 "yacc.y"
    {
		insert_fields_num--;
		insert_key_index--;
		if (insert_key_index == 0)
		{
			sql_parsing_result.insert_statement.sharding_key_value = (yyvsp[(3) - (3)].val);
		}
		size_t length = strlen((yyvsp[(1) - (3)].val)) + strlen((yyvsp[(2) - (3)].val)) + strlen((yyvsp[(3) - (3)].val)) + 1;
		char *insert_values = (char *)malloc(sizeof(char) * length);
		memset(insert_values, 0, length);
		sprintf(insert_values, "%s%s%s", (yyvsp[(1) - (3)].val), (yyvsp[(2) - (3)].val), (yyvsp[(3) - (3)].val));
		(yyval.val) = strdup(insert_values);
		sql_parsing_result.insert_statement.values = (yyval.val);
		free(insert_values);
	;}
    break;


/* Line 1267 of yacc.c.  */
#line 2124 "yacc.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 695 "yacc.y"


/**
 * If parsing got wrong, this function will be called.
 * and set the sql_parser_result to NULL;
 */
int yyerror(const char *s)
{
	sql_parsing_result.sql_result = strdup(s);
	sql_parsing_result.sql_type = 1064;
	return 0;
}

/**
 * Remember that the return string must be released using 'free()' call.
 * !!!:
 * calling:
 * free(pointer_to_result);
 */
SQL_PARSER_RESULT *parse_sql(char *sql_statement, char *shardingKey)
{
    insert_key_index = 0;
    insert_key_stop = 0;
    insert_fields_num = 0;
	sharding_key = shardingKey;
	
	/**
	 * Some value need to be reinitialise
	 *  in the next job.
	 */
	sql_parsing_result.update_statement.sharding_key_value = NULL;
	sql_parsing_result.insert_statement.sharding_key_value = NULL;
	sql_parsing_result.delete_statement.sharding_key_value = NULL;

    YY_BUFFER_STATE buffer = yy_scan_string(sql_statement);
    yyparse();
    yy_delete_buffer(buffer);
    return &sql_parsing_result;
}
