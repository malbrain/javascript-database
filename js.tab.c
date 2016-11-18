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
    NEW = 269,
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
    PLUS_ASSIGN = 294,
    MINUS_ASSIGN = 295,
    LSHIFT_ASSIGN = 296,
    RSHIFT_ASSIGN = 297,
    ASSIGN = 298,
    MPY_ASSIGN = 299,
    DIV_ASSIGN = 300,
    MOD_ASSIGN = 301,
    AND_ASSIGN = 302,
    XOR_ASSIGN = 303,
    OR_ASSIGN = 304,
    LOR = 305,
    LAND = 306,
    LT = 307,
    LE = 308,
    EQ = 309,
    NEQ = 310,
    GT = 311,
    GE = 312,
    LSHIFT = 313,
    RSHIFT = 314,
    RUSHIFT = 315,
    PLUS = 316,
    MINUS = 317,
    MPY = 318,
    DIV = 319,
    MOD = 320,
    TYPEOF = 321,
    UMINUS = 322
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

#line 191 "js.tab.c" /* yacc.c:355  */
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

#line 210 "js.tab.c" /* yacc.c:358  */

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
#define YYFINAL  64
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1213

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  107
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  244

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   322

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
      65,    66,    67
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   102,   102,   108,   117,   122,   140,   156,   177,   182,
     189,   199,   209,   217,   226,   235,   244,   253,   264,   275,
     286,   299,   310,   323,   328,   333,   341,   346,   367,   381,
     389,   405,   414,   425,   435,   447,   456,   467,   478,   486,
     506,   514,   527,   539,   552,   565,   575,   585,   595,   604,
     613,   623,   633,   643,   653,   663,   673,   683,   693,   703,
     713,   723,   733,   743,   753,   762,   771,   780,   785,   797,
     809,   821,   833,   845,   857,   869,   881,   893,   905,   917,
     926,   965,   973,   983,   987,   999,  1008,  1016,  1021,  1026,
    1049,  1056,  1064,  1069,  1078,  1096,  1105,  1116,  1128,  1143,
    1148,  1156,  1172,  1177,  1185,  1201,  1206,  1217
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "NAME", "NUM", "EOS", "IF",
  "ELSE", "WHILE", "DO", "FOR", "FCN", "VAR", "NEW", "RETURN", "CONTINUE",
  "BREAK", "LPAR", "RPAR", "COLON", "COMMA", "LBRACE", "RBRACE", "LBRACK",
  "RBRACK", "SEMI", "ENUM", "INCR", "DECR", "DOT", "NOT", "BITNOT",
  "BITAND", "BITXOR", "BITOR", "TERN", "FORIN", "FOROF", "PLUS_ASSIGN",
  "MINUS_ASSIGN", "LSHIFT_ASSIGN", "RSHIFT_ASSIGN", "ASSIGN", "MPY_ASSIGN",
  "DIV_ASSIGN", "MOD_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN",
  "LOR", "LAND", "LT", "LE", "EQ", "NEQ", "GT", "GE", "LSHIFT", "RSHIFT",
  "RUSHIFT", "PLUS", "MINUS", "MPY", "DIV", "MOD", "TYPEOF", "UMINUS",
  "$accept", "script", "pgmlist", "funcdef", "fname", "stmt", "stmtlist",
  "symbol", "decl", "decllist", "enum", "enumlist", "expr", "exprlist",
  "arg", "arglist", "objarraylit", "elem", "arraylist", "elemlist",
  "paramlist", YY_NULL
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
     315,   316,   317,   318,   319,   320,   321,   322
};
# endif

#define YYPACT_NINF -110

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-110)))

#define YYTABLE_NINF -88

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      24,  -110,  -110,  -110,  -110,    -3,    -1,   351,     0,    15,
      16,    16,   447,    -2,     6,   447,   255,   447,     3,   447,
     447,   447,   447,   447,   447,    21,  -110,   287,   287,  -110,
     530,    19,  -110,   447,   447,    43,    45,   383,    31,    44,
      18,    42,    39,    48,     8,    41,  -110,  -110,    52,    54,
      61,   319,    49,    62,    59,   578,    60,    83,   -15,   -15,
      40,    40,    40,    40,  -110,  -110,  -110,   415,   447,   447,
    -110,  -110,    84,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,   447,   447,   447,   447,   447,   447,
     447,   447,   447,   447,  -110,    70,    72,  -110,    74,    16,
     482,    68,    16,    16,   447,    16,  -110,   415,    54,    61,
    -110,  -110,   447,   447,  -110,  -110,     8,  -110,   447,  -110,
      53,    76,    75,    16,   914,    78,    82,  -110,   626,  -110,
    1134,  1086,  1072,   674,   914,   914,   914,   914,   914,   914,
     914,   914,   914,   914,   914,  1010,  1024,   164,   164,   164,
     164,   164,   164,  1141,  1141,  1141,  1148,  1148,    40,    40,
      40,   351,   351,   447,    22,    69,   447,   447,   447,    81,
      86,    87,   914,  -110,    90,   914,   914,  -110,  -110,   447,
      83,  -110,  -110,   415,  -110,  -110,   447,    95,  -110,    92,
     447,   447,   447,   722,   770,    88,    16,    91,    93,  -110,
     914,  -110,  -110,   962,   351,    94,   818,   866,    96,   351,
     351,   447,  -110,   287,   287,  -110,  -110,   351,   351,   447,
    -110,  -110,    97,    89,    98,  -110,  -110,    99,   351,  -110,
    -110,   351,  -110,  -110
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,    81,    28,    80,     2,     0,     0,    87,     0,     8,
       0,     0,    87,     0,     0,    87,    26,    99,     0,     0,
       0,     0,     0,     0,     0,     0,     3,     4,     4,    83,
      88,     0,    86,    87,    87,     8,     0,    87,     0,     0,
      29,    31,     0,     0,   102,     0,    14,    13,     0,    81,
      28,    26,     0,   103,     0,   100,     0,     0,    41,    42,
      65,    66,    64,    40,     1,     6,     5,    92,    87,     0,
      43,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,     0,     0,     9,     0,     0,
      88,     0,   105,   105,     0,     0,    24,    92,     0,     0,
      12,    67,     0,     0,    27,    23,   102,    95,    99,    96,
      33,    35,     0,     0,    91,    93,     0,    89,     0,    84,
      61,    62,    63,     0,    71,    72,    69,    70,    68,    73,
      75,    74,    76,    78,    77,    48,    49,    50,    51,    52,
      53,    55,    54,    47,    46,    45,    56,    57,    58,    60,
      59,    87,    87,    87,    31,     0,     0,     0,    87,   106,
       0,     0,    30,    32,     0,    98,    97,   104,   101,     0,
       0,    38,    90,    92,    79,    85,     0,    10,    15,     0,
       0,     0,    87,     0,     0,     0,   105,     0,     0,    82,
      34,    36,    94,    37,    87,     0,     0,     0,     0,    87,
      87,    87,   107,     4,     4,    11,    16,    87,    87,    87,
      20,    22,     0,     0,     0,    19,    21,     0,    87,     7,
      39,    87,    18,    17
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -110,  -110,   -25,  -110,  -110,    -7,    66,    -5,    10,   -99,
    -110,   -66,    56,   -11,  -110,  -109,  -110,  -110,    32,     1,
    -106
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    25,    26,    27,    39,    28,    52,    29,    41,    42,
     131,   132,    30,    31,   135,   136,    32,    53,    56,    54,
     180
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      36,    45,    65,    66,    48,    40,    43,   181,   184,    51,
     175,   118,   119,    70,    71,    33,   183,    34,    37,    38,
       2,    64,   105,   106,    46,    57,   111,     1,     2,     3,
       4,     5,    47,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,   115,    51,   104,    16,   107,    17,   112,
     -87,    18,    19,    20,   108,    21,    22,   137,    67,   200,
     201,   114,   113,   115,    69,   116,   117,   120,    70,    71,
      72,   121,   125,    55,   122,    58,    59,    60,    61,    62,
      63,   123,   127,   126,   212,   129,    23,   130,   139,   171,
      24,   172,   173,   110,   178,   202,   189,   190,   191,   193,
     222,   194,   206,   214,    40,   207,   208,   179,   179,   209,
      40,   215,   239,   223,   221,   224,   238,   124,   241,   174,
     226,   240,   229,   134,   211,   138,     0,   187,   192,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     188,     0,   199,     0,   197,   198,     0,   205,     0,     0,
     182,     0,     0,   134,     0,     0,     0,     0,   185,   186,
       0,     0,    67,     0,    55,     0,     0,     0,    69,     0,
       0,   218,    70,    71,    72,     0,     0,     0,   233,   234,
       0,   179,     0,     0,     0,     0,     0,   225,     0,     0,
     232,     0,   230,   231,     0,     0,     0,     0,   237,     0,
     235,   236,    96,    97,    98,    99,   100,   101,   102,   103,
       0,   242,   203,   204,   243,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   210,     0,     0,     0,   134,
       0,     0,   213,     0,     0,     0,   216,   217,    49,    50,
       3,     0,     5,     0,     6,     7,     8,    35,    10,    11,
      12,    13,    14,    15,     0,     0,     0,    16,     0,    17,
       0,   -87,    18,    19,    20,     0,    21,    22,     0,     0,
       1,     2,     3,     0,     5,     0,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,     0,     0,     0,    16,
       0,    17,     0,   -87,    18,    19,    20,    23,    21,    22,
       0,    24,     1,     2,     3,     0,     5,     0,     6,     7,
       8,    35,    10,    11,    12,    13,    14,    15,     0,     0,
       0,    16,     0,    17,     0,   -87,    18,    19,    20,    23,
      21,    22,     0,    24,     1,     2,     3,     0,     5,     0,
       6,     7,     8,    35,    10,    11,    12,    13,    14,    15,
       0,     0,     0,    16,     0,    17,     0,     0,    18,    19,
      20,    23,    21,    22,     0,    24,     1,     2,     3,     0,
       0,     0,     0,     0,     0,    35,   109,    11,     0,     0,
       0,    15,     0,     0,     0,    44,     0,    17,     0,     0,
      18,    19,    20,    23,    21,    22,     0,    24,     1,     2,
       3,     0,     0,     0,     0,     0,     0,    35,     0,    11,
       0,     0,     0,    15,     0,     0,     0,    44,     0,    17,
       0,     0,    18,    19,    20,    23,    21,    22,   133,    24,
       1,     2,     3,     0,     0,     0,     0,     0,     0,    35,
       0,    11,     0,     0,     0,    15,     0,     0,     0,    44,
       0,    17,     0,     0,    18,    19,    20,    23,    21,    22,
       0,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      67,     0,     0,    68,     0,     0,    69,     0,     0,    23,
      70,    71,    72,    24,     0,    73,    74,    75,    76,   176,
     177,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    67,     0,
       0,    68,     0,     0,    69,     0,     0,     0,    70,    71,
      72,     0,     0,    73,    74,    75,    76,     0,     0,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    67,     0,     0,   128,
       0,     0,    69,     0,     0,     0,    70,    71,    72,     0,
       0,    73,    74,    75,    76,     0,     0,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    67,     0,     0,     0,     0,     0,
      69,   195,     0,     0,    70,    71,    72,     0,     0,    73,
      74,    75,    76,     0,     0,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    67,     0,   196,     0,     0,     0,    69,     0,
       0,     0,    70,    71,    72,     0,     0,    73,    74,    75,
      76,     0,     0,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
      67,   219,     0,     0,     0,     0,    69,     0,     0,     0,
      70,    71,    72,     0,     0,    73,    74,    75,    76,     0,
       0,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    67,   220,
       0,     0,     0,     0,    69,     0,     0,     0,    70,    71,
      72,     0,     0,    73,    74,    75,    76,     0,     0,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    67,   227,     0,     0,
       0,     0,    69,     0,     0,     0,    70,    71,    72,     0,
       0,    73,    74,    75,    76,     0,     0,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    67,   228,     0,     0,     0,     0,
      69,     0,     0,     0,    70,    71,    72,     0,     0,    73,
      74,    75,    76,     0,     0,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    67,     0,     0,     0,     0,     0,    69,     0,
       0,     0,    70,    71,    72,     0,     0,    73,    74,    75,
      76,     0,     0,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
      67,     0,     0,     0,     0,     0,    69,     0,     0,     0,
      70,    71,    72,     0,     0,    73,    74,    75,    76,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    67,     0,
       0,     0,     0,     0,    69,     0,     0,     0,    70,    71,
      72,     0,    67,    73,    74,    75,     0,     0,    69,     0,
       0,     0,    70,    71,    72,     0,     0,    73,    74,    75,
       0,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
      67,     0,     0,     0,     0,     0,    69,     0,     0,     0,
      70,    71,    72,     0,    67,    73,    74,     0,     0,     0,
      69,     0,     0,     0,    70,    71,    72,     0,     0,    73,
       0,     0,     0,     0,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    67,     0,     0,     0,     0,     0,    69,    67,
       0,     0,    70,    71,    72,    69,    67,     0,     0,    70,
      71,    72,    69,     0,     0,     0,    70,    71,    72,     0,
       0,     0,     0,     0,     0,     0,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
       0,     0,    99,   100,   101,   102,   103,     0,     0,     0,
       0,   101,   102,   103
};

static const yytype_int16 yycheck[] =
{
       7,    12,    27,    28,    15,    10,    11,   113,   117,    16,
     109,     3,     4,    28,    29,    18,   115,    18,    18,     4,
       4,     0,    33,    34,    26,    22,    37,     3,     4,     5,
       6,     7,    26,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    21,    51,    26,    22,     4,    24,    18,
      26,    27,    28,    29,     9,    31,    32,    68,    18,    37,
      38,    43,    18,    21,    24,    26,    18,    26,    28,    29,
      30,    19,    23,    17,    20,    19,    20,    21,    22,    23,
      24,    20,    23,    21,   193,    25,    62,     4,     4,    19,
      66,    19,    18,    37,    26,    26,    43,    21,    23,    21,
     206,    19,    21,     8,   109,    19,    19,   112,   113,    19,
     115,    19,    23,    22,    26,    22,    19,    51,    19,   109,
      26,    23,    26,    67,   190,    69,    -1,   126,   133,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     128,    -1,   173,    -1,   171,   172,    -1,   178,    -1,    -1,
     114,    -1,    -1,   117,    -1,    -1,    -1,    -1,   122,   123,
      -1,    -1,    18,    -1,   128,    -1,    -1,    -1,    24,    -1,
      -1,   202,    28,    29,    30,    -1,    -1,    -1,   223,   224,
      -1,   206,    -1,    -1,    -1,    -1,    -1,   214,    -1,    -1,
     221,    -1,   219,   220,    -1,    -1,    -1,    -1,   229,    -1,
     227,   228,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,   238,   176,   177,   241,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   189,    -1,    -1,    -1,   193,
      -1,    -1,   196,    -1,    -1,    -1,   200,   201,     3,     4,
       5,    -1,     7,    -1,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    -1,    -1,    22,    -1,    24,
      -1,    26,    27,    28,    29,    -1,    31,    32,    -1,    -1,
       3,     4,     5,    -1,     7,    -1,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    -1,    22,
      -1,    24,    -1,    26,    27,    28,    29,    62,    31,    32,
      -1,    66,     3,     4,     5,    -1,     7,    -1,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      -1,    22,    -1,    24,    -1,    26,    27,    28,    29,    62,
      31,    32,    -1,    66,     3,     4,     5,    -1,     7,    -1,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    -1,    22,    -1,    24,    -1,    -1,    27,    28,
      29,    62,    31,    32,    -1,    66,     3,     4,     5,    -1,
      -1,    -1,    -1,    -1,    -1,    12,    13,    14,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    22,    -1,    24,    -1,    -1,
      27,    28,    29,    62,    31,    32,    -1,    66,     3,     4,
       5,    -1,    -1,    -1,    -1,    -1,    -1,    12,    -1,    14,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    22,    -1,    24,
      -1,    -1,    27,    28,    29,    62,    31,    32,    33,    66,
       3,     4,     5,    -1,    -1,    -1,    -1,    -1,    -1,    12,
      -1,    14,    -1,    -1,    -1,    18,    -1,    -1,    -1,    22,
      -1,    24,    -1,    -1,    27,    28,    29,    62,    31,    32,
      -1,    66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      18,    -1,    -1,    21,    -1,    -1,    24,    -1,    -1,    62,
      28,    29,    30,    66,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    18,    -1,
      -1,    21,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    18,    -1,    -1,    21,
      -1,    -1,    24,    -1,    -1,    -1,    28,    29,    30,    -1,
      -1,    33,    34,    35,    36,    -1,    -1,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    18,    -1,    -1,    -1,    -1,    -1,
      24,    25,    -1,    -1,    28,    29,    30,    -1,    -1,    33,
      34,    35,    36,    -1,    -1,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    18,    -1,    20,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,    35,
      36,    -1,    -1,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      18,    19,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,
      28,    29,    30,    -1,    -1,    33,    34,    35,    36,    -1,
      -1,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    18,    19,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    -1,    33,    34,    35,    36,    -1,    -1,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    18,    19,    -1,    -1,
      -1,    -1,    24,    -1,    -1,    -1,    28,    29,    30,    -1,
      -1,    33,    34,    35,    36,    -1,    -1,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    18,    19,    -1,    -1,    -1,    -1,
      24,    -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,
      34,    35,    36,    -1,    -1,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,
      -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,    35,
      36,    -1,    -1,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,
      28,    29,    30,    -1,    -1,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    18,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,    28,    29,
      30,    -1,    18,    33,    34,    35,    -1,    -1,    24,    -1,
      -1,    -1,    28,    29,    30,    -1,    -1,    33,    34,    35,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,    -1,
      28,    29,    30,    -1,    18,    33,    34,    -1,    -1,    -1,
      24,    -1,    -1,    -1,    28,    29,    30,    -1,    -1,    33,
      -1,    -1,    -1,    -1,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    18,    -1,    -1,    -1,    -1,    -1,    24,    18,
      -1,    -1,    28,    29,    30,    24,    18,    -1,    -1,    28,
      29,    30,    24,    -1,    -1,    -1,    28,    29,    30,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    -1,    61,    62,    63,    64,    65,    -1,    -1,    -1,
      -1,    63,    64,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    22,    24,    27,    28,
      29,    31,    32,    62,    66,    69,    70,    71,    73,    75,
      80,    81,    84,    18,    18,    12,    73,    18,     4,    72,
      75,    76,    77,    75,    22,    81,    26,    26,    81,     3,
       4,    73,    74,    85,    87,    80,    86,    22,    80,    80,
      80,    80,    80,    80,     0,    70,    70,    18,    21,    24,
      28,    29,    30,    33,    34,    35,    36,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    26,    81,    81,     4,     9,    13,
      80,    81,    18,    18,    43,    21,    26,    18,     3,     4,
      26,    19,    20,    20,    74,    23,    21,    23,    21,    25,
       4,    78,    79,    33,    80,    82,    83,    81,    80,     4,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    19,    19,    18,    76,    77,    37,    38,    26,    75,
      88,    88,    80,    77,    83,    80,    80,    87,    86,    43,
      21,    23,    75,    21,    19,    25,    20,    73,    73,    81,
      37,    38,    26,    80,    80,    81,    21,    19,    19,    19,
      80,    79,    83,    80,     8,    19,    80,    80,    81,    19,
      19,    26,    88,    22,    22,    73,    26,    19,    19,    26,
      73,    73,    81,    70,    70,    73,    73,    81,    19,    23,
      23,    19,    73,    73
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    68,    69,    69,    70,    70,    70,    71,    72,    72,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    74,    74,    75,    76,
      76,    77,    77,    78,    78,    79,    79,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    81,    81,    81,
      82,    82,    83,    83,    83,    84,    84,    85,    85,    86,
      86,    86,    87,    87,    87,    88,    88,    88
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     0,     2,     2,     8,     0,     1,
       5,     7,     3,     2,     2,     5,     7,    10,     9,     8,
       7,     8,     7,     3,     3,     2,     0,     2,     1,     1,
       3,     1,     3,     1,     3,     1,     3,     5,     4,     8,
       2,     2,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     4,
       1,     1,     5,     1,     3,     4,     1,     0,     1,     3,
       2,     1,     0,     1,     3,     3,     3,     3,     3,     0,
       1,     3,     0,     1,     3,     0,     1,     3
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
#line 103 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("script -> EOS\n");
			pd->beginning = 0;
			YYACCEPT;
		}
#line 1640 "js.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 109 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("script -> pgmlist\n");
			pd->beginning = (yyvsp[0].slot);
			YYACCEPT;
		}
#line 1650 "js.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 118 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("pgmlist -> _empty_\n");
		}
#line 1659 "js.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 123 "js.y" /* yacc.c:1646  */
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
#line 1681 "js.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 141 "js.y" /* yacc.c:1646  */
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
#line 1698 "js.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 157 "js.y" /* yacc.c:1646  */
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
#line 1720 "js.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 178 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("fname -> _empty_\n");
			(yyval.slot) = 0;
		}
#line 1729 "js.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 183 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 1;
		}
#line 1737 "js.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 190 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-2].slot);
			ifthen->thenstmt = (yyvsp[0].slot);
			ifthen->elsestmt = 0;

			if (debug) printf("stmt -> IF LPAR exprlist RPAR stmt %d\n", (yyval.slot));
		}
#line 1751 "js.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 200 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + (yyval.slot));
			ifthen->condexpr = (yyvsp[-4].slot);
			ifthen->thenstmt = (yyvsp[-2].slot);
			ifthen->elsestmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> IF LPAR exprlist RPAR stmt ELSE stmt %d\n", (yyval.slot));
		}
#line 1765 "js.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 210 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("stmt -> RETURN exprlist SEMI %d\n", (yyval.slot));
		}
#line 1777 "js.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 218 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= flag_break;
			en->expr = 0;

			if (debug) printf("stmt -> BREAK SEMI %d\n", (yyval.slot));
		}
#line 1790 "js.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 227 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->flag |= flag_continue;
			en->expr = 0;

			if (debug) printf("stmt -> CONTINUE SEMI %d\n", (yyval.slot));
		}
#line 1803 "js.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 236 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> WHILE LPAR exprlist RPAR stmt %d\n", (yyval.slot));
		}
#line 1816 "js.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 245 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + (yyval.slot));
			wn->cond = (yyvsp[-2].slot);
			wn->stmt = (yyvsp[-5].slot);

			if (debug) printf("stmt -> DO stmt WHILE LPAR exprlist RPAR SEMI %d\n", (yyval.slot));
		}
#line 1829 "js.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 254 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decllist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1844 "js.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 265 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + (yyval.slot));
			fn->init = (yyvsp[-6].slot);
			fn->cond = (yyvsp[-4].slot);
			fn->incr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR exprlist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", (yyvsp[-6].slot), (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 1859 "js.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 276 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_in;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decl FORIN expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1874 "js.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 287 "js.y" /* yacc.c:1646  */
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
#line 1891 "js.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 300 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + (yyval.slot));
			fn->hdr->aux = for_of;
			fn->var = (yyvsp[-4].slot);
			fn->expr = (yyvsp[-2].slot);
			fn->stmt = (yyvsp[0].slot);

			if (debug) printf("stmt -> FOR LPAR VAR decl FOROF expr RPAR stmt %d\n", (yyval.slot));
		}
#line 1906 "js.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 311 "js.y" /* yacc.c:1646  */
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
#line 1923 "js.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 324 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> LBRACE stmtlist RBRACE\n");
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 1932 "js.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 329 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> VAR decllist SEMI %d\n", (yyvsp[-1].slot));
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 1941 "js.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 334 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("stmt -> exprlist[%d] SEMI\n", (yyvsp[-1].slot));
			(yyval.slot) = (yyvsp[-1].slot);
		}
#line 1950 "js.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 342 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("stmtlist -> _empty_\n");
		}
#line 1959 "js.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 347 "js.y" /* yacc.c:1646  */
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
#line 1981 "js.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 368 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + (yyval.slot));
			sym->name = (yyvsp[0].slot);

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("symbol -> NAME[%s] %d\n", sn->string, (yyval.slot));
			}
		}
#line 1996 "js.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 382 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl | flag_lval;
			(yyval.slot) = (yyvsp[0].slot);

			if (debug) printf("decl -> symbol[%d]\n", (yyvsp[0].slot));
		}
#line 2008 "js.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 390 "js.y" /* yacc.c:1646  */
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
#line 2025 "js.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 406 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("decllist -> decl[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2037 "js.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 415 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (debug) printf("decllist -> decl[%d] COMMA decllist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2049 "js.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 426 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->left = (yyvsp[0].slot);
			bn->right = 0;

			if (debug) printf("enum -> NAME %d\n", (yyval.slot));
		}
#line 2062 "js.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 436 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("enum -> NAME ASSIGN expr %d\n", (yyval.slot));
		}
#line 2075 "js.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 448 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("enumlist -> enum[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2087 "js.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 457 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[-2].slot);

			if (debug) printf("enumlist -> enum[%d] COMMA enumlist %d\n", (yyvsp[-2].slot), (yyval.slot));
		}
#line 2099 "js.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 468 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_ternary, sizeof(ternaryNode), true);
			ternaryNode *tn = (ternaryNode *)(pd->table + (yyval.slot));
			tn->condexpr = (yyvsp[-4].slot);
			tn->trueexpr = (yyvsp[-2].slot);
			tn->falseexpr = (yyvsp[0].slot);

			if (debug) printf("expr -> expr[%d] TERN expr[%d] COLON expr[%d] %d\n", (yyvsp[-4].slot), (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2113 "js.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 479 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_enum, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[-1].slot);

			if (debug) printf("expr -> ENUM LBRACE enumlist[%d] RBRACE %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2125 "js.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 487 "js.y" /* yacc.c:1646  */
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
#line 2148 "js.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 507 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> TYPEOF expr %d\n", (yyval.slot));
		}
#line 2160 "js.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 515 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> INCR expr %d\n", (yyval.slot));
		}
#line 2176 "js.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 528 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_before;
			en->expr = (yyvsp[0].slot);

			Node *node = pd->table + (yyvsp[0].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> DECR expr %d\n", (yyval.slot));
		}
#line 2192 "js.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 540 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = incr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", (yyval.slot));
		}
#line 2208 "js.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 553 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = decr_after;
			en->expr = (yyvsp[-1].slot);

			Node *node = pd->table + (yyvsp[-1].slot);
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", (yyval.slot));
		}
#line 2224 "js.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 566 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_rushift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT expr %d\n", (yyval.slot));
		}
#line 2238 "js.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 576 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT expr %d\n", (yyval.slot));
		}
#line 2252 "js.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 586 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LSHIFT expr %d\n", (yyval.slot));
		}
#line 2266 "js.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 596 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lor, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LOR expr %d\n", (yyval.slot));
		}
#line 2279 "js.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 605 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_land, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LAND expr %d\n", (yyval.slot));
		}
#line 2292 "js.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 614 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_lt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LT expr %d\n", (yyval.slot));
		}
#line 2306 "js.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 624 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_le;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LE expr %d\n", (yyval.slot));
		}
#line 2320 "js.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 634 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_eq;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr EQ expr %d\n", (yyval.slot));
		}
#line 2334 "js.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 644 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_ne;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr NEQ expr %d\n", (yyval.slot));
		}
#line 2348 "js.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 654 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_ge;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr GE expr %d\n", (yyval.slot));
		}
#line 2362 "js.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 664 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_gt;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr GT expr %d\n", (yyval.slot));
		}
#line 2376 "js.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 674 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr PLUS expr %d\n", (yyval.slot));
		}
#line 2390 "js.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 684 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MINUS expr %d\n", (yyval.slot));
		}
#line 2404 "js.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 694 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MPY expr %d\n", (yyval.slot));
		}
#line 2418 "js.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 704 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MOD expr %d\n", (yyval.slot));
		}
#line 2432 "js.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 714 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr DIV expr %d\n", (yyval.slot));
		}
#line 2446 "js.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 724 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITAND expr %d\n", (yyval.slot));
		}
#line 2460 "js.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 734 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITXOR expr %d\n", (yyval.slot));
		}
#line 2474 "js.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 744 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = math_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr BITOR expr %d\n", (yyval.slot));
		}
#line 2488 "js.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 754 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_uminus;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> UMINUS expr %d\n", (yyval.slot));
		}
#line 2501 "js.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 763 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_not;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> NOT expr %d\n", (yyval.slot));
		}
#line 2514 "js.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 772 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + (yyval.slot));
			en->hdr->aux = neg_bitnot;
			en->expr = (yyvsp[0].slot);

			if (debug) printf("expr -> BITNOT expr %d\n", (yyval.slot));
		}
#line 2527 "js.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 781 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[-1].slot);
			if (debug) printf("expr -> LPAR exprlist[%d] RPAR\n", (yyvsp[-1].slot));
		}
#line 2536 "js.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 786 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_assign;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr[%d] ASSIGN expr[%d] %d\n", (yyvsp[-2].slot), (yyvsp[0].slot), (yyval.slot));
		}
#line 2552 "js.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 798 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_lshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr LSHIFT_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2568 "js.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 810 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_rshift;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr RSHIFT_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2584 "js.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 822 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_add;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr PLUS_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2600 "js.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 834 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_sub;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MINUS_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2616 "js.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 846 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_mpy;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MPY_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2632 "js.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 858 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_mod;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr MOD_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2648 "js.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 870 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_div;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr DIV_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2664 "js.tab.c" /* yacc.c:1646  */
    break;

  case 76:
#line 882 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_and;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr AND_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2680 "js.tab.c" /* yacc.c:1646  */
    break;

  case 77:
#line 894 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_or;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr OR_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2696 "js.tab.c" /* yacc.c:1646  */
    break;

  case 78:
#line 906 "js.y" /* yacc.c:1646  */
    {
			pd->table[(yyvsp[-2].slot)].flag |= flag_lval;

			(yyval.slot) = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->hdr->aux = pm_xor;
			bn->right = (yyvsp[0].slot);
			bn->left = (yyvsp[-2].slot);

			if (debug) printf("expr -> expr XOR_ASSIGN expr %d\n", (yyval.slot));
		}
#line 2712 "js.tab.c" /* yacc.c:1646  */
    break;

  case 79:
#line 918 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + (yyval.slot));
			fc->name = (yyvsp[-3].slot);
			fc->args = (yyvsp[-1].slot);

			if (debug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", (yyvsp[-3].slot), (yyvsp[-1].slot), (yyval.slot));
		}
#line 2725 "js.tab.c" /* yacc.c:1646  */
    break;

  case 80:
#line 927 "js.y" /* yacc.c:1646  */
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
#line 2768 "js.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 966 "js.y" /* yacc.c:1646  */
    {
			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + (yyvsp[0].slot));
				printf("expr -> STRING[%s] %d\n", sn->string, (yyvsp[0].slot));
			}
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2780 "js.tab.c" /* yacc.c:1646  */
    break;

  case 82:
#line 974 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + (yyval.slot));
			fc->hdr->flag |= flag_newobj;
			fc->name = (yyvsp[-3].slot);
			fc->args = (yyvsp[-1].slot);

			if (debug) printf("expr -> NEW expr LPAR arglist RPAR %d\n", (yyval.slot));
		}
#line 2794 "js.tab.c" /* yacc.c:1646  */
    break;

  case 83:
#line 984 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2802 "js.tab.c" /* yacc.c:1646  */
    break;

  case 84:
#line 988 "js.y" /* yacc.c:1646  */
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
#line 2818 "js.tab.c" /* yacc.c:1646  */
    break;

  case 85:
#line 1000 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + (yyval.slot));
			bn->right = (yyvsp[-1].slot);
			bn->left = (yyvsp[-3].slot);

			if (debug) printf("expr -> expr LBRACK expr RBRACK %d\n", (yyval.slot));
		}
#line 2831 "js.tab.c" /* yacc.c:1646  */
    break;

  case 86:
#line 1009 "js.y" /* yacc.c:1646  */
    {
			if (debug) printf("expr -> objarraylit\n");
			(yyval.slot) = (yyvsp[0].slot);
		}
#line 2840 "js.tab.c" /* yacc.c:1646  */
    break;

  case 87:
#line 1017 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("exprlist -> _empty_\n");
		}
#line 2849 "js.tab.c" /* yacc.c:1646  */
    break;

  case 88:
#line 1022 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("exprlist -> expr[%d]\n", (yyvsp[0].slot));
		}
#line 2858 "js.tab.c" /* yacc.c:1646  */
    break;

  case 89:
#line 1027 "js.y" /* yacc.c:1646  */
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
#line 2882 "js.tab.c" /* yacc.c:1646  */
    break;

  case 90:
#line 1050 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->type = node_ref;
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("arg -> BITAND symbol\n");
		}
#line 2893 "js.tab.c" /* yacc.c:1646  */
    break;

  case 91:
#line 1057 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = (yyvsp[0].slot);
			if (debug) printf("arg -> expr[%d]\n", (yyvsp[0].slot));
		}
#line 2902 "js.tab.c" /* yacc.c:1646  */
    break;

  case 92:
#line 1065 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("arglist -> _empty_\n");
		}
#line 2911 "js.tab.c" /* yacc.c:1646  */
    break;

  case 93:
#line 1070 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug)
				printf("arglist -> arg[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 2924 "js.tab.c" /* yacc.c:1646  */
    break;

  case 94:
#line 1079 "js.y" /* yacc.c:1646  */
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
#line 2943 "js.tab.c" /* yacc.c:1646  */
    break;

  case 95:
#line 1097 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + (yyval.slot));
			on->elemlist = (yyvsp[-1].slot);

			if (debug) printf("objarraylit -> LBRACE elemlist RBRACE %d\n", (yyval.slot));
		}
#line 2955 "js.tab.c" /* yacc.c:1646  */
    break;

  case 96:
#line 1106 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + (yyval.slot));
			an->exprlist = (yyvsp[-1].slot);

			if (debug) printf("objarraylit -> LBRACK arraylist[%d] RBRACK %d\n", (yyvsp[-1].slot), (yyval.slot));
		}
#line 2967 "js.tab.c" /* yacc.c:1646  */
    break;

  case 97:
#line 1117 "js.y" /* yacc.c:1646  */
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
#line 2983 "js.tab.c" /* yacc.c:1646  */
    break;

  case 98:
#line 1129 "js.y" /* yacc.c:1646  */
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
#line 2999 "js.tab.c" /* yacc.c:1646  */
    break;

  case 99:
#line 1144 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("arraylist -> _empty_\n");
		}
#line 3008 "js.tab.c" /* yacc.c:1646  */
    break;

  case 100:
#line 1149 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("arraylist -> expr[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3020 "js.tab.c" /* yacc.c:1646  */
    break;

  case 101:
#line 1157 "js.y" /* yacc.c:1646  */
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
#line 3037 "js.tab.c" /* yacc.c:1646  */
    break;

  case 102:
#line 1173 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("elemlist -> _empty_\n");
		}
#line 3046 "js.tab.c" /* yacc.c:1646  */
    break;

  case 103:
#line 1178 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("elemlist -> elem[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3058 "js.tab.c" /* yacc.c:1646  */
    break;

  case 104:
#line 1186 "js.y" /* yacc.c:1646  */
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
#line 3075 "js.tab.c" /* yacc.c:1646  */
    break;

  case 105:
#line 1202 "js.y" /* yacc.c:1646  */
    {
			(yyval.slot) = 0;
			if (debug) printf("paramlist -> _empty_\n");
		}
#line 3084 "js.tab.c" /* yacc.c:1646  */
    break;

  case 106:
#line 1207 "js.y" /* yacc.c:1646  */
    {
			symNode *sym = (symNode *)(pd->table + (yyvsp[0].slot));
			sym->hdr->flag |= flag_decl;

			(yyval.slot) = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + (yyval.slot));
			ln->elem = (yyvsp[0].slot);

			if (debug) printf("paramlist -> symbol[%d] %d\n", (yyvsp[0].slot), (yyval.slot));
		}
#line 3099 "js.tab.c" /* yacc.c:1646  */
    break;

  case 107:
#line 1218 "js.y" /* yacc.c:1646  */
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
#line 3119 "js.tab.c" /* yacc.c:1646  */
    break;


#line 3123 "js.tab.c" /* yacc.c:1646  */
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
#line 1234 "js.y" /* yacc.c:1906  */


void yyerror( void *scanner, parseData *pd, const char *s)
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineno, s);
}

