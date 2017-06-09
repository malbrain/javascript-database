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
#line 17 "js.y" /* yacc.c:1909  */
	// yymathexpr
	uint32_t slot;

#line 134 "js.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner, parseData *pd);

#endif /* !YY_YY_JS_TAB_H_INCLUDED  */
