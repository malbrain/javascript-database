/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "3.0"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 4 "js.y" /* yacc.c:339  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "js.h"

#define YY_DECL
#include "js.tab.h"
#include "js.lex.h"

int yylex (YYSTYPE * yymathexpr_param, yyscan_t yyscanner, parseData *pd);

#line 79 "js.tab.c" /* yacc.c:339  */

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "js.tab.h".  */
#ifndef YY_YY_JS_TAB_H_INCLUDED
# define YY_YY_JS_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    STRING = 258,
    NAME = 259,
    NUM = 260,
    EOS = 261,
    IF = 262,
    ELSE = 263,
    WHILE = 264,
    DO = 265,
    FOR = 266,
    FCN = 267,
    VAR = 268,
    RETURN = 269,
    CONTINUE = 270,
    BREAK = 271,
    LPAR = 272,
    RPAR = 273,
    COLON = 274,
    COMMA = 275,
    LBRACE = 276,
    RBRACE = 277,
    LBRACK = 278,
    RBRACK = 279,
    SEMI = 280,
    ENUM = 281,
    INCR = 282,
    DECR = 283,
    DOT = 284,
    NOT = 285,
    BITNOT = 286,
    BITAND = 287,
    BITXOR = 288,
    BITOR = 289,
    TERN = 290,
    FORIN = 291,
    FOROF = 292,
    TRY = 293,
    CATCH = 294,
    FINALLY = 295,
    THROW = 296,
    NEW = 297,
    PLUS_ASSIGN = 298,
    MINUS_ASSIGN = 299,
    LSHIFT_ASSIGN = 300,
    RSHIFT_ASSIGN = 301,
    ASSIGN = 302,
    MPY_ASSIGN = 303,
    DIV_ASSIGN = 304,
    MOD_ASSIGN = 305,
    AND_ASSIGN = 306,
    XOR_ASSIGN = 307,
    OR_ASSIGN = 308,
    LOR = 309,
    LAND = 310,
    IDENTICAL = 311,
    NOTIDENTICAL = 312,
    LT = 313,
    LE = 314,
    EQ = 315,
    NEQ = 316,
    GT = 317,
    GE = 318,
    LSHIFT = 319,
    RSHIFT = 320,
    RUSHIFT = 321,
    PLUS = 322,
    MINUS = 323,
    MPY = 324,
    DIV = 325,
    MOD = 326,
    DEL = 327,
    TYPEOF = 328,
    UMINUS = 329
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 17 "js.y" /* yacc.c:355  */
	// yymathexpr
	uint32_t slot;

#line 198 "js.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner, parseData *pd);

#endif /* !YY_YY_JS_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 21 "js.y" /* yacc.c:358  */

void yyerror( void *scanner, parseData *pd, const char *s);

#line 215 "js.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if (! defined __GNUC__ || __GNUC__ < 2 \
      || (__GNUC__ == 2 && __GNUC_MINOR__ < 5))
#  define __attribute__(Spec) /* empty */
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  72
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1494

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  114
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  268

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   329

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   106,   106,   112,   121,   126,   144,   160,   181,   186,
     193,   203,   213,   221,   229,   238,   247,   256,   265,   276,
     287,   298,   311,   322,   335,   343,   357,   366,   381,   386,
     391,   399,   404,   425,   439,   447,   463,   472,   483,   493,
     505,   514,   525,   536,   544,   564,   572,   585,   597,   610,
     623,   633,   643,   653,   662,   671,   681,   691,   701,   711,
     721,   731,   741,   751,   761,   771,   781,   791,   801,   811,
     821,   831,   840,   849,   858,   863,   875,   887,   899,   911,
     923,   935,   947,   959,   971,   983,   995,  1002,  1012,  1051,
    1059,  1066,  1072,  1084,  1093,  1100,  1107,  1112,  1117,  1140,
    1145,  1154,  1172,  1181,  1192,  1204,  1219,  1224,  1232,  1248,
    1253,  1261,  1277,  1282,  1293
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "NAME", "NUM", "EOS", "IF",
  "ELSE", "WHILE", "DO", "FOR", "FCN", "VAR", "RETURN", "CONTINUE",
  "BREAK", "LPAR", "RPAR", "COLON", "COMMA", "LBRACE", "RBRACE", "LBRACK",
  "RBRACK", "SEMI", "ENUM", "INCR", "DECR", "DOT", "NOT", "BITNOT",
  "BITAND", "BITXOR", "BITOR", "TERN", "FORIN", "FOROF", "TRY", "CATCH",
  "FINALLY", "THROW", "NEW", "PLUS_ASSIGN", "MINUS_ASSIGN",
  "LSHIFT_ASSIGN", "RSHIFT_ASSIGN", "ASSIGN", "MPY_ASSIGN", "DIV_ASSIGN",
  "MOD_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN", "LOR", "LAND",
  "IDENTICAL", "NOTIDENTICAL", "LT", "LE", "EQ", "NEQ", "GT", "GE",
  "LSHIFT", "RSHIFT", "RUSHIFT", "PLUS", "MINUS", "MPY", "DIV", "MOD",
  "DEL", "TYPEOF", "UMINUS", "$accept", "script", "pgmlist", "funcdef",
  "fname", "stmt", "stmtlist", "symbol", "decl", "decllist", "enum",
  "enumlist", "expr", "exprlist", "arglist", "objarraylit", "elem",
  "arraylist", "elemlist", "paramlist", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329
};
# endif

#define YYPACT_NINF -121

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-121)))

#define YYTABLE_NINF -97

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     265,  -121,  -121,  -121,  -121,   -11,    -5,   526,     6,     5,
      13,   558,     2,     8,   558,   336,   558,    15,   558,   558,
     558,   558,    13,    21,   558,   558,   558,   558,   558,    37,
    -121,   407,   407,  -121,   655,    18,  -121,   558,   558,    41,
      38,    36,    29,    34,     7,    32,    30,    16,    33,  -121,
    -121,    42,    46,    50,   478,    39,    51,    48,   710,    55,
      81,    27,    27,    27,    27,  -121,   478,   765,     1,    27,
       1,     1,  -121,  -121,  -121,   558,   558,   558,  -121,  -121,
      82,   558,   558,   558,   558,   558,   558,   558,   558,   558,
     558,   558,   558,   558,   558,   558,   558,   558,   558,   558,
     558,   558,   558,   558,   558,   558,   558,   558,   558,   558,
     558,   558,   558,   558,  -121,    69,    70,  -121,    72,    13,
     600,    65,    13,    13,   558,    13,  -121,    46,    50,  -121,
    -121,   558,   558,  -121,  -121,    16,  -121,   558,  -121,    44,
      73,    76,    77,  -121,   820,    74,  -121,   875,  -121,  1423,
    1407,  1352,   930,  1205,  1205,  1205,  1205,  1205,  1205,  1205,
    1205,  1205,  1205,  1205,  1279,  1334,   107,   107,   107,   107,
     107,   107,   107,   107,   302,   302,   302,   177,   177,     1,
       1,     1,   526,   526,   558,   -15,    75,   558,   558,   558,
      83,    84,    87,  1205,  -121,  1205,  1205,  -121,  -121,   558,
      81,  -121,   -14,   558,  -121,  -121,   558,    86,  -121,    88,
     558,   558,   558,   985,  1040,    85,    13,    80,    91,  1205,
    -121,    78,    92,  -121,  1260,   526,    93,  1095,  1150,    94,
     526,   526,   558,  -121,   407,   407,   103,   478,  -121,  -121,
     526,   526,   558,  -121,  -121,    98,    99,   100,   102,   101,
    -121,  -121,   108,   526,  -121,  -121,   104,  -121,   526,  -121,
     478,  -121,   105,    89,   111,   478,   106,  -121
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    89,    33,    88,     2,     0,     0,    96,     0,     8,
       0,    96,     0,     0,    96,    31,   106,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,     4,     4,    95,    97,     0,    91,    96,    96,     8,
       0,    96,     0,     0,    34,    36,     0,   109,     0,    15,
      14,     0,    89,    33,    31,     0,   110,     0,   107,     0,
       0,    46,    47,    72,    73,    94,    31,     0,    86,    71,
      90,    45,     1,     6,     5,    99,    96,     0,    48,    49,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30,     0,     0,     9,     0,     0,
      97,     0,   112,   112,     0,     0,    29,     0,     0,    13,
      74,     0,     0,    32,    28,   109,   102,   106,   103,    38,
      40,     0,     0,    12,   100,     0,    98,     0,    92,    68,
      69,    70,     0,    78,    79,    76,    77,    75,    80,    82,
      81,    83,    85,    84,    53,    54,    59,    60,    55,    56,
      57,    58,    62,    61,    52,    51,    50,    63,    64,    65,
      67,    66,    96,    96,    96,    36,     0,     0,     0,    96,
     113,     0,     0,    35,    37,   105,   104,   111,   108,     0,
       0,    43,    24,    99,    87,    93,     0,    10,    16,     0,
       0,     0,    96,     0,     0,     0,   112,     0,     0,    39,
      41,     0,     0,   101,    42,    96,     0,     0,     0,     0,
      96,    96,    96,   114,     4,     4,     0,    31,    11,    17,
      96,    96,    96,    21,    23,     0,     0,     0,     0,     0,
      20,    22,     0,    96,     7,    44,     0,    26,    96,    19,
      31,    18,     0,    25,     0,    31,     0,    27
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -121,  -121,   -16,  -121,  -121,     0,   -53,    -8,    60,  -115,
    -121,   -30,    56,    -3,   -18,  -121,  -121,    52,    57,  -120
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    29,    30,    31,    43,    54,    55,    33,    45,    46,
     140,   141,    34,    35,   145,    36,    56,    59,    57,   191
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      32,   133,    44,   192,   186,   125,    37,    40,    48,    42,
     194,    51,    38,   142,    65,    73,    74,     2,    75,   127,
     128,   210,   211,    41,    77,   221,   222,    49,    78,    79,
      80,    32,    32,    50,   115,   116,    60,    72,   121,     1,
       2,     3,    66,   114,    75,   117,   122,   118,    39,   119,
      77,   123,   125,    14,   124,   126,    80,    47,   129,    16,
     130,   134,    17,    18,    19,   131,    20,    21,    22,   132,
     136,   135,    58,   146,    61,    62,    63,    64,    25,   138,
      67,    68,    69,    70,    71,   139,   148,   182,   183,   184,
     189,   199,   204,   200,   225,   236,   233,   120,   201,   202,
     212,   234,   217,   216,    26,   218,   226,   248,    27,    28,
     232,    44,   235,   237,   190,   190,   253,    44,   239,   242,
     256,   254,   255,   257,    75,   260,   258,   263,   267,   264,
      77,   144,   265,   147,    78,    79,    80,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     220,   106,   107,   108,   109,   110,   111,   112,   113,   185,
     193,   209,   207,   208,   249,   223,   215,   195,   196,   198,
       0,     0,   197,    58,    75,     0,     0,     0,     0,     0,
      77,     0,     0,     0,    78,    79,    80,   262,   190,   229,
       0,     0,   266,     0,     0,     0,     0,     0,   246,   247,
       0,     0,     0,     0,     0,   238,     0,     0,     0,   245,
     243,   244,     0,     0,    32,    32,     0,     0,     0,   252,
     250,   251,     0,   213,   214,     0,   111,   112,   113,     0,
       0,     0,     0,   259,     0,   219,     0,     0,   261,   144,
       0,     0,   224,     0,     0,     0,   227,   228,     1,     2,
       3,     4,     5,     0,     6,     7,     8,     9,    10,    11,
      12,    13,    14,     0,     0,     0,    15,     0,    16,     0,
     -96,    17,    18,    19,     0,    20,    21,    22,     0,     0,
       0,     0,     0,    23,     0,     0,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    75,
       0,     0,     0,     0,     0,    77,     0,     0,     0,    78,
      79,    80,     0,    26,     0,     0,     0,    27,    28,    52,
      53,     3,     0,     5,     0,     6,     7,     8,    39,    10,
      11,    12,    13,    14,     0,     0,     0,    15,     0,    16,
       0,   -96,    17,    18,    19,     0,    20,    21,    22,   109,
     110,   111,   112,   113,    23,     0,     0,    24,    25,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    26,     0,     0,     0,    27,    28,
       1,     2,     3,     0,     5,     0,     6,     7,     8,     9,
      10,    11,    12,    13,    14,     0,     0,     0,    15,     0,
      16,     0,   -96,    17,    18,    19,     0,    20,    21,    22,
       0,     0,     0,     0,     0,    23,     0,     0,    24,    25,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    26,     0,     0,     0,    27,
      28,     1,     2,     3,     0,     5,     0,     6,     7,     8,
      39,    10,    11,    12,    13,    14,     0,     0,     0,    15,
       0,    16,     0,   -96,    17,    18,    19,     0,    20,    21,
      22,     0,     0,     0,     0,     0,    23,     0,     0,    24,
      25,     0,     0,     0,     0,     0,     0,     0,     0,     1,
       2,     3,     0,     5,     0,     6,     7,     8,    39,    10,
      11,    12,    13,    14,     0,     0,    26,    15,     0,    16,
      27,    28,    17,    18,    19,     0,    20,    21,    22,     0,
       0,     1,     2,     3,    23,     0,     0,    24,    25,     0,
      39,     0,     0,     0,     0,    14,     0,     0,     0,    47,
       0,    16,     0,     0,    17,    18,    19,     0,    20,    21,
      22,     0,     0,     0,    26,     0,     0,     0,    27,    28,
      25,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    75,     0,     0,
      76,     0,     0,    77,     0,     0,    26,    78,    79,    80,
      27,    28,    81,    82,    83,    84,   187,   188,     0,     0,
       0,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    75,     0,     0,    76,     0,     0,    77,     0,
       0,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    75,     0,     0,
     137,     0,     0,    77,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    75,     0,     0,     0,     0,     0,    77,     0,
     143,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    75,     0,     0,
     203,     0,     0,    77,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    75,     0,     0,     0,     0,     0,    77,   205,
       0,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    75,     0,   206,
       0,     0,     0,    77,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    75,   230,     0,     0,     0,     0,    77,     0,
       0,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    75,   231,     0,
       0,     0,     0,    77,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    75,   240,     0,     0,     0,     0,    77,     0,
       0,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    75,   241,     0,
       0,     0,     0,    77,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,    75,     0,     0,     0,     0,     0,    77,     0,
       0,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,     0,     0,     0,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,    75,     0,     0,
       0,     0,     0,    77,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,    75,     0,     0,     0,
       0,     0,    77,     0,     0,     0,    78,    79,    80,     0,
       0,    81,    82,    83,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,    75,     0,     0,     0,     0,     0,    77,     0,     0,
       0,    78,    79,    80,     0,     0,    81,    82,    83,    75,
       0,     0,     0,     0,     0,    77,     0,     0,     0,    78,
      79,    80,     0,     0,    81,    82,     0,     0,     0,     0,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,     0,     0,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,    75,     0,     0,     0,     0,     0,
      77,     0,     0,     0,    78,    79,    80,     0,     0,    81,
      75,     0,     0,     0,     0,     0,    77,     0,     0,     0,
      78,    79,    80,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113
};

static const yytype_int16 yycheck[] =
{
       0,    54,    10,   123,   119,    20,    17,     7,    11,     4,
     125,    14,    17,    66,    22,    31,    32,     4,    17,     3,
       4,    36,    37,    17,    23,    39,    40,    25,    27,    28,
      29,    31,    32,    25,    37,    38,    21,     0,    41,     3,
       4,     5,    21,    25,    17,     4,    17,     9,    12,    13,
      23,    17,    20,    17,    47,    25,    29,    21,    25,    23,
      18,    22,    26,    27,    28,    19,    30,    31,    32,    19,
      22,    20,    16,    76,    18,    19,    20,    21,    42,    24,
      24,    25,    26,    27,    28,     4,     4,    18,    18,    17,
      25,    47,    18,    20,     8,    17,   216,    41,    22,    22,
      25,    21,    18,    20,    68,    18,    18,     4,    72,    73,
      25,   119,    21,    21,   122,   123,    18,   125,    25,    25,
      18,    22,    22,    22,    17,    21,    18,    22,    22,    40,
      23,    75,    21,    77,    27,    28,    29,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     200,    64,    65,    66,    67,    68,    69,    70,    71,   119,
     124,   184,   182,   183,   237,   203,   189,   131,   132,   137,
      -1,    -1,   135,   137,    17,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,   260,   216,   212,
      -1,    -1,   265,    -1,    -1,    -1,    -1,    -1,   234,   235,
      -1,    -1,    -1,    -1,    -1,   225,    -1,    -1,    -1,   232,
     230,   231,    -1,    -1,   234,   235,    -1,    -1,    -1,   242,
     240,   241,    -1,   187,   188,    -1,    69,    70,    71,    -1,
      -1,    -1,    -1,   253,    -1,   199,    -1,    -1,   258,   203,
      -1,    -1,   206,    -1,    -1,    -1,   210,   211,     3,     4,
       5,     6,     7,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    -1,    -1,    -1,    21,    -1,    23,    -1,
      25,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    -1,    -1,    38,    -1,    -1,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,
      28,    29,    -1,    68,    -1,    -1,    -1,    72,    73,     3,
       4,     5,    -1,     7,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    -1,    -1,    -1,    21,    -1,    23,
      -1,    25,    26,    27,    28,    -1,    30,    31,    32,    67,
      68,    69,    70,    71,    38,    -1,    -1,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,    72,    73,
       3,     4,     5,    -1,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    -1,    -1,    -1,    21,    -1,
      23,    -1,    25,    26,    27,    28,    -1,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,    72,
      73,     3,     4,     5,    -1,     7,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    -1,    -1,    -1,    21,
      -1,    23,    -1,    25,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,    -1,     7,    -1,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    -1,    -1,    68,    21,    -1,    23,
      72,    73,    26,    27,    28,    -1,    30,    31,    32,    -1,
      -1,     3,     4,     5,    38,    -1,    -1,    41,    42,    -1,
      12,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,    21,
      -1,    23,    -1,    -1,    26,    27,    28,    -1,    30,    31,
      32,    -1,    -1,    -1,    68,    -1,    -1,    -1,    72,    73,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,    -1,
      20,    -1,    -1,    23,    -1,    -1,    68,    27,    28,    29,
      72,    73,    32,    33,    34,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    17,    -1,    -1,    20,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    -1,    -1,
      20,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,
      25,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    -1,    -1,
      20,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    17,    -1,    -1,    -1,    -1,    -1,    23,    24,
      -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    -1,    19,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    17,    18,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    18,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    17,    18,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    18,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    -1,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    35,    17,    -1,    -1,    -1,
      -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,    -1,
      -1,    32,    33,    34,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    -1,    -1,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,
      -1,    27,    28,    29,    -1,    -1,    32,    33,    34,    17,
      -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,
      28,    29,    -1,    -1,    32,    33,    -1,    -1,    -1,    -1,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    -1,    -1,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    17,    -1,    -1,    -1,    -1,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    -1,    -1,    32,
      17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    21,    23,    26,    27,    28,
      30,    31,    32,    38,    41,    42,    68,    72,    73,    76,
      77,    78,    80,    82,    87,    88,    90,    17,    17,    12,
      80,    17,     4,    79,    82,    83,    84,    21,    88,    25,
      25,    88,     3,     4,    80,    81,    91,    93,    87,    92,
      21,    87,    87,    87,    87,    82,    21,    87,    87,    87,
      87,    87,     0,    77,    77,    17,    20,    23,    27,    28,
      29,    32,    33,    34,    35,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    25,    88,    88,     4,     9,    13,
      87,    88,    17,    17,    47,    20,    25,     3,     4,    25,
      18,    19,    19,    81,    22,    20,    22,    20,    24,     4,
      85,    86,    81,    25,    87,    89,    88,    87,     4,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    18,    18,    17,    83,    84,    36,    37,    25,
      82,    94,    94,    87,    84,    87,    87,    93,    92,    47,
      20,    22,    22,    20,    18,    24,    19,    80,    80,    88,
      36,    37,    25,    87,    87,    88,    20,    18,    18,    87,
      86,    39,    40,    89,    87,     8,    18,    87,    87,    88,
      18,    18,    25,    94,    21,    21,    17,    21,    80,    25,
      18,    18,    25,    80,    80,    88,    77,    77,     4,    81,
      80,    80,    88,    18,    22,    22,    18,    22,    18,    80,
      21,    80,    81,    22,    40,    21,    81,    22
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    76,    77,    77,    77,    78,    79,    79,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    81,    81,    82,    83,    83,    84,    84,    85,    85,
      86,    86,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    88,    88,    88,    89,
      89,    89,    90,    90,    91,    91,    92,    92,    92,    93,
      93,    93,    94,    94,    94
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     2,     8,     0,     1,
       5,     7,     3,     3,     2,     2,     5,     7,    10,     9,
       8,     7,     8,     7,     4,    11,     8,    15,     3,     3,
       2,     0,     2,     1,     1,     3,     1,     3,     1,     3,
       1,     3,     5,     4,     8,     2,     2,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     4,     1,     1,
       2,     1,     3,     4,     2,     1,     0,     1,     3,     0,
       1,     3,     3,     3,     3,     3,     0,     1,     3,     0,
       1,     3,     0,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (scanner, pd, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner, pd); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *scanner, parseData *pd)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (scanner);
  YYUSE (pd);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, void *scanner, parseData *pd)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, scanner, pd);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner, parseData *pd)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , scanner, pd);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, pd); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner, parseData *pd)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  YYUSE (pd);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner, parseData *pd)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner, pd);
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 107 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("script -> EOS\n");
			pd->beginning = 0;
			YYACCEPT;
		}
#line 1710 "js.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 113 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("script -> pgmlist\n");
			pd->beginning = (yyvsp[0].slot);
			YYACCEPT;
		}
#line 1720 "js.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 122 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("pgmlist -> _empty_\n");
		}
#line 1729 "js.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 127 "js.y" /* yacc.c:1646  */
    {
			if ((yyvsp[-1].slot) == 0) {
				(yyval.slot) = (yyvsp[0].slot);
				if (debug) printf("pgmlist -> empty stmt discard\n");
			} else {
				(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + (yyval.slot));
				ln->elem = (yyvsp[-1].slot);

				if ((yyvsp[0].slot) == 0) {
					ln->hdr->type = node_endlist;
					if (debug) printf("pgmlist -> stmt[%d] %d\n", (yyvsp[-1].slot), (yyval.slot));
				} else {
					if (debug) printf("pgmlist -> stmt[%d] pgmlist %d\n", (yyvsp[-1].slot), (yyval.slot));
				}
			}
		}
#line 1751 "js.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 145 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-1].slot);

			if ((yyvsp[0].slot) == 0) {
				ln->hdr->type = node_endlist;
				if (debug) printf("pgmlist -> funcdef[%d] %d\n", (yyvsp[-1].slot), (yyval.slot));
			} else {
				if (debug) printf("pgmlist -> funcdef[%d] pgmlist %d\n", (yyvsp[-1].slot), (yyval.slot));
			}
		}
#line 1768 "js.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 161 "js.y" /* yacc.c:1646  */
    {
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = (yyvsp[-6].slot);

			(yyval.slot) = newNode(pd, node_fcndef, sizeof(fcnDeclNode), true);
			fcnDeclNode *fn = (fcnDeclNode *)(pd->table + (yyval.slot));
			fn->hdr->type = node_fcndef;
			fn->name = node;
			fn->params = (yyvsp[-4].slot);
			fn->body = (yyvsp[-1].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + sym->name);
				printf("funcdef -> symbol[%s] LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", sn->string, (yyval.slot));
			}
		}
#line 1790 "js.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 182 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("fname -> _empty_\n");
			(yyval.slot) = 0;
		}
#line 1799 "js.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 187 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 1;
		}
#line 1807 "js.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 194 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-2].slot);
			ifthen->thenstmt = (yyvsp[0].slot);
			ifthen->elsestmt = 0;

			if (debug) printf("stmt -> IF LPAR exprlist RPAR stmt %d\n", (yyval.slot));
		}
#line 1821 "js.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 204 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-4].slot);
			ifthen->thenstmt = (yyvsp[-2].slot);
			ifthen->elsestmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> IF LPAR exprlist RPAR stmt ELSE stmt %d\n", (yyval.slot));
		}
#line 1835 "js.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 214 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_throw, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("stmt -> THROW expr SEMI %d\n", (yyval.slot));
		}
#line 1847 "js.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 222 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("stmt -> RETURN exprlist SEMI %d\n", (yyval.slot));
		}
#line 1859 "js.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 230 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= flag_break;
			en->expr = 0;

			if (debug) printf("stmt -> BREAK SEMI %d\n", (yyval.slot));
		}
#line 1872 "js.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 239 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= flag_continue;
			en->expr = 0;

			if (debug) printf("stmt -> CONTINUE SEMI %d\n", (yyval.slot));
		}
#line 1885 "js.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 248 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> WHILE LPAR exprlist RPAR stmt %d\n", (yyval.slot));
		}
#line 1898 "js.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 257 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[-5].slot);

			if (debug) printf("stmt -> DO stmt WHILE LPAR exprlist RPAR SEMI %d\n", (yyval.slot));
		}
#line 1911 "js.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 266 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decllist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1926 "js.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 277 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR exprlist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1941 "js.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 288 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decl FORIN expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1956 "js.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 299 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-4].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR expr FORIN expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1973 "js.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 312 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decl FOROF expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1988 "js.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 323 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-4].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR expr FOROF expr RPAR stmt %d\n", (yyval.slot));
		}
#line 2005 "js.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 336 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-1].slot);

			if (debug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2017 "js.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 344 "js.y" /* yacc.c:1646  */
    {
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = (yyvsp[-4].slot);

			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-8].slot);
			xn->binding = node;
			xn->catchblk = (yyvsp[-1].slot);

			if (debug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE CATCH LPAR NAME[%d] RPAR LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-8].slot), node, (yyvsp[-1].slot), (yyval.slot));
		}
#line 2035 "js.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 358 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-5].slot);
			xn->finallyblk = (yyvsp[-1].slot);

			if (debug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-5].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2048 "js.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 367 "js.y" /* yacc.c:1646  */
    {
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = (yyvsp[-8].slot);

			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-12].slot);
			xn->binding = node;
			xn->catchblk = (yyvsp[-5].slot);
			xn->finallyblk = (yyvsp[-1].slot);

			if (debug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE CATCH LPAR NAME[%d] RPAR LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-12].slot), node, (yyvsp[-5].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2067 "js.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 382 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> LBRACE stmtlist RBRACE\n");
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 2076 "js.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 387 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> VAR decllist SEMI %d\n", (yyvsp[-1].slot));
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 2085 "js.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 392 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> exprlist[%d] SEMI\n", (yyvsp[-1].slot));
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 2094 "js.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 400 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("stmtlist -> _empty_\n");
		}
#line 2103 "js.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 405 "js.y" /* yacc.c:1646  */
    {
			if ((yyvsp[-1].slot) == 0) {
				(yyval.slot) = (yyvsp[0].slot);
				if (debug) printf("stmtlist -> empty stmt discard\n");
			} else {
				(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + (yyval.slot));
				ln->elem = (yyvsp[-1].slot);

				if ((yyvsp[0].slot) == 0) {
					ln->hdr->type = node_endlist;
					if (debug) printf("stmtlist -> stmt[%d] %d\n", (yyvsp[-1].slot), (yyval.slot));
				} else {
					if (debug) printf("stmtlist -> stmt[%d] stmtlist %d\n", (yyvsp[-1].slot), (yyval.slot));
				}
			}
		}
#line 2125 "js.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 426 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + (yyval.slot));
			sym->name = (yyvsp[0].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("symbol -> NAME[%s] %d\n", sn->string, (yyval.slot));
			}
		}
#line 2140 "js.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 440 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl | flag_lval;
			(yyval.slot) = (yyvsp[0].slot);

			if (debug) printf("decl -> symbol[%d]\n", (yyvsp[0].slot));
		}
#line 2152 "js.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 448 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[-2].slot));
			sym->hdr->flag |= flag_lval | flag_decl;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("decl -> symbol[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2169 "js.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 464 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("decllist -> decl[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2181 "js.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 473 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (debug) printf("decllist -> decl[%d] COMMA decllist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2193 "js.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 484 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->left = (yyvsp[0].slot);
			bn->right = 0;

			if (debug) printf("enum -> NAME %d\n", (yyval.slot));
		}
#line 2206 "js.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 494 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("enum -> NAME ASSIGN expr %d\n", (yyval.slot));
		}
#line 2219 "js.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 506 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("enumlist -> enum[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2231 "js.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 515 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (debug) printf("enumlist -> enum[%d] COMMA enumlist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2243 "js.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 526 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ternary, sizeof(ternaryNode), true);
			ternaryNode *tn = (ternaryNode *)(pd->table + (yyval.slot));
			tn->condexpr = (yyvsp[-4].slot);
			tn->trueexpr = (yyvsp[-2].slot);
			tn->falseexpr = (yyvsp[0].slot);

			if (debug) printf("expr -> expr[%d] TERN expr[%d] COLON expr[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2257 "js.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 537 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("expr -> ENUM LBRACE enumlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2269 "js.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 545 "js.y" /* yacc.c:1646  */
    {
			int node = 0;

			if ((yyvsp[-6].slot)) {
				node = newNode(pd, node_var, sizeof(symNode), true);
				symNode *sym = (symNode *)(pd->table + node);
				sym->name = (yyvsp[-6].slot);
			}

			(yyval.slot) = newNode(pd, node_fcnexpr, sizeof(fcnDeclNode), true);
			fcnDeclNode *fn = (fcnDeclNode *)(pd->table + (yyval.slot));
			fn->hdr->type = node_fcnexpr;
			fn->name = node;
			fn->params = (yyvsp[-4].slot);
			fn->body = (yyvsp[-1].slot);

			if (debug) printf("funcexpr -> FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", (yyval.slot));
		}
#line 2292 "js.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 565 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> TYPEOF expr %d\n", (yyval.slot));
		}
#line 2304 "js.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 573 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> INCR expr %d\n", (yyval.slot));
		}
#line 2320 "js.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 586 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> DECR expr %d\n", (yyval.slot));
		}
#line 2336 "js.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 598 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", (yyval.slot));
		}
#line 2352 "js.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 611 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", (yyval.slot));
		}
#line 2368 "js.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 624 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_rushift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT expr %d\n", (yyval.slot));
		}
#line 2382 "js.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 634 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT expr %d\n", (yyval.slot));
		}
#line 2396 "js.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 644 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LSHIFT expr %d\n", (yyval.slot));
		}
#line 2410 "js.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 654 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lor, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LOR expr %d\n", (yyval.slot));
		}
#line 2423 "js.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 663 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_land, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LAND expr %d\n", (yyval.slot));
		}
#line 2436 "js.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 672 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_lt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LT expr %d\n", (yyval.slot));
		}
#line 2450 "js.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 682 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_le;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LE expr %d\n", (yyval.slot));
		}
#line 2464 "js.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 692 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_eq;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr EQ expr %d\n", (yyval.slot));
		}
#line 2478 "js.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 702 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_ne;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr NEQ expr %d\n", (yyval.slot));
		}
#line 2492 "js.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 712 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_id;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr IDENTICAL expr %d\n", (yyval.slot));
		}
#line 2506 "js.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 722 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_nid;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr NOTIDENTICAL expr %d\n", (yyval.slot));
		}
#line 2520 "js.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 732 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_ge;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr GE expr %d\n", (yyval.slot));
		}
#line 2534 "js.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 742 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_gt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr GT expr %d\n", (yyval.slot));
		}
#line 2548 "js.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 752 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr PLUS expr %d\n", (yyval.slot));
		}
#line 2562 "js.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 762 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MINUS expr %d\n", (yyval.slot));
		}
#line 2576 "js.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 772 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MPY expr %d\n", (yyval.slot));
		}
#line 2590 "js.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 782 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MOD expr %d\n", (yyval.slot));
		}
#line 2604 "js.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 792 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr DIV expr %d\n", (yyval.slot));
		}
#line 2618 "js.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 802 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITAND expr %d\n", (yyval.slot));
		}
#line 2632 "js.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 812 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITXOR expr %d\n", (yyval.slot));
		}
#line 2646 "js.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 822 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITOR expr %d\n", (yyval.slot));
		}
#line 2660 "js.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 832 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_uminus;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> UMINUS expr %d\n", (yyval.slot));
		}
#line 2673 "js.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 841 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_not;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> NOT expr %d\n", (yyval.slot));
		}
#line 2686 "js.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 850 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_bitnot;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> BITNOT expr %d\n", (yyval.slot));
		}
#line 2699 "js.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 859 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);
			if (debug) printf("expr -> LPAR exprlist[%d] RPAR\n", (yyvsp[-1].slot));
		}
#line 2708 "js.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 864 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2724 "js.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 876 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LSHIFT_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2740 "js.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 888 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2756 "js.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 900 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr PLUS_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2772 "js.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 912 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MINUS_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2788 "js.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 924 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MPY_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2804 "js.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 936 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MOD_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2820 "js.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 948 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr DIV_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2836 "js.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 960 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr AND_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2852 "js.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 972 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr OR_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2868 "js.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 984 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr XOR_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2884 "js.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 996 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			pd->table[(yyvsp[0].slot)].flag = flag_newobj;

			if (debug) printf("expr -> NEW expr %d\n", (yyval.slot));
		}
#line 2895 "js.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1003 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + (yyval.slot));

			fc->name = (yyvsp[-3].slot);
			fc->args = (yyvsp[-1].slot);

			if (debug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", (yyvsp[-3].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2909 "js.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 1013 "js.y" /* yacc.c:1646  */
    {
			if (debug) {
				numNode *nn = (numNode *)(pd->table + (yyvsp[0].slot));

				switch(nn->hdr->aux) {
				  case nn_dbl:
					printf("expr -> DBL[%G] %d\n", nn->dblval, (yyvsp[0].slot));
					break;
				  case nn_int:
					printf("expr -> INT[%" PRIi64 "] %d\n", nn->intval, (yyvsp[0].slot));
					break;
				  case nn_bool:
					printf("expr -> BOOL[%d] %d\n", nn->boolval, (yyvsp[0].slot));
					break;
				  case nn_null:
					printf("expr -> NULL %d\n", (yyvsp[0].slot));
					break;
				  case nn_this:
					printf("expr -> THIS %d\n", (yyvsp[0].slot));
					break;
				  case nn_args:
					printf("expr -> ARGS %d\n", (yyvsp[0].slot));
					break;
				  case nn_undef:
					printf("expr -> UNDEFINED %d\n", (yyvsp[0].slot));
					break;
				  case nn_infinity:
					printf("expr -> INFINITY %d\n", (yyvsp[0].slot));
					break;
				  case nn_nan:
					printf("expr -> NAN %d\n", (yyvsp[0].slot));
					break;
				  default:
					printf("expr -> DEFAULT %d\n", (yyvsp[0].slot));
				}
			}
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2952 "js.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 1052 "js.y" /* yacc.c:1646  */
    {
			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("expr -> STRING[%s] %d\n", sn->string, (yyvsp[0].slot));
			}
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2964 "js.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1060 "js.y" /* yacc.c:1646  */
    {
			exprNode *en = (exprNode *)(pd->table + (yyvsp[0].slot));
			en->hdr->flag |= flag_delete;

			if (debug) printf("expr -> DEL expr[%d]\n", (yyvsp[0].slot));
		}
#line 2975 "js.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1067 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("expr -> objarraylit\n");
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2984 "js.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1073 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_access, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("expr -> expr[%d] DOT NAME[%s] %d\n", (yyvsp[-2].slot), sn->string, (yyval.slot));
			}
		}
#line 3000 "js.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1085 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[-1].slot);
			bn->left = (yyvsp[-3].slot);

			if (debug) printf("expr -> expr LBRACK expr RBRACK %d\n", (yyval.slot));
		}
#line 3013 "js.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1094 "js.y" /* yacc.c:1646  */
    {
			Node *node = pd->table + (yyvsp[0].slot);
			if (debug) printf ("expr -> BITAND expr[%d]\n", (yyvsp[0].slot));
			node->flag |= flag_lval;
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3024 "js.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1101 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3032 "js.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1108 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("exprlist -> _empty_\n");
		}
#line 3041 "js.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1113 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("exprlist -> expr[%d]\n", (yyvsp[0].slot));
		}
#line 3050 "js.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1118 "js.y" /* yacc.c:1646  */
    {
			if ((yyvsp[0].slot) && pd->table[(yyvsp[0].slot)].type != node_list) { 
				(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + (yyval.slot));
				ln->elem = (yyvsp[0].slot);
			}

			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
			  if (debug)
				printf("exprlist -> expr[%d] COMMA exprlist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
			  ln->hdr->type = node_endlist;
			  if (debug) printf("exprlist -> expr[%d]\n", (yyvsp[-2].slot));
			}
		}
#line 3074 "js.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 1141 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("arglist -> _empty_\n");
		}
#line 3083 "js.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1146 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug)
				printf("arglist -> expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3096 "js.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 1155 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
			  if (debug)
				printf("arglist -> expr[%d] COMMA arglist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
			  ln->hdr->type = node_endlist;
			  if (debug)
				printf("arglist -> expr[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3115 "js.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 1173 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + (yyval.slot));
			on->elemlist = (yyvsp[-1].slot);

			if (debug) printf("objarraylit -> LBRACE elemlist RBRACE %d\n", (yyval.slot));
		}
#line 3127 "js.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 1182 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + (yyval.slot));
			an->exprlist = (yyvsp[-1].slot);

			if (debug) printf("objarraylit -> LBRACK arraylist[%d] RBRACK %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 3139 "js.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 1193 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[-2].slot));
				printf("elem -> NAME[%s] COLON expr %d\n", sn->string, (yyval.slot));
			}
		}
#line 3155 "js.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1205 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[-2].slot));
				printf("elem -> STRING[%s] COLON expr %d\n", sn->string, (yyval.slot));
			}
		}
#line 3171 "js.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1220 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("arraylist -> _empty_\n");
		}
#line 3180 "js.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1225 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("arraylist -> expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3192 "js.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1233 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
				if (debug) printf("arraylist -> elem[%d] COMMA arraylist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
				ln->hdr->type = node_endlist;
				if (debug) printf("arraylist -> expr[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3209 "js.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 1249 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("elemlist -> _empty_\n");
		}
#line 3218 "js.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1254 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("elemlist -> elem[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3230 "js.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1262 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
				if (debug) printf("elemlist -> elem[%d] COMMA elemlist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
				ln->hdr->type = node_endlist;
				if (debug) printf("elemlist -> elem[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3247 "js.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1278 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("paramlist -> _empty_\n");
		}
#line 3256 "js.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1283 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl;

			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("paramlist -> symbol[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3271 "js.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 1294 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[-2].slot));
			sym->hdr->flag |= flag_decl;

			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
				if (debug) printf("paramlist -> symbol[%d] COMMA paramlist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
				ln->hdr->type = node_endlist;
				if (debug) printf("paramlist -> symbol[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3291 "js.tab.c" /* yacc.c:1646  */
    break;


#line 3295 "js.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, pd, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, pd, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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
                      yytoken, &yylval, scanner, pd);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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
                  yystos[yystate], yyvsp, scanner, pd);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, pd, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner, pd);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner, pd);
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
  return yyresult;
}
#line 1310 "js.y" /* yacc.c:1906  */


void yyerror( void *scanner, parseData *pd, const char *s)
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineNo, s);
}

