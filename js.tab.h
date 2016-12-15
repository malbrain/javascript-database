/* A Bison parser, made by GNU Bison 3.0.  */

/* Bison interface for Yacc-like parsers in C

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
#line 17 "js.y" /* yacc.c:1909  */
	// yymathexpr
	uint32_t slot;

#line 133 "js.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner, parseData *pd);

#endif /* !YY_YY_JS_TAB_H_INCLUDED  */
