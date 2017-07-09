/* A Bison parser, made by GNU Bison 3.0.4.  */

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
#define YYBISON_VERSION "3.0.4"

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
    LET = 269,
    RETURN = 270,
    CONTINUE = 271,
    BREAK = 272,
    LPAR = 273,
    RPAR = 274,
    COLON = 275,
    COMMA = 276,
    LBRACE = 277,
    RBRACE = 278,
    LBRACK = 279,
    RBRACK = 280,
    SEMI = 281,
    ENUM = 282,
    INCR = 283,
    DECR = 284,
    DOT = 285,
    NOT = 286,
    BITNOT = 287,
    BITAND = 288,
    BITXOR = 289,
    BITOR = 290,
    TERN = 291,
    FORIN = 292,
    FOROF = 293,
    TRY = 294,
    CATCH = 295,
    FINALLY = 296,
    THROW = 297,
    NEW = 298,
    PLUS_ASSIGN = 299,
    MINUS_ASSIGN = 300,
    LSHIFT_ASSIGN = 301,
    RSHIFT_ASSIGN = 302,
    ASSIGN = 303,
    MPY_ASSIGN = 304,
    DIV_ASSIGN = 305,
    MOD_ASSIGN = 306,
    AND_ASSIGN = 307,
    XOR_ASSIGN = 308,
    OR_ASSIGN = 309,
    LOR = 310,
    LAND = 311,
    IDENTICAL = 312,
    NOTIDENTICAL = 313,
    LT = 314,
    LE = 315,
    EQ = 316,
    NEQ = 317,
    GT = 318,
    GE = 319,
    LSHIFT = 320,
    RSHIFT = 321,
    RUSHIFT = 322,
    PLUS = 323,
    MINUS = 324,
    MPY = 325,
    DIV = 326,
    MOD = 327,
    DEL = 328,
    TYPEOF = 329,
    UMINUS = 330
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

#line 199 "js.tab.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner, parseData *pd);

#endif /* !YY_YY_JS_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */
#line 21 "js.y" /* yacc.c:358  */

void yyerror( void *scanner, parseData *pd, const char *s);

#line 216 "js.tab.c" /* yacc.c:358  */

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
#define YYFINAL  76
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1657

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  121
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  297

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   330

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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   111,   111,   117,   126,   131,   149,   165,   186,   191,
     198,   208,   218,   226,   234,   243,   252,   261,   270,   281,
     292,   303,   316,   327,   340,   353,   364,   377,   391,   400,
     415,   423,   429,   434,   442,   447,   469,   483,   491,   507,
     515,   533,   542,   555,   564,   575,   585,   597,   606,   617,
     628,   636,   656,   664,   677,   689,   702,   715,   727,   739,
     751,   762,   773,   785,   797,   809,   821,   833,   845,   857,
     869,   881,   893,   905,   917,   929,   941,   953,   965,   974,
     983,   992,   997,  1007,  1018,  1029,  1040,  1051,  1062,  1073,
    1084,  1095,  1106,  1117,  1124,  1134,  1173,  1181,  1188,  1194,
    1217,  1237,  1244,  1251,  1256,  1261,  1284,  1289,  1298,  1316,
    1325,  1336,  1349,  1365,  1370,  1378,  1394,  1399,  1407,  1423,
    1428,  1439
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "NAME", "NUM", "EOS", "IF",
  "ELSE", "WHILE", "DO", "FOR", "FCN", "VAR", "LET", "RETURN", "CONTINUE",
  "BREAK", "LPAR", "RPAR", "COLON", "COMMA", "LBRACE", "RBRACE", "LBRACK",
  "RBRACK", "SEMI", "ENUM", "INCR", "DECR", "DOT", "NOT", "BITNOT",
  "BITAND", "BITXOR", "BITOR", "TERN", "FORIN", "FOROF", "TRY", "CATCH",
  "FINALLY", "THROW", "NEW", "PLUS_ASSIGN", "MINUS_ASSIGN",
  "LSHIFT_ASSIGN", "RSHIFT_ASSIGN", "ASSIGN", "MPY_ASSIGN", "DIV_ASSIGN",
  "MOD_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN", "LOR", "LAND",
  "IDENTICAL", "NOTIDENTICAL", "LT", "LE", "EQ", "NEQ", "GT", "GE",
  "LSHIFT", "RSHIFT", "RUSHIFT", "PLUS", "MINUS", "MPY", "DIV", "MOD",
  "DEL", "TYPEOF", "UMINUS", "$accept", "script", "pgmlist", "funcdef",
  "fname", "stmt", "stmtlist", "symbol", "decl", "scopeddecl",
  "scopedlist", "decllist", "enum", "enumlist", "expr", "exprlist",
  "arglist", "objarraylit", "elem", "arraylist", "elemlist", "paramlist", YY_NULL
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
     325,   326,   327,   328,   329,   330
};
# endif

#define YYPACT_NINF -123

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-123)))

#define YYTABLE_NINF -104

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     297,  -123,  -123,  -123,  -123,    -2,     9,   561,    11,    34,
      68,    68,   628,    50,    63,   628,   369,   628,    72,   628,
     628,   628,   628,    68,    82,   628,   628,   628,   628,   628,
     108,  -123,   441,   441,  -123,   780,    83,  -123,   628,   628,
     110,   103,   593,    99,   100,    73,   102,    98,    78,   106,
     104,     7,   105,  -123,  -123,   109,   112,   113,   513,   128,
     132,   131,   835,   130,   125,    10,    10,    10,    10,  -123,
     513,   890,    45,    10,    45,    45,  -123,  -123,  -123,   628,
     628,   628,  -123,  -123,   188,   628,   628,   628,   628,   628,
     628,   628,   628,   628,   628,   628,   628,   628,   628,   628,
     628,   628,   628,   628,   628,   628,   628,   628,   628,   628,
     628,   628,   628,   628,   628,   628,   628,   628,  -123,   174,
     175,  -123,   177,    68,    68,   670,   170,    68,    68,   628,
      68,  -123,   628,    68,  -123,   112,   113,  -123,  -123,   628,
     628,  -123,  -123,     7,  -123,   628,  -123,   150,   178,   179,
     181,  -123,   945,   186,  -123,  1000,  -123,  1585,    77,  1568,
    1055,  1440,  1440,  1440,  1440,  1440,  1440,  1440,  1440,  1440,
    1440,  1440,  1495,  1513,    13,    13,    13,    13,    13,    13,
      13,    13,   205,   205,   205,   333,   333,    45,    45,    45,
     561,   561,   628,    49,   180,    -4,    28,   182,   628,   628,
     628,   191,   190,   194,  1440,  -123,  1440,  -123,  1440,  1440,
    -123,  -123,   628,   125,  -123,    27,   628,  -123,  -123,   628,
     199,  -123,   195,   628,   628,   628,   628,   628,   628,   628,
    1110,  1165,   189,    68,   196,   197,  1440,  -123,   202,   208,
    -123,   -10,   561,   198,  1220,  1275,   206,   725,  1330,  1385,
     210,   561,   561,   628,  -123,   441,   441,   213,   513,  -123,
    -123,   561,   561,   628,   561,   561,   628,  -123,  -123,   219,
     216,   217,   222,   220,  -123,  -123,   225,  -123,  -123,   226,
     561,  -123,  -123,   224,  -123,   561,   561,  -123,   513,  -123,
    -123,   230,   207,   227,   513,   231,  -123
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    96,    36,    95,     2,     0,     0,   103,     0,     8,
       0,     0,   103,     0,     0,   103,    34,   113,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     3,     4,     4,   102,   104,     0,    98,   103,   103,
       8,     0,   103,     0,     0,    37,    43,     0,    39,    41,
       0,   116,     0,    15,    14,     0,    96,    36,    34,     0,
     117,     0,   114,     0,     0,    53,    54,    79,    80,   101,
      34,     0,    93,    78,    97,    52,     1,     6,     5,   106,
     103,     0,    55,    56,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,     0,
       0,     9,     0,     0,     0,   104,     0,   119,   119,     0,
       0,    32,     0,     0,    31,     0,     0,    13,    81,     0,
       0,    35,    30,   116,   109,   113,   110,    45,    47,     0,
       0,    12,   107,     0,   105,     0,    99,    75,    76,    77,
       0,    85,    86,    83,    84,    82,    87,    89,    88,    90,
      92,    91,    60,    61,    66,    67,    62,    63,    64,    65,
      69,    68,    59,    58,    57,    70,    71,    72,    74,    73,
     103,   103,   103,    43,     0,    37,     0,     0,     0,     0,
     103,   120,     0,     0,    38,    44,    40,    42,   112,   111,
     118,   115,     0,     0,    50,     0,   106,    94,   100,     0,
      10,    16,     0,     0,     0,   103,     0,     0,     0,   103,
       0,     0,     0,   119,     0,     0,    46,    48,     0,     0,
     108,    49,   103,     0,     0,     0,     0,    38,     0,     0,
       0,   103,   103,   103,   121,     4,     4,     0,    34,    11,
      17,   103,   103,   103,   103,   103,   103,    23,    26,     0,
       0,     0,     0,     0,    22,    25,     0,    21,    24,     0,
     103,     7,    51,     0,    28,   103,   103,    20,    34,    19,
      18,     0,    27,     0,    34,     0,    29
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -123,  -123,   -28,  -123,  -123,     0,   -57,    -8,   -21,  -123,
    -103,   -59,  -123,     8,    71,    -3,    31,  -123,  -123,   114,
     115,  -122
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    30,    31,    32,    44,    58,    59,    34,    46,    49,
      50,    47,   148,   149,    35,    36,   153,    37,    60,    63,
      61,   202
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      33,   141,    45,    48,    77,    78,   203,    41,    79,    52,
     135,   136,    55,   150,    81,    69,    38,   -39,    82,    83,
      84,   197,   -39,    85,    86,    87,    88,    39,    79,    42,
     207,    79,    33,    33,    81,   119,   120,    81,    43,   126,
      84,    82,    83,    84,   226,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,   194,   227,   228,   238,   239,    81,
     130,   205,     2,    82,    83,    84,    53,   154,   110,   111,
     112,   113,   114,   115,   116,   117,   223,   224,    62,    54,
      65,    66,    67,    68,    64,    79,    71,    72,    73,    74,
      75,    81,   193,   196,    70,    82,    83,    84,    76,   118,
      85,   254,   122,   125,   121,    45,   195,   127,   128,   201,
     201,   129,    45,   130,   131,    48,   132,   133,   138,   147,
     134,   137,   139,   140,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     152,   142,   155,   143,   144,   146,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   222,
     220,   221,   156,   190,   191,   192,   200,   232,   212,   213,
     204,   273,   214,   206,   215,   217,   225,   242,   229,   234,
     208,   209,   233,   235,   243,   253,    62,   272,   255,   256,
     257,   237,   246,    79,   260,   201,   250,   270,   271,    81,
     258,   291,   263,    82,    83,    84,   266,   295,   280,   281,
     282,   283,   259,   284,   285,   286,   288,   240,   293,   294,
     269,   267,   268,   292,   296,    33,    33,     0,   210,   211,
     276,   274,   275,   279,   277,   278,     0,     0,     0,   230,
     231,     0,     0,   113,   114,   115,   116,   117,     0,     0,
     287,     0,     0,   236,     0,   289,   290,   152,     0,     0,
     241,     0,     0,     0,   244,   245,     0,   247,   248,   249,
       1,     2,     3,     4,     5,     0,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,     0,     0,     0,    16,
       0,    17,     0,  -103,    18,    19,    20,     0,    21,    22,
      23,     0,     0,     0,     0,     0,    24,     0,     0,    25,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    79,     0,     0,     0,     0,     0,    81,     0,     0,
       0,    82,    83,    84,     0,     0,    27,     0,     0,     0,
      28,    29,    56,    57,     3,     0,     5,     0,     6,     7,
       8,    40,    10,    11,    12,    13,    14,    15,     0,     0,
       0,    16,     0,    17,     0,  -103,    18,    19,    20,     0,
      21,    22,    23,   115,   116,   117,     0,     0,    24,     0,
       0,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,     0,
       0,     0,    28,    29,     1,     2,     3,     0,     5,     0,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
       0,     0,     0,    16,     0,    17,     0,  -103,    18,    19,
      20,     0,    21,    22,    23,     0,     0,     0,     0,     0,
      24,     0,     0,    25,    26,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,    28,    29,     1,     2,     3,     0,
       5,     0,     6,     7,     8,    40,    10,    11,    12,    13,
      14,    15,     0,     0,     0,    16,     0,    17,     0,  -103,
      18,    19,    20,     0,    21,    22,    23,     0,     0,     0,
       0,     0,    24,     0,     0,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     1,     2,     3,     0,     5,     0,
       6,     7,     8,    40,    10,    11,    12,    13,    14,    15,
       0,     0,    27,    16,     0,    17,    28,    29,    18,    19,
      20,     0,    21,    22,    23,     0,     1,     2,     3,     0,
      24,     0,     0,    25,    26,    40,   123,   124,     0,     0,
       0,    15,     0,     0,     0,    51,     0,    17,     0,     0,
      18,    19,    20,     0,    21,    22,    23,     0,     0,     0,
      27,     1,     2,     3,    28,    29,    26,     0,     0,     0,
      40,     0,     0,     0,     0,     0,    15,     0,     0,     0,
      51,     0,    17,     0,     0,    18,    19,    20,     0,    21,
      22,    23,    27,     0,     0,     0,    28,    29,     0,     0,
       0,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    79,     0,
       0,    80,     0,     0,    81,     0,     0,    27,    82,    83,
      84,    28,    29,    85,    86,    87,    88,   198,   199,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,     0,     0,   -40,     0,     0,    81,
       0,   -40,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,     0,
       0,    80,     0,     0,    81,     0,     0,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,     0,     0,   145,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,     0,
       0,     0,     0,     0,    81,     0,   151,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,     0,     0,   216,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,     0,
       0,     0,     0,     0,    81,   218,     0,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,     0,   219,     0,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,   251,
       0,     0,     0,     0,    81,     0,     0,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,   252,     0,     0,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,   261,
       0,     0,     0,     0,    81,     0,     0,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,   262,     0,     0,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,   264,
       0,     0,     0,     0,    81,     0,     0,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,   265,     0,     0,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    88,     0,     0,     0,     0,     0,     0,     0,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,    79,     0,
       0,     0,     0,     0,    81,     0,     0,     0,    82,    83,
      84,     0,     0,    85,    86,    87,    88,     0,     0,     0,
       0,     0,     0,     0,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,    79,     0,     0,     0,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,    85,    86,
      87,    79,     0,     0,     0,     0,     0,    81,     0,     0,
       0,    82,    83,    84,     0,     0,    85,    86,    87,     0,
       0,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,     0,     0,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,    79,     0,     0,     0,
       0,     0,    81,     0,     0,     0,    82,    83,    84,     0,
       0,    85,    86,    79,     0,     0,     0,     0,     0,    81,
       0,     0,     0,    82,    83,    84,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,     0,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117
};

static const yytype_int16 yycheck[] =
{
       0,    58,    10,    11,    32,    33,   128,     7,    18,    12,
       3,     4,    15,    70,    24,    23,    18,    21,    28,    29,
      30,   124,    26,    33,    34,    35,    36,    18,    18,    18,
     133,    18,    32,    33,    24,    38,    39,    24,     4,    42,
      30,    28,    29,    30,    48,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,   123,    37,    38,    40,    41,    24,
      21,   130,     4,    28,    29,    30,    26,    80,    65,    66,
      67,    68,    69,    70,    71,    72,    37,    38,    17,    26,
      19,    20,    21,    22,    22,    18,    25,    26,    27,    28,
      29,    24,   123,   124,    22,    28,    29,    30,     0,    26,
      33,   233,     9,    42,     4,   123,   124,    18,    18,   127,
     128,    48,   130,    21,    26,   133,    48,    21,    19,     4,
      26,    26,    20,    20,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      79,    23,    81,    21,    23,    25,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   192,
     190,   191,     4,    19,    19,    18,    26,   200,    48,    21,
     129,   258,    23,   132,    23,    19,    26,     8,    26,    19,
     139,   140,    21,    19,    19,    26,   145,     4,    22,    22,
      18,   213,   225,    18,    26,   233,   229,   255,   256,    24,
      22,   288,    26,    28,    29,    30,    26,   294,    19,    23,
      23,    19,   242,    23,    19,    19,    22,   216,    41,    22,
     253,   251,   252,    23,    23,   255,   256,    -1,   143,   145,
     263,   261,   262,   266,   264,   265,    -1,    -1,    -1,   198,
     199,    -1,    -1,    68,    69,    70,    71,    72,    -1,    -1,
     280,    -1,    -1,   212,    -1,   285,   286,   216,    -1,    -1,
     219,    -1,    -1,    -1,   223,   224,    -1,   226,   227,   228,
       3,     4,     5,     6,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      -1,    24,    -1,    26,    27,    28,    29,    -1,    31,    32,
      33,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    42,
      43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    28,    29,    30,    -1,    -1,    69,    -1,    -1,    -1,
      73,    74,     3,     4,     5,    -1,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      -1,    22,    -1,    24,    -1,    26,    27,    28,    29,    -1,
      31,    32,    33,    70,    71,    72,    -1,    -1,    39,    -1,
      -1,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    69,    -1,
      -1,    -1,    73,    74,     3,     4,     5,    -1,     7,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    -1,    22,    -1,    24,    -1,    26,    27,    28,
      29,    -1,    31,    32,    33,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      69,    -1,    -1,    -1,    73,    74,     3,     4,     5,    -1,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    -1,    22,    -1,    24,    -1,    26,
      27,    28,    29,    -1,    31,    32,    33,    -1,    -1,    -1,
      -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,     5,    -1,     7,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    69,    22,    -1,    24,    73,    74,    27,    28,
      29,    -1,    31,    32,    33,    -1,     3,     4,     5,    -1,
      39,    -1,    -1,    42,    43,    12,    13,    14,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    22,    -1,    24,    -1,    -1,
      27,    28,    29,    -1,    31,    32,    33,    -1,    -1,    -1,
      69,     3,     4,     5,    73,    74,    43,    -1,    -1,    -1,
      12,    -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,
      22,    -1,    24,    -1,    -1,    27,    28,    29,    -1,    31,
      32,    33,    69,    -1,    -1,    -1,    73,    74,    -1,    -1,
      -1,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
      -1,    21,    -1,    -1,    24,    -1,    -1,    69,    28,    29,
      30,    73,    74,    33,    34,    35,    36,    37,    38,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    -1,    -1,    21,    -1,    -1,    24,
      -1,    26,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    -1,
      -1,    21,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    -1,    -1,    21,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    26,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    -1,    -1,    21,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    -1,
      -1,    -1,    -1,    -1,    24,    25,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    -1,    20,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    19,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    19,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    19,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    19,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    19,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    19,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    18,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,
      35,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    28,    29,    30,    -1,    -1,    33,    34,    35,    -1,
      -1,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    -1,    -1,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    18,    -1,    -1,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    28,    29,    30,    -1,
      -1,    33,    34,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    28,    29,    30,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    -1,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    22,    24,    27,    28,
      29,    31,    32,    33,    39,    42,    43,    69,    73,    74,
      77,    78,    79,    81,    83,    90,    91,    93,    18,    18,
      12,    81,    18,     4,    80,    83,    84,    87,    83,    85,
      86,    22,    91,    26,    26,    91,     3,     4,    81,    82,
      94,    96,    90,    95,    22,    90,    90,    90,    90,    83,
      22,    90,    90,    90,    90,    90,     0,    78,    78,    18,
      21,    24,    28,    29,    30,    33,    34,    35,    36,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    26,    91,
      91,     4,     9,    13,    14,    90,    91,    18,    18,    48,
      21,    26,    48,    21,    26,     3,     4,    26,    19,    20,
      20,    82,    23,    21,    23,    21,    25,     4,    88,    89,
      82,    26,    90,    92,    91,    90,     4,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      19,    19,    18,    84,    87,    83,    84,    86,    37,    38,
      26,    83,    97,    97,    90,    87,    90,    86,    90,    90,
      96,    95,    48,    21,    23,    23,    21,    19,    25,    20,
      81,    81,    91,    37,    38,    26,    48,    37,    38,    26,
      90,    90,    91,    21,    19,    19,    90,    89,    40,    41,
      92,    90,     8,    19,    90,    90,    91,    90,    90,    90,
      91,    19,    19,    26,    97,    22,    22,    18,    22,    81,
      26,    19,    19,    26,    19,    19,    26,    81,    81,    91,
      78,    78,     4,    82,    81,    81,    91,    81,    81,    91,
      19,    23,    23,    19,    23,    19,    19,    81,    22,    81,
      81,    82,    23,    41,    22,    82,    23
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    77,    78,    78,    78,    79,    80,    80,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    82,    82,    83,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    89,    89,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    91,    91,    91,    92,    92,    92,    93,
      93,    94,    94,    95,    95,    95,    96,    96,    96,    97,
      97,    97
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     2,     8,     0,     1,
       5,     7,     3,     3,     2,     2,     5,     7,    10,    10,
       9,     8,     8,     7,     8,     8,     7,    11,     8,    15,
       3,     3,     3,     2,     0,     2,     1,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     5,
       4,     8,     2,     2,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     4,     1,     1,     2,     1,     3,
       4,     2,     1,     0,     1,     3,     0,     1,     3,     3,
       3,     3,     3,     0,     1,     3,     0,     1,     3,     0,
       1,     3
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
#line 112 "js.y" /* yacc.c:1646  */
    {
			if (parseDebug) printf("script -> EOS\n");
			pd->beginning = 0;
			YYACCEPT;
		}
#line 1758 "js.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 118 "js.y" /* yacc.c:1646  */
    {
			if (parseDebug) printf("script -> pgmlist\n");
			pd->beginning = (yyvsp[0].slot);
			YYACCEPT;
		}
#line 1768 "js.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 127 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("pgmlist -> _empty_\n");
		}
#line 1777 "js.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 132 "js.y" /* yacc.c:1646  */
    {
			if ((yyvsp[-1].slot) == 0) {
				(yyval.slot) = (yyvsp[0].slot);
				if (parseDebug) printf("pgmlist -> empty stmt discard\n");
			} else {
				(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + (yyval.slot));
				ln->elem = (yyvsp[-1].slot);

				if ((yyvsp[0].slot) == 0) {
					ln->hdr->type = node_endlist;
					if (parseDebug) printf("pgmlist -> stmt[%d] %d\n", (yyvsp[-1].slot), (yyval.slot));
				} else {
					if (parseDebug) printf("pgmlist -> stmt[%d] pgmlist %d\n", (yyvsp[-1].slot), (yyval.slot));
				}
			}
		}
#line 1799 "js.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 150 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-1].slot);

			if ((yyvsp[0].slot) == 0) {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("pgmlist -> funcdef[%d] %d\n", (yyvsp[-1].slot), (yyval.slot));
			} else {
				if (parseDebug) printf("pgmlist -> funcdef[%d] pgmlist %d\n", (yyvsp[-1].slot), (yyval.slot));
			}
		}
#line 1816 "js.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 166 "js.y" /* yacc.c:1646  */
    {
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = (yyvsp[-6].slot);

			(yyval.slot) = newNode(pd, node_fcndef, sizeof(fcnDeclNode), true);
			fcnDeclNode *fd = (fcnDeclNode *)(pd->table + (yyval.slot));
			fd->hdr->type = node_fcndef;
			fd->name = node;
			fd->params = (yyvsp[-4].slot);
			fd->body = (yyvsp[-1].slot);

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + sym->name);
				printf("funcdef -> symbol[%s] LPAR paramlist[%d] RPAR LBRACE pgmlist[%d] RBRACE %d\n", sn->str.val, (yyvsp[-4].slot), (yyvsp[-1].slot), (yyval.slot));
			}
		}
#line 1838 "js.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 187 "js.y" /* yacc.c:1646  */
    {
			if (parseDebug) printf("fname -> _empty_\n");
			(yyval.slot) = 0;
		}
#line 1847 "js.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 192 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 1;
		}
#line 1855 "js.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 199 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-2].slot);
			ifthen->thenstmt = (yyvsp[0].slot);
			ifthen->elsestmt = 0;

			if (parseDebug) printf("stmt -> IF LPAR exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1869 "js.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 209 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-4].slot);
			ifthen->thenstmt = (yyvsp[-2].slot);
			ifthen->elsestmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> IF LPAR exprlist[%d] RPAR stmt[%d] ELSE stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1883 "js.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 219 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_throw, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (parseDebug) printf("stmt -> THROW expr[%d] SEMI %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 1895 "js.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 227 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (parseDebug) printf("stmt -> RETURN exprlist[%d] SEMI %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 1907 "js.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 235 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= ctl_break;
			en->expr = 0;

			if (parseDebug) printf("stmt -> BREAK SEMI %d\n", (yyval.slot));
		}
#line 1920 "js.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 244 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= ctl_continue;
			en->expr = 0;

			if (parseDebug) printf("stmt -> CONTINUE SEMI %d\n", (yyval.slot));
		}
#line 1933 "js.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 253 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> WHILE LPAR exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1946 "js.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 262 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[-5].slot);

			if (parseDebug) printf("stmt -> DO stmt[%d] WHILE LPAR exprlist[%d] RPAR SEMI %d\n", (yyvsp[-5].slot), (yyvsp[-2].slot), (yyval.slot));
		}
#line 1959 "js.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 271 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR LET scopedlist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1974 "js.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 282 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR VAR decllist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1989 "js.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 293 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR exprlist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2004 "js.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 304 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			pd->table[fn->var].flag |= flag_scope;

			if (parseDebug) printf("stmt -> FOR LPAR LET decl[%d] FORIN expr[%d] RPAR stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2021 "js.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 317 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR VAR decl[%d] FORIN expr[%d] RPAR stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2036 "js.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 328 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-4].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR expr[%d] FORIN expr[%d] RPAR stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2053 "js.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 341 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			pd->table[fn->var].flag |= flag_scope;

			if (parseDebug) printf("stmt -> FOR LPAR LET decl[%d] FOROF expr[%d] RPAR stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2070 "js.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 354 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR VAR decl[%d] FOROF expr[%d] RPAR stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2085 "js.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 365 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-4].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (parseDebug) printf("stmt -> FOR LPAR expr[%d] FOROF expr[%d] RPAR stmt[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2102 "js.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 378 "js.y" /* yacc.c:1646  */
    {
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = (yyvsp[-4].slot);

			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-8].slot);
			xn->binding = node;
			xn->catchblk = (yyvsp[-1].slot);

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE CATCH LPAR NAME[%d] RPAR LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-8].slot), node, (yyvsp[-1].slot), (yyval.slot));
		}
#line 2120 "js.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 392 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-5].slot);
			xn->finallyblk = (yyvsp[-1].slot);

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-5].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2133 "js.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 401 "js.y" /* yacc.c:1646  */
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

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE CATCH LPAR NAME[%d] RPAR LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-12].slot), node, (yyvsp[-5].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2152 "js.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 416 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_block, sizeof(blkEntryNode), true);
			blkEntryNode *be = (blkEntryNode *)(pd->table + (yyval.slot));
			be->body = (yyvsp[-1].slot);

			if (parseDebug) printf("stmt -> LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2164 "js.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 424 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);

			if (parseDebug) printf("stmt -> LET scopedlist SEMI %d\n", (yyvsp[-1].slot));
		}
#line 2174 "js.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 430 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);
			if (parseDebug) printf("stmt -> VAR decllist SEMI %d\n", (yyvsp[-1].slot));
		}
#line 2183 "js.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 435 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);
			if (parseDebug) printf("stmt -> exprlist SEMI %d\n", (yyvsp[-1].slot));
		}
#line 2192 "js.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 443 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("stmtlist -> _empty_\n");
		}
#line 2201 "js.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 448 "js.y" /* yacc.c:1646  */
    {
			if ((yyvsp[-1].slot) == 0) {
				(yyval.slot) = (yyvsp[0].slot);
				if (parseDebug) printf("stmtlist -> empty stmt discard\n");
			} else {
				(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + (yyval.slot));
				ln->elem = (yyvsp[-1].slot);

				if ((yyvsp[0].slot) == 0) {
					ln->hdr->type = node_endlist;
					ln->hdr->aux = aux_endstmt;
					if (parseDebug) printf("stmtlist -> stmt[%d] %d\n", (yyvsp[-1].slot), (yyval.slot));
				} else {
					if (parseDebug) printf("stmtlist -> stmt[%d] stmtlist %d\n", (yyvsp[-1].slot), (yyval.slot));
				}
			}
		}
#line 2224 "js.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 470 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + (yyval.slot));
			sym->name = (yyvsp[0].slot);

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("symbol -> NAME[%s] %d\n", sn->str.val, (yyval.slot));
			}
		}
#line 2239 "js.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 484 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl | flag_lval;
			(yyval.slot) = (yyvsp[0].slot);

			if (parseDebug) printf("decl -> symbol[%d]\n", (yyvsp[0].slot));
		}
#line 2251 "js.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 492 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[-2].slot));
			sym->hdr->flag |= flag_lval | flag_decl;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("decl -> symbol[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2268 "js.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 508 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl | flag_lval | flag_scope;
			(yyval.slot) = (yyvsp[0].slot);

			if (parseDebug) printf("scopeddecl -> symbol[%d]\n", (yyvsp[0].slot));
		}
#line 2280 "js.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 516 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[-2].slot));
			sym->hdr->flag |= flag_lval | flag_decl | flag_scope;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			pd->table[(yyvsp[-2].slot)].flag |= flag_scope;

			if (parseDebug) printf("scopeddecl -> symbol[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2299 "js.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 534 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug) printf("scopedlist -> decl[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2311 "js.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 543 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			pd->table[(yyvsp[-2].slot)].flag |= flag_scope;

			if (parseDebug) printf("scopedlist -> decl[%d] COMMA scopedlist[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2325 "js.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 556 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug) printf("decllist -> decl[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2337 "js.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 565 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (parseDebug) printf("decllist -> decl[%d] COMMA decllist[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2349 "js.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 576 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->left = (yyvsp[0].slot);
			bn->right = 0;

			if (parseDebug) printf("enum -> NAME[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2362 "js.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 586 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("enum -> NAME[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2375 "js.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 598 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug) printf("enumlist -> enum[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2387 "js.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 607 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (parseDebug) printf("enumlist -> enum[%d] COMMA enumlist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2399 "js.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 618 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ternary, sizeof(ternaryNode), true);
			ternaryNode *tn = (ternaryNode *)(pd->table + (yyval.slot));
			tn->condexpr = (yyvsp[-4].slot);
			tn->trueexpr = (yyvsp[-2].slot);
			tn->falseexpr = (yyvsp[0].slot);

			if (parseDebug) printf("expr -> expr[%d] TERN expr[%d] COLON expr[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2413 "js.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 629 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (parseDebug) printf("expr -> ENUM LBRACE enumlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2425 "js.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 637 "js.y" /* yacc.c:1646  */
    {
			int node = 0;

			if ((yyvsp[-6].slot)) {
				node = newNode(pd, node_var, sizeof(symNode), true);
				symNode *sym = (symNode *)(pd->table + node);
				sym->name = (yyvsp[-6].slot);
			}

			(yyval.slot) = newNode(pd, node_fcnexpr, sizeof(fcnDeclNode), true);
			fcnDeclNode *fd = (fcnDeclNode *)(pd->table + (yyval.slot));
			fd->hdr->type = node_fcnexpr;
			fd->name = node;
			fd->params = (yyvsp[-4].slot);
			fd->body = (yyvsp[-1].slot);

			if (parseDebug) printf("funcexpr -> FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", (yyval.slot));
		}
#line 2448 "js.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 657 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[0].slot);

			if (parseDebug) printf("expr -> TYPEOF expr %d\n", (yyval.slot));
		}
#line 2460 "js.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 665 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> INCR expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2476 "js.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 678 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> DECR expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2492 "js.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 690 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> expr[%d] INCR %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2508 "js.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 703 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> expr[%d] INCR %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2524 "js.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 716 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_rushift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] RUSHIFT expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2540 "js.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 728 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] RSHIFT expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2556 "js.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 740 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] LSHIFT expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2572 "js.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 752 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lor, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] LOR expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2587 "js.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 763 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_land, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] LAND expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2602 "js.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 774 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_lt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] LT expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2618 "js.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 786 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_le;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] LE expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2634 "js.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 798 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_eq;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] EQ expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2650 "js.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 810 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_ne;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] NEQ expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2666 "js.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 822 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_id;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] IDENTICAL expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2682 "js.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 834 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_nid;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] NOTIDENTICAL expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2698 "js.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 846 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_ge;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] GE expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2714 "js.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 858 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_gt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] GT expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2730 "js.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 870 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] PLUS expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2746 "js.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 882 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] MINUS expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2762 "js.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 894 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] MPY expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2778 "js.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 906 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] MOD expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2794 "js.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 918 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] DIV expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2810 "js.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 930 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] BITAND expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2826 "js.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 942 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] BITXOR expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2842 "js.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 954 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[-2].slot)].flag |= flag_operand;
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = math_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] BITOR expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2858 "js.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 966 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_uminus;
			en->expr = (yyvsp[0].slot);

			if (parseDebug) printf("expr -> UMINUS expr %d\n", (yyval.slot));
		}
#line 2871 "js.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 975 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_not;
			en->expr = (yyvsp[0].slot);

			if (parseDebug) printf("expr -> NOT expr %d\n", (yyval.slot));
		}
#line 2884 "js.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 984 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_bitnot;
			en->expr = (yyvsp[0].slot);

			if (parseDebug) printf("expr -> BITNOT expr %d\n", (yyval.slot));
		}
#line 2897 "js.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 993 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);
			if (parseDebug) printf("expr -> LPAR exprlist[%d] RPAR\n", (yyvsp[-1].slot));
		}
#line 2906 "js.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 998 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2920 "js.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 1008 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] LSHIFT_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2935 "js.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 1019 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] RSHIFT_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2950 "js.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 1030 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] PLUS_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2965 "js.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 1041 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] MINUS_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2980 "js.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1052 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] MPY_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2995 "js.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 1063 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] MOD_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 3010 "js.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 1074 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] DIV_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 3025 "js.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1085 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] AND_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 3040 "js.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1096 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] OR_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 3055 "js.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1107 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_opassign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->hdr->aux = pm_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) printf("expr -> expr[%d] XOR_ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 3070 "js.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1118 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			pd->table[(yyvsp[0].slot)].aux = aux_newobj;

			if (parseDebug) printf("expr -> NEW expr %d\n", (yyval.slot));
		}
#line 3081 "js.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1125 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + (yyval.slot));

			fc->name = (yyvsp[-3].slot);
			fc->args = (yyvsp[-1].slot);

			if (parseDebug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", (yyvsp[-3].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 3095 "js.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1135 "js.y" /* yacc.c:1646  */
    {
			if (parseDebug) {
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
#line 3138 "js.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1174 "js.y" /* yacc.c:1646  */
    {
			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("expr -> STRING[%s] %d\n", sn->str.val, (yyvsp[0].slot));
			}
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3150 "js.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1182 "js.y" /* yacc.c:1646  */
    {
			exprNode *en = (exprNode *)(pd->table + (yyvsp[0].slot));
			en->hdr->flag |= ctl_delete;

			if (parseDebug) printf("expr -> DEL expr[%d]\n", (yyvsp[0].slot));
		}
#line 3161 "js.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1189 "js.y" /* yacc.c:1646  */
    {
			if (parseDebug) printf("expr -> objarraylit\n");
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3170 "js.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 1195 "js.y" /* yacc.c:1646  */
    {
			int id = (yyvsp[-2].slot);

			(yyval.slot) = newNode(pd, node_access, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			while (id) {
			  bn = (binaryNode *)(pd->table + id);
			  if (bn->hdr->type == node_access || bn->hdr->type == node_lookup) {
				bn->hdr->aux += 1;
				id = bn->left;
			  } else
				break;
			}

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("expr -> expr[%d] DOT NAME[%s] %d\n", (yyvsp[-2].slot), sn->str.val, (yyval.slot));
			}
		}
#line 3197 "js.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1218 "js.y" /* yacc.c:1646  */
    {
			int id = (yyvsp[-3].slot);

			(yyval.slot) = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[-1].slot);
			bn->left = (yyvsp[-3].slot);

			while (id) {
			  bn = (binaryNode *)(pd->table + id);
			  if (bn->hdr->type == node_access || bn->hdr->type == node_lookup) {
				bn->hdr->aux += 1;
				id = bn->left;
			  } else
				break;
			}

			if (parseDebug) printf("expr -> expr[%d] LBRACK expr[%d] RBRACK %d\n", (yyvsp[-3].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 3221 "js.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 1238 "js.y" /* yacc.c:1646  */
    {
			Node *node = pd->table + (yyvsp[0].slot);
			if (parseDebug) printf ("expr -> BITAND expr[%d]\n", (yyvsp[0].slot));
			node->flag |= flag_lval;
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3232 "js.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 1245 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3240 "js.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 1252 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("exprlist -> _empty_\n");
		}
#line 3249 "js.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 1257 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			if (parseDebug) printf("exprlist -> expr[%d]\n", (yyvsp[0].slot));
		}
#line 3258 "js.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1262 "js.y" /* yacc.c:1646  */
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
			  if (parseDebug)
				printf("exprlist -> expr[%d] COMMA exprlist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
			  ln->hdr->type = node_endlist;
			  if (parseDebug) printf("exprlist -> expr[%d]\n", (yyvsp[-2].slot));
			}
		}
#line 3282 "js.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1285 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("arglist -> _empty_\n");
		}
#line 3291 "js.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1290 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug)
				printf("arglist -> expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3304 "js.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1299 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
			  if (parseDebug)
				printf("arglist -> expr[%d] COMMA arglist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
			  ln->hdr->type = node_endlist;
			  if (parseDebug)
				printf("arglist -> expr[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3323 "js.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 1317 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + (yyval.slot));
			on->elemlist = (yyvsp[-1].slot);

			if (parseDebug) printf("objarraylit -> LBRACE elemlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 3335 "js.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1326 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + (yyval.slot));
			an->exprlist = (yyvsp[-1].slot);

			if (parseDebug) printf("objarraylit -> LBRACK arraylist[%d] RBRACK %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 3347 "js.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1337 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[-2].slot));
				printf("elem -> NAME[%s] COLON expr[%d] %d\n", sn->str.val, (yyvsp[0].slot), (yyval.slot));
			}
		}
#line 3364 "js.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1350 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			pd->table[(yyvsp[0].slot)].flag |= flag_operand;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[-2].slot));
				printf("elem -> STRING[%s] COLON expr[%d] %d\n", sn->str.val, (yyvsp[0].slot), (yyval.slot));
			}
		}
#line 3381 "js.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1366 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("arraylist -> _empty_\n");
		}
#line 3390 "js.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 1371 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug) printf("arraylist -> expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3402 "js.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 1379 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
				if (parseDebug) printf("arraylist -> elem[%d] COMMA arraylist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("arraylist -> expr[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3419 "js.tab.c" /* yacc.c:1646  */
    break;

  case 116:
#line 1395 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("elemlist -> _empty_\n");
		}
#line 3428 "js.tab.c" /* yacc.c:1646  */
    break;

  case 117:
#line 1400 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug) printf("elemlist -> elem[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3440 "js.tab.c" /* yacc.c:1646  */
    break;

  case 118:
#line 1408 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
				if (parseDebug) printf("elemlist -> elem[%d] COMMA elemlist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("elemlist -> elem[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3457 "js.tab.c" /* yacc.c:1646  */
    break;

  case 119:
#line 1424 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (parseDebug) printf("paramlist -> _empty_\n");
		}
#line 3466 "js.tab.c" /* yacc.c:1646  */
    break;

  case 120:
#line 1429 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl;

			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (parseDebug) printf("paramlist -> symbol[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3481 "js.tab.c" /* yacc.c:1646  */
    break;

  case 121:
#line 1440 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[-2].slot));
			sym->hdr->flag |= flag_decl;

			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
				if (parseDebug) printf("paramlist -> symbol[%d] COMMA paramlist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("paramlist -> symbol[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3501 "js.tab.c" /* yacc.c:1646  */
    break;


#line 3505 "js.tab.c" /* yacc.c:1646  */
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
#line 1456 "js.y" /* yacc.c:1906  */


void yyerror( void *scanner, parseData *pd, const char *s)
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineNo, s);
}

