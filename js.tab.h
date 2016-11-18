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
#line 17 "js.y" /* yacc.c:1909  */
	// yymathexpr
	uint32_t slot;

#line 126 "js.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner, parseData *pd);

#endif /* !YY_YY_JS_TAB_H_INCLUDED  */
