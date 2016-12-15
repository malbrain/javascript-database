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

static bool debug = false;

void yyerror( void *scanner, parseData *pd, const char *s);

#line 217 "js.tab.c" /* yacc.c:358  */

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
#define YYFINAL  70
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1488

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  269

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
       0,   108,   108,   114,   123,   128,   146,   162,   183,   188,
     195,   205,   215,   223,   231,   240,   249,   258,   267,   278,
     289,   300,   313,   324,   337,   345,   359,   368,   383,   388,
     393,   401,   406,   427,   441,   449,   465,   474,   485,   495,
     507,   516,   527,   538,   546,   566,   574,   587,   599,   612,
     625,   635,   645,   655,   664,   673,   683,   693,   703,   713,
     723,   733,   743,   753,   763,   773,   783,   793,   803,   813,
     823,   833,   842,   851,   860,   865,   877,   889,   901,   913,
     925,   937,   949,   961,   973,   985,   997,  1004,  1014,  1053,
    1061,  1068,  1074,  1086,  1095,  1102,  1107,  1112,  1135,  1142,
    1150,  1155,  1164,  1182,  1191,  1202,  1214,  1229,  1234,  1242,
    1258,  1263,  1271,  1287,  1292,  1303
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
  "enumlist", "expr", "exprlist", "arg", "arglist", "objarraylit", "elem",
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

#define YYPACT_NINF -120

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-120)))

#define YYTABLE_NINF -96

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     272,  -120,  -120,  -120,  -120,   -12,    -2,   533,     5,    25,
      35,   599,    16,    20,   599,   343,   599,    31,   599,   599,
     599,   599,    41,   599,   599,   599,   599,   599,    64,  -120,
     414,   414,  -120,   696,    43,  -120,   599,   599,    66,    57,
      23,    55,    59,    30,    70,    54,    39,    67,  -120,  -120,
      75,    65,    78,   485,    72,    79,    81,   751,    76,    97,
      38,    38,    38,    38,   485,   806,    -4,    38,    -4,    -4,
    -120,  -120,  -120,   565,   599,   599,  -120,  -120,   100,   599,
     599,   599,   599,   599,   599,   599,   599,   599,   599,   599,
     599,   599,   599,   599,   599,   599,   599,   599,   599,   599,
     599,   599,   599,   599,   599,   599,   599,   599,   599,   599,
     599,   599,  -120,    87,    88,  -120,    90,    35,   641,    84,
      35,    35,   599,    35,  -120,    65,    78,  -120,  -120,   599,
     599,  -120,  -120,    39,  -120,   599,  -120,    63,    93,    96,
      98,  -120,    35,  1191,    99,   103,  -120,   861,  -120,  1409,
    1393,  1338,   916,  1191,  1191,  1191,  1191,  1191,  1191,  1191,
    1191,  1191,  1191,  1191,  1265,  1320,  1417,  1417,  1417,  1417,
    1417,  1417,  1417,  1417,   -11,   -11,   -11,    46,    46,    -4,
      -4,    -4,   533,   533,   599,   -16,   101,   599,   599,   599,
     102,   105,   106,  1191,  -120,  1191,  1191,  -120,  -120,   599,
      97,  -120,     8,  -120,   565,  -120,  -120,   599,   117,  -120,
     109,   599,   599,   599,   971,  1026,   104,    35,   107,   110,
    1191,  -120,   113,   111,  -120,  1246,   533,   114,  1081,  1136,
     115,   533,   533,   599,  -120,   414,   414,   130,   485,  -120,
    -120,   533,   533,   599,  -120,  -120,   118,   116,   152,   157,
     154,  -120,  -120,   159,   533,  -120,  -120,   158,  -120,   533,
    -120,   485,  -120,   156,   140,   164,   485,   165,  -120
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    89,    33,    88,     2,     0,     0,    95,     0,     8,
       0,    95,     0,     0,    95,    31,   107,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     3,
       4,     4,    94,    96,     0,    91,    95,    95,     8,     0,
      95,     0,     0,    34,    36,     0,   110,     0,    15,    14,
       0,    89,    33,    31,     0,   111,     0,   108,     0,     0,
      46,    47,    72,    73,    31,     0,    86,    71,    90,    45,
       1,     6,     5,   100,    95,     0,    48,    49,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,     0,     0,     9,     0,     0,    96,     0,
     113,   113,     0,     0,    29,     0,     0,    13,    74,     0,
       0,    32,    28,   110,   103,   107,   104,    38,    40,     0,
       0,    12,     0,    99,   101,     0,    97,     0,    92,    68,
      69,    70,     0,    78,    79,    76,    77,    75,    80,    82,
      81,    83,    85,    84,    53,    54,    59,    60,    55,    56,
      57,    58,    62,    61,    52,    51,    50,    63,    64,    65,
      67,    66,    95,    95,    95,    36,     0,     0,     0,    95,
     114,     0,     0,    35,    37,   106,   105,   112,   109,     0,
       0,    43,    24,    98,   100,    87,    93,     0,    10,    16,
       0,     0,     0,    95,     0,     0,     0,   113,     0,     0,
      39,    41,     0,     0,   102,    42,    95,     0,     0,     0,
       0,    95,    95,    95,   115,     4,     4,     0,    31,    11,
      17,    95,    95,    95,    21,    23,     0,     0,     0,     0,
       0,    20,    22,     0,    95,     7,    44,     0,    26,    95,
      19,    31,    18,     0,    25,     0,    31,     0,    27
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -120,  -120,   -21,  -120,  -120,     0,   -50,    -9,    73,   -85,
    -120,    -7,    62,    -3,  -120,   -15,  -120,  -120,    60,    61,
    -119
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    28,    29,    30,    42,    53,    54,    32,    44,    45,
     138,   139,    33,    34,   144,   145,    35,    55,    58,    56,
     191
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      31,    43,   192,   131,   123,    36,    73,    39,    47,    71,
      72,    50,    75,    73,   140,    37,    76,    77,    78,    75,
     211,   212,    40,    76,    77,    78,     1,     2,     3,    41,
      31,    31,   186,   113,   114,    38,   117,   119,   194,     2,
      14,    48,   125,   126,    46,    49,    16,   222,   223,    17,
      18,    19,    59,    20,    21,    73,   107,   108,   109,   110,
     111,    75,    64,    73,    70,    24,   116,    78,   112,    75,
     115,   146,   120,    76,    77,    78,   121,   122,    57,   124,
      60,    61,    62,    63,   129,    65,    66,    67,    68,    69,
     123,    25,   127,   128,   132,    26,    27,   130,   234,   133,
     136,   137,   118,   134,   148,   182,   183,   184,    43,   189,
     199,   190,   190,   200,    43,   109,   110,   111,   201,   204,
     202,   205,   217,   218,   219,   226,   213,   227,   235,   233,
     237,   236,   238,   203,   249,   143,   254,   147,   255,   240,
     243,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   256,   257,   258,   259,   264,   261,
     265,   210,   208,   209,   193,   266,   216,   268,   250,   224,
     185,   195,   196,   221,   197,   198,     0,    57,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   190,     0,
     230,   263,     0,     0,   247,   248,   267,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   239,     0,     0,     0,
     246,   244,   245,     0,     0,    31,    31,     0,     0,     0,
     253,   251,   252,     0,     0,     0,     0,     0,     0,   214,
     215,     0,     0,     0,   260,     0,     0,     0,     0,   262,
       0,   220,     0,     0,     0,     0,   143,     0,     0,   225,
       0,     0,     0,   228,   229,     1,     2,     3,     4,     5,
       0,     6,     7,     8,     9,    10,    11,    12,    13,    14,
       0,     0,     0,    15,     0,    16,     0,   -95,    17,    18,
      19,     0,    20,    21,     0,     0,     0,     0,     0,     0,
      22,     0,     0,    23,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,     0,     0,    26,    27,    51,    52,     3,     0,
       5,     0,     6,     7,     8,    38,    10,    11,    12,    13,
      14,     0,     0,     0,    15,     0,    16,     0,   -95,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     0,     0,
       0,    22,     0,     0,    23,    24,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    25,     0,     0,     0,    26,    27,     1,     2,     3,
       0,     5,     0,     6,     7,     8,     9,    10,    11,    12,
      13,    14,     0,     0,     0,    15,     0,    16,     0,   -95,
      17,    18,    19,     0,    20,    21,     0,     0,     0,     0,
       0,     0,    22,     0,     0,    23,    24,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    25,     0,     0,     0,    26,    27,     1,     2,
       3,     0,     5,     0,     6,     7,     8,    38,    10,    11,
      12,    13,    14,     0,     0,     0,    15,     0,    16,     0,
     -95,    17,    18,    19,     0,    20,    21,     0,     0,     0,
       0,     0,     0,    22,     0,     0,    23,    24,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     2,     3,     0,
       5,     0,     6,     7,     8,    38,    10,    11,    12,    13,
      14,     0,     0,    25,    15,     0,    16,    26,    27,    17,
      18,    19,     0,    20,    21,     0,     0,     0,     1,     2,
       3,    22,     0,     0,    23,    24,     0,    38,     0,     0,
       0,     0,    14,     0,     0,     0,    46,     0,    16,     0,
       0,    17,    18,    19,     0,    20,    21,   142,     0,     0,
       0,    25,     1,     2,     3,    26,    27,    24,     0,     0,
       0,    38,     0,     0,     0,     0,    14,     0,     0,     0,
      46,     0,    16,     0,     0,    17,    18,    19,     0,    20,
      21,     0,     0,    25,     0,     0,     0,    26,    27,     0,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    73,     0,
       0,    74,     0,     0,    75,     0,     0,    25,    76,    77,
      78,    26,    27,    79,    80,    81,    82,   187,   188,     0,
       0,     0,     0,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,    73,     0,     0,    74,     0,     0,    75,
       0,     0,     0,    76,    77,    78,     0,     0,    79,    80,
      81,    82,     0,     0,     0,     0,     0,     0,     0,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    73,     0,
       0,   135,     0,     0,    75,     0,     0,     0,    76,    77,
      78,     0,     0,    79,    80,    81,    82,     0,     0,     0,
       0,     0,     0,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,    73,     0,     0,     0,     0,     0,    75,
       0,   141,     0,    76,    77,    78,     0,     0,    79,    80,
      81,    82,     0,     0,     0,     0,     0,     0,     0,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    73,     0,
       0,     0,     0,     0,    75,   206,     0,     0,    76,    77,
      78,     0,     0,    79,    80,    81,    82,     0,     0,     0,
       0,     0,     0,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,    73,     0,   207,     0,     0,     0,    75,
       0,     0,     0,    76,    77,    78,     0,     0,    79,    80,
      81,    82,     0,     0,     0,     0,     0,     0,     0,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    73,   231,
       0,     0,     0,     0,    75,     0,     0,     0,    76,    77,
      78,     0,     0,    79,    80,    81,    82,     0,     0,     0,
       0,     0,     0,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,    73,   232,     0,     0,     0,     0,    75,
       0,     0,     0,    76,    77,    78,     0,     0,    79,    80,
      81,    82,     0,     0,     0,     0,     0,     0,     0,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    73,   241,
       0,     0,     0,     0,    75,     0,     0,     0,    76,    77,
      78,     0,     0,    79,    80,    81,    82,     0,     0,     0,
       0,     0,     0,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,    73,   242,     0,     0,     0,     0,    75,
       0,     0,     0,    76,    77,    78,     0,     0,    79,    80,
      81,    82,     0,     0,     0,     0,     0,     0,     0,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,    73,     0,
       0,     0,     0,     0,    75,     0,     0,     0,    76,    77,
      78,     0,     0,    79,    80,    81,    82,     0,     0,     0,
       0,     0,     0,     0,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,    73,     0,     0,     0,     0,     0,    75,
       0,     0,     0,    76,    77,    78,     0,     0,    79,    80,
      81,    82,    73,     0,     0,     0,     0,     0,    75,     0,
       0,     0,    76,    77,    78,     0,     0,    79,    80,    81,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,     0,     0,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,    73,     0,     0,
       0,     0,     0,    75,     0,     0,     0,    76,    77,    78,
       0,     0,    79,    80,    81,    73,     0,     0,     0,     0,
       0,    75,     0,     0,     0,    76,    77,    78,     0,     0,
      79,    80,     0,     0,     0,     0,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,     0,     0,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
      73,     0,     0,     0,     0,     0,    75,     0,     0,     0,
      76,    77,    78,     0,     0,    79,    73,     0,     0,     0,
       0,     0,    75,     0,    73,     0,    76,    77,    78,     0,
      75,     0,     0,     0,    76,    77,    78,     0,     0,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   104,   105,   106,   107,   108,   109,   110,   111
};

static const yytype_int16 yycheck[] =
{
       0,    10,   121,    53,    20,    17,    17,     7,    11,    30,
      31,    14,    23,    17,    64,    17,    27,    28,    29,    23,
      36,    37,    17,    27,    28,    29,     3,     4,     5,     4,
      30,    31,   117,    36,    37,    12,    13,    40,   123,     4,
      17,    25,     3,     4,    21,    25,    23,    39,    40,    26,
      27,    28,    21,    30,    31,    17,    67,    68,    69,    70,
      71,    23,    21,    17,     0,    42,     9,    29,    25,    23,
       4,    74,    17,    27,    28,    29,    17,    47,    16,    25,
      18,    19,    20,    21,    19,    23,    24,    25,    26,    27,
      20,    68,    25,    18,    22,    72,    73,    19,   217,    20,
      24,     4,    40,    22,     4,    18,    18,    17,   117,    25,
      47,   120,   121,    20,   123,    69,    70,    71,    22,    20,
      22,    18,    20,    18,    18,     8,    25,    18,    21,    25,
      17,    21,    21,   142,     4,    73,    18,    75,    22,    25,
      25,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,    22,    18,    22,    18,    22,    21,
      40,   184,   182,   183,   122,    21,   189,    22,   238,   204,
     117,   129,   130,   200,   133,   135,    -1,   135,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   217,    -1,
     213,   261,    -1,    -1,   235,   236,   266,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   226,    -1,    -1,    -1,
     233,   231,   232,    -1,    -1,   235,   236,    -1,    -1,    -1,
     243,   241,   242,    -1,    -1,    -1,    -1,    -1,    -1,   187,
     188,    -1,    -1,    -1,   254,    -1,    -1,    -1,    -1,   259,
      -1,   199,    -1,    -1,    -1,    -1,   204,    -1,    -1,   207,
      -1,    -1,    -1,   211,   212,     3,     4,     5,     6,     7,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      -1,    -1,    -1,    21,    -1,    23,    -1,    25,    26,    27,
      28,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    -1,    -1,    72,    73,     3,     4,     5,    -1,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    -1,    -1,    -1,    21,    -1,    23,    -1,    25,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    38,    -1,    -1,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    -1,    -1,    -1,    72,    73,     3,     4,     5,
      -1,     7,    -1,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    -1,    -1,    -1,    21,    -1,    23,    -1,    25,
      26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    38,    -1,    -1,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    68,    -1,    -1,    -1,    72,    73,     3,     4,
       5,    -1,     7,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    -1,    -1,    -1,    21,    -1,    23,    -1,
      25,    26,    27,    28,    -1,    30,    31,    -1,    -1,    -1,
      -1,    -1,    -1,    38,    -1,    -1,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,    -1,
       7,    -1,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    -1,    -1,    68,    21,    -1,    23,    72,    73,    26,
      27,    28,    -1,    30,    31,    -1,    -1,    -1,     3,     4,
       5,    38,    -1,    -1,    41,    42,    -1,    12,    -1,    -1,
      -1,    -1,    17,    -1,    -1,    -1,    21,    -1,    23,    -1,
      -1,    26,    27,    28,    -1,    30,    31,    32,    -1,    -1,
      -1,    68,     3,     4,     5,    72,    73,    42,    -1,    -1,
      -1,    12,    -1,    -1,    -1,    -1,    17,    -1,    -1,    -1,
      21,    -1,    23,    -1,    -1,    26,    27,    28,    -1,    30,
      31,    -1,    -1,    68,    -1,    -1,    -1,    72,    73,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    -1,
      -1,    20,    -1,    -1,    23,    -1,    -1,    68,    27,    28,
      29,    72,    73,    32,    33,    34,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    17,    -1,    -1,    20,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    17,    -1,
      -1,    20,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    17,    -1,    -1,    -1,    -1,    -1,    23,
      -1,    25,    -1,    27,    28,    29,    -1,    -1,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    17,    -1,
      -1,    -1,    -1,    -1,    23,    24,    -1,    -1,    27,    28,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    17,    -1,    19,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    17,    18,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    17,    18,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    17,    18,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    17,    18,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,
      34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    17,    -1,
      -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    17,    -1,    -1,    -1,    -1,    -1,    23,
      -1,    -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,
      34,    35,    17,    -1,    -1,    -1,    -1,    -1,    23,    -1,
      -1,    -1,    27,    28,    29,    -1,    -1,    32,    33,    34,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    -1,    -1,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    17,    -1,    -1,
      -1,    -1,    -1,    23,    -1,    -1,    -1,    27,    28,    29,
      -1,    -1,    32,    33,    34,    17,    -1,    -1,    -1,    -1,
      -1,    23,    -1,    -1,    -1,    27,    28,    29,    -1,    -1,
      32,    33,    -1,    -1,    -1,    -1,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    -1,    -1,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      17,    -1,    -1,    -1,    -1,    -1,    23,    -1,    -1,    -1,
      27,    28,    29,    -1,    -1,    32,    17,    -1,    -1,    -1,
      -1,    -1,    23,    -1,    17,    -1,    27,    28,    29,    -1,
      23,    -1,    -1,    -1,    27,    28,    29,    -1,    -1,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    64,    65,    66,    67,    68,    69,    70,    71
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    21,    23,    26,    27,    28,
      30,    31,    38,    41,    42,    68,    72,    73,    76,    77,
      78,    80,    82,    87,    88,    91,    17,    17,    12,    80,
      17,     4,    79,    82,    83,    84,    21,    88,    25,    25,
      88,     3,     4,    80,    81,    92,    94,    87,    93,    21,
      87,    87,    87,    87,    21,    87,    87,    87,    87,    87,
       0,    77,    77,    17,    20,    23,    27,    28,    29,    32,
      33,    34,    35,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    25,    88,    88,     4,     9,    13,    87,    88,
      17,    17,    47,    20,    25,     3,     4,    25,    18,    19,
      19,    81,    22,    20,    22,    20,    24,     4,    85,    86,
      81,    25,    32,    87,    89,    90,    88,    87,     4,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    87,    87,    87,    87,    87,    87,    87,    87,
      87,    87,    18,    18,    17,    83,    84,    36,    37,    25,
      82,    95,    95,    87,    84,    87,    87,    94,    93,    47,
      20,    22,    22,    82,    20,    18,    24,    19,    80,    80,
      88,    36,    37,    25,    87,    87,    88,    20,    18,    18,
      87,    86,    39,    40,    90,    87,     8,    18,    87,    87,
      88,    18,    18,    25,    95,    21,    21,    17,    21,    80,
      25,    18,    18,    25,    80,    80,    88,    77,    77,     4,
      81,    80,    80,    88,    18,    22,    22,    18,    22,    18,
      80,    21,    80,    81,    22,    40,    21,    81,    22
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
      87,    87,    87,    87,    87,    88,    88,    88,    89,    89,
      90,    90,    90,    91,    91,    92,    92,    93,    93,    93,
      94,    94,    94,    95,    95,    95
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
       2,     1,     3,     4,     1,     0,     1,     3,     2,     1,
       0,     1,     3,     3,     3,     3,     3,     0,     1,     3,
       0,     1,     3,     0,     1,     3
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
#line 109 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("script -> EOS\n");
			pd->beginning = 0;
			YYACCEPT;
		}
#line 1712 "js.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 115 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("script -> pgmlist\n");
			pd->beginning = (yyvsp[0].slot);
			YYACCEPT;
		}
#line 1722 "js.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 124 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("pgmlist -> _empty_\n");
		}
#line 1731 "js.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 129 "js.y" /* yacc.c:1646  */
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
#line 1753 "js.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 147 "js.y" /* yacc.c:1646  */
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
#line 1770 "js.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 163 "js.y" /* yacc.c:1646  */
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
#line 1792 "js.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 184 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("fname -> _empty_\n");
			(yyval.slot) = 0;
		}
#line 1801 "js.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 189 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 1;
		}
#line 1809 "js.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 196 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-2].slot);
			ifthen->thenstmt = (yyvsp[0].slot);
			ifthen->elsestmt = 0;

			if (debug) printf("stmt -> IF LPAR exprlist RPAR stmt %d\n", (yyval.slot));
		}
#line 1823 "js.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 206 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-4].slot);
			ifthen->thenstmt = (yyvsp[-2].slot);
			ifthen->elsestmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> IF LPAR exprlist RPAR stmt ELSE stmt %d\n", (yyval.slot));
		}
#line 1837 "js.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 216 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_throw, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("stmt -> THROW expr SEMI %d\n", (yyval.slot));
		}
#line 1849 "js.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 224 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("stmt -> RETURN exprlist SEMI %d\n", (yyval.slot));
		}
#line 1861 "js.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 232 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= flag_break;
			en->expr = 0;

			if (debug) printf("stmt -> BREAK SEMI %d\n", (yyval.slot));
		}
#line 1874 "js.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 241 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= flag_continue;
			en->expr = 0;

			if (debug) printf("stmt -> CONTINUE SEMI %d\n", (yyval.slot));
		}
#line 1887 "js.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 250 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> WHILE LPAR exprlist RPAR stmt %d\n", (yyval.slot));
		}
#line 1900 "js.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 259 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[-5].slot);

			if (debug) printf("stmt -> DO stmt WHILE LPAR exprlist RPAR SEMI %d\n", (yyval.slot));
		}
#line 1913 "js.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 268 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decllist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1928 "js.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 279 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR exprlist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1943 "js.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 290 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decl FORIN expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1958 "js.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 301 "js.y" /* yacc.c:1646  */
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
#line 1975 "js.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 314 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decl FOROF expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1990 "js.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 325 "js.y" /* yacc.c:1646  */
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
#line 2007 "js.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 338 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-1].slot);

			if (debug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2019 "js.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 346 "js.y" /* yacc.c:1646  */
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
#line 2037 "js.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 360 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + (yyval.slot));
			xn->tryblk = (yyvsp[-5].slot);
			xn->finallyblk = (yyvsp[-1].slot);

			if (debug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", (yyvsp[-5].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2050 "js.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 369 "js.y" /* yacc.c:1646  */
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
#line 2069 "js.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 384 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> LBRACE stmtlist RBRACE\n");
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 2078 "js.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 389 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> VAR decllist SEMI %d\n", (yyvsp[-1].slot));
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 2087 "js.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 394 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> exprlist[%d] SEMI\n", (yyvsp[-1].slot));
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 2096 "js.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 402 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("stmtlist -> _empty_\n");
		}
#line 2105 "js.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 407 "js.y" /* yacc.c:1646  */
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
#line 2127 "js.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 428 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + (yyval.slot));
			sym->name = (yyvsp[0].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("symbol -> NAME[%s] %d\n", sn->string, (yyval.slot));
			}
		}
#line 2142 "js.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 442 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl | flag_lval;
			(yyval.slot) = (yyvsp[0].slot);

			if (debug) printf("decl -> symbol[%d]\n", (yyvsp[0].slot));
		}
#line 2154 "js.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 450 "js.y" /* yacc.c:1646  */
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
#line 2171 "js.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 466 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("decllist -> decl[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2183 "js.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 475 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (debug) printf("decllist -> decl[%d] COMMA decllist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2195 "js.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 486 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->left = (yyvsp[0].slot);
			bn->right = 0;

			if (debug) printf("enum -> NAME %d\n", (yyval.slot));
		}
#line 2208 "js.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 496 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("enum -> NAME ASSIGN expr %d\n", (yyval.slot));
		}
#line 2221 "js.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 508 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("enumlist -> enum[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2233 "js.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 517 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (debug) printf("enumlist -> enum[%d] COMMA enumlist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2245 "js.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 528 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ternary, sizeof(ternaryNode), true);
			ternaryNode *tn = (ternaryNode *)(pd->table + (yyval.slot));
			tn->condexpr = (yyvsp[-4].slot);
			tn->trueexpr = (yyvsp[-2].slot);
			tn->falseexpr = (yyvsp[0].slot);

			if (debug) printf("expr -> expr[%d] TERN expr[%d] COLON expr[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2259 "js.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 539 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("expr -> ENUM LBRACE enumlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2271 "js.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 547 "js.y" /* yacc.c:1646  */
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
#line 2294 "js.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 567 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> TYPEOF expr %d\n", (yyval.slot));
		}
#line 2306 "js.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 575 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> INCR expr %d\n", (yyval.slot));
		}
#line 2322 "js.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 588 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> DECR expr %d\n", (yyval.slot));
		}
#line 2338 "js.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 600 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", (yyval.slot));
		}
#line 2354 "js.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 613 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", (yyval.slot));
		}
#line 2370 "js.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 626 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_rushift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT expr %d\n", (yyval.slot));
		}
#line 2384 "js.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 636 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT expr %d\n", (yyval.slot));
		}
#line 2398 "js.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 646 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LSHIFT expr %d\n", (yyval.slot));
		}
#line 2412 "js.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 656 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lor, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LOR expr %d\n", (yyval.slot));
		}
#line 2425 "js.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 665 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_land, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LAND expr %d\n", (yyval.slot));
		}
#line 2438 "js.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 674 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_lt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LT expr %d\n", (yyval.slot));
		}
#line 2452 "js.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 684 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_le;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LE expr %d\n", (yyval.slot));
		}
#line 2466 "js.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 694 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_eq;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr EQ expr %d\n", (yyval.slot));
		}
#line 2480 "js.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 704 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_ne;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr NEQ expr %d\n", (yyval.slot));
		}
#line 2494 "js.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 714 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_id;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr IDENTICAL expr %d\n", (yyval.slot));
		}
#line 2508 "js.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 724 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_nid;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr NOTIDENTICAL expr %d\n", (yyval.slot));
		}
#line 2522 "js.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 734 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_ge;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr GE expr %d\n", (yyval.slot));
		}
#line 2536 "js.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 744 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_gt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr GT expr %d\n", (yyval.slot));
		}
#line 2550 "js.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 754 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr PLUS expr %d\n", (yyval.slot));
		}
#line 2564 "js.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 764 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MINUS expr %d\n", (yyval.slot));
		}
#line 2578 "js.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 774 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MPY expr %d\n", (yyval.slot));
		}
#line 2592 "js.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 784 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MOD expr %d\n", (yyval.slot));
		}
#line 2606 "js.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 794 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr DIV expr %d\n", (yyval.slot));
		}
#line 2620 "js.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 804 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITAND expr %d\n", (yyval.slot));
		}
#line 2634 "js.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 814 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITXOR expr %d\n", (yyval.slot));
		}
#line 2648 "js.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 824 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITOR expr %d\n", (yyval.slot));
		}
#line 2662 "js.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 834 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_uminus;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> UMINUS expr %d\n", (yyval.slot));
		}
#line 2675 "js.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 843 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_not;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> NOT expr %d\n", (yyval.slot));
		}
#line 2688 "js.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 852 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_bitnot;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> BITNOT expr %d\n", (yyval.slot));
		}
#line 2701 "js.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 861 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);
			if (debug) printf("expr -> LPAR exprlist[%d] RPAR\n", (yyvsp[-1].slot));
		}
#line 2710 "js.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 866 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2726 "js.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 878 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LSHIFT_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2742 "js.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 890 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2758 "js.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 902 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr PLUS_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2774 "js.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 914 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MINUS_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2790 "js.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 926 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MPY_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2806 "js.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 938 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MOD_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2822 "js.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 950 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr DIV_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2838 "js.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 962 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr AND_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2854 "js.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 974 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr OR_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2870 "js.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 986 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr XOR_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2886 "js.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 998 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			pd->table[(yyvsp[0].slot)].flag = flag_newobj;

			if (debug) printf("expr -> NEW expr %d\n", (yyval.slot));
		}
#line 2897 "js.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1005 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + (yyval.slot));

			fc->name = (yyvsp[-3].slot);
			fc->args = (yyvsp[-1].slot);

			if (debug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", (yyvsp[-3].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2911 "js.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 1015 "js.y" /* yacc.c:1646  */
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
#line 2954 "js.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 1054 "js.y" /* yacc.c:1646  */
    {
			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("expr -> STRING[%s] %d\n", sn->string, (yyvsp[0].slot));
			}
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2966 "js.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1062 "js.y" /* yacc.c:1646  */
    {
			exprNode *en = (exprNode *)(pd->table + (yyvsp[0].slot));
			en->hdr->flag |= flag_delete;

			if (debug) printf("expr -> DEL expr[%d]\n", (yyvsp[0].slot));
		}
#line 2977 "js.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1069 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("expr -> objarraylit\n");
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2986 "js.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1075 "js.y" /* yacc.c:1646  */
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
#line 3002 "js.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1087 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[-1].slot);
			bn->left = (yyvsp[-3].slot);

			if (debug) printf("expr -> expr LBRACK expr RBRACK %d\n", (yyval.slot));
		}
#line 3015 "js.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1096 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 3023 "js.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1103 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("exprlist -> _empty_\n");
		}
#line 3032 "js.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1108 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("exprlist -> expr[%d]\n", (yyvsp[0].slot));
		}
#line 3041 "js.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1113 "js.y" /* yacc.c:1646  */
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
#line 3065 "js.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1136 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->type = node_ref;
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("arg -> BITAND symbol\n");
		}
#line 3076 "js.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 1143 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("arg -> expr[%d]\n", (yyvsp[0].slot));
		}
#line 3085 "js.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1151 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("arglist -> _empty_\n");
		}
#line 3094 "js.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 1156 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug)
				printf("arglist -> arg[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3107 "js.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 1165 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if ((yyvsp[0].slot)) {
			  if (debug)
				printf("arglist -> arg[%d] COMMA arglist %d\n", (yyvsp[-2].slot), (yyval.slot));
			} else {
			  ln->hdr->type = node_endlist;
			  if (debug)
				printf("arglist -> arg[%d] %d\n", (yyvsp[-2].slot), (yyval.slot));
			}
		}
#line 3126 "js.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 1183 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + (yyval.slot));
			on->elemlist = (yyvsp[-1].slot);

			if (debug) printf("objarraylit -> LBRACE elemlist RBRACE %d\n", (yyval.slot));
		}
#line 3138 "js.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 1192 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + (yyval.slot));
			an->exprlist = (yyvsp[-1].slot);

			if (debug) printf("objarraylit -> LBRACK arraylist[%d] RBRACK %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 3150 "js.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1203 "js.y" /* yacc.c:1646  */
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
#line 3166 "js.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1215 "js.y" /* yacc.c:1646  */
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
#line 3182 "js.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1230 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("arraylist -> _empty_\n");
		}
#line 3191 "js.tab.c" /* yacc.c:1646  */
    break;

  case 108:
#line 1235 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("arraylist -> expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3203 "js.tab.c" /* yacc.c:1646  */
    break;

  case 109:
#line 1243 "js.y" /* yacc.c:1646  */
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
#line 3220 "js.tab.c" /* yacc.c:1646  */
    break;

  case 110:
#line 1259 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("elemlist -> _empty_\n");
		}
#line 3229 "js.tab.c" /* yacc.c:1646  */
    break;

  case 111:
#line 1264 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("elemlist -> elem[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3241 "js.tab.c" /* yacc.c:1646  */
    break;

  case 112:
#line 1272 "js.y" /* yacc.c:1646  */
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
#line 3258 "js.tab.c" /* yacc.c:1646  */
    break;

  case 113:
#line 1288 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("paramlist -> _empty_\n");
		}
#line 3267 "js.tab.c" /* yacc.c:1646  */
    break;

  case 114:
#line 1293 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl;

			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("paramlist -> symbol[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3282 "js.tab.c" /* yacc.c:1646  */
    break;

  case 115:
#line 1304 "js.y" /* yacc.c:1646  */
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
#line 3302 "js.tab.c" /* yacc.c:1646  */
    break;


#line 3306 "js.tab.c" /* yacc.c:1646  */
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
#line 1320 "js.y" /* yacc.c:1906  */


void yyerror( void *scanner, parseData *pd, const char *s)
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineNo, s);
}

