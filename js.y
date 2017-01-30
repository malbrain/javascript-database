%define api.pure full
%error-verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "js.h"

#define YY_DECL
#include "js.tab.h"
#include "js.lex.h"

int yylex (YYSTYPE * yymathexpr_param, yyscan_t yyscanner, parseData *pd);
%}

%union {	// yymathexpr
	uint32_t slot;
}

%{
void yyerror( void *scanner, parseData *pd, const char *s);
%}

%lex-param		{ void *scanner }
%lex-param		{ parseData *pd }
%parse-param	{ void *scanner }
%parse-param	{ parseData *pd }

%token <slot>	STRING 
%token <slot>	NAME
%token <slot>	NUM
%token			EOS
%token			IF
%token			ELSE
%token			WHILE
%token			DO
%token			FOR
%token			FCN
%token			VAR
%token			RETURN
%token			CONTINUE
%token			BREAK
%token			LPAR
%token			RPAR
%token			COLON
%token			COMMA
%token			LBRACE
%token			RBRACE
%token			LBRACK
%token			RBRACK
%token			SEMI
%token			ENUM
%token			INCR
%token			DECR
%token			DOT
%token			NOT
%token			BITNOT
%token			BITAND
%token			BITXOR
%token			BITOR
%token			TERN
%token			FORIN
%token			FOROF
%token			TRY
%token			CATCH
%token			FINALLY
%token			THROW
%token			NEW

%precedence		NAME
%right			ELSE RPAR
%right			PLUS_ASSIGN MINUS_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN ASSIGN MPY_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN
%right			TERN COLON
%left			LOR
%left			LAND
%left			BITOR
%left			BITXOR
%left			BITAND
%left			IDENTICAL NOTIDENTICAL LT LE EQ NEQ GT GE
%left			LSHIFT RSHIFT RUSHIFT
%left			PLUS MINUS
%left			MPY DIV MOD
%precedence		DEL NEW
%precedence		TYPEOF
%precedence 	INCR DECR
%precedence		NOT BITNOT
%precedence		UMINUS
%precedence		DOT LBRACK
%precedence		LPAR

%type <slot>	enum enumlist
%type <slot>	decl decllist
%type <slot>	stmt stmtlist
%type <slot>	paramlist arglist
%type <slot>	elem elemlist
%type <slot>	objarraylit
%type <slot>	funcdef
%type <slot>	fname pgmlist
%type <slot>	exprlist expr
%type <slot>	symbol
%type <slot>	arraylist

%start script
%%

script:
		EOS
		{
			if (parseDebug) printf("script -> EOS\n");
			pd->beginning = 0;
			YYACCEPT;
		}
	|	pgmlist
		{
			if (parseDebug) printf("script -> pgmlist\n");
			pd->beginning = $1;
			YYACCEPT;
		}
	;

pgmlist: 
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("pgmlist -> _empty_\n");
		}
	|	stmt pgmlist 
		{
			if ($1 == 0) {
				$$ = $2;
				if (parseDebug) printf("pgmlist -> empty stmt discard\n");
			} else {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if ($2 == 0) {
					ln->hdr->type = node_endlist;
					if (parseDebug) printf("pgmlist -> stmt[%d] %d\n", $1, $$);
				} else {
					if (parseDebug) printf("pgmlist -> stmt[%d] pgmlist %d\n", $1, $$);
				}
			}
		}
	|	funcdef pgmlist 
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if ($2 == 0) {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("pgmlist -> funcdef[%d] %d\n", $1, $$);
			} else {
				if (parseDebug) printf("pgmlist -> funcdef[%d] pgmlist %d\n", $1, $$);
			}
		}
	;

funcdef:
		FCN NAME LPAR paramlist RPAR LBRACE pgmlist RBRACE
		{
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = $2;

			$$ = newNode(pd, node_fcndef, sizeof(fcnDeclNode), true);
			fcnDeclNode *fd = (fcnDeclNode *)(pd->table + $$);
			fd->hdr->type = node_fcndef;
			fd->name = node;
			fd->params = $4;
			fd->body = $7;

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + sym->name);
				printf("funcdef -> symbol[%s] LPAR paramlist[%d] RPAR LBRACE pgmlist[%d] RBRACE %d\n", sn->str.val, $4, $7, $$);
			}
		}
	;

fname:
		%empty
		{
			if (parseDebug) printf("fname -> _empty_\n");
			$$ = 0;
		}
	|	NAME
		{
			$$ = 1;
		}
	;

stmt:	
		IF LPAR exprlist RPAR stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = 0;

			if (parseDebug) printf("stmt -> IF LPAR exprlist[%d] RPAR stmt[%d] %d\n", $3, $5, $$);
		}
	|	IF LPAR exprlist RPAR stmt ELSE stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = $7;

			if (parseDebug) printf("stmt -> IF LPAR exprlist[%d] RPAR stmt[%d] ELSE stmt[%d] %d\n", $3, $5, $7, $$);
		}
	|	THROW expr SEMI
		{
			$$ = newNode(pd, node_throw, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (parseDebug) printf("stmt -> THROW expr[%d] SEMI %d\n", $2, $$);
		}
	|	RETURN exprlist SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (parseDebug) printf("stmt -> RETURN exprlist[%d] SEMI %d\n", $2, $$);
		}
	|	BREAK SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->flag |= ctl_break;
			en->expr = 0;

			if (parseDebug) printf("stmt -> BREAK SEMI %d\n", $$);
		}
	|	CONTINUE SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->flag |= ctl_continue;
			en->expr = 0;

			if (parseDebug) printf("stmt -> CONTINUE SEMI %d\n", $$);
		}
	|	WHILE LPAR exprlist RPAR stmt
		{
			$$ = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $3;
			wn->stmt = $5;

			if (parseDebug) printf("stmt -> WHILE LPAR exprlist[%d] RPAR stmt[%d] %d\n", $3, $5, $$);
		}
	|	DO stmt WHILE LPAR exprlist RPAR SEMI
		{
			$$ = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $5;
			wn->stmt = $2;

			if (parseDebug) printf("stmt -> DO stmt[%d] WHILE LPAR exprlist[%d] RPAR SEMI %d\n", $2, $5, $$);
		}
	|	FOR LPAR VAR decllist SEMI exprlist SEMI exprlist RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $4;
			fn->cond = $6;
			fn->incr = $8;
			fn->stmt = $10;

			if (parseDebug) printf("stmt -> FOR LPAR VAR decllist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", $4, $6, $8, $10, $$);
		}
	|	FOR LPAR exprlist SEMI exprlist SEMI exprlist RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $3;
			fn->cond = $5;
			fn->incr = $7;
			fn->stmt = $9;

			if (parseDebug) printf("stmt -> FOR LPAR exprlist[%d] SEMI exprlist[%d] SEMI exprlist[%d] RPAR stmt[%d] %d\n", $3, $5, $7, $9, $$);
		}
	|	FOR LPAR VAR decl FORIN expr RPAR stmt
		{
			$$ = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + $$);
			fn->hdr->aux = for_in;
			fn->var = $4;
			fn->expr = $6;
			fn->stmt = $8;

			if (parseDebug) printf("stmt -> FOR LPAR VAR decl[%d] FORIN expr[%d] RPAR stmt[%d] %d\n", $4, $6, $8, $$);
		}
	|	FOR LPAR expr FORIN expr RPAR stmt
		{
			pd->table[$3].flag |= flag_lval;

			$$ = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + $$);
			fn->hdr->aux = for_in;
			fn->var = $3;
			fn->expr = $5;
			fn->stmt = $7;

			if (parseDebug) printf("stmt -> FOR LPAR expr[%d] FORIN expr[%d] RPAR stmt[%d] %d\n", $3, $5, $7, $$);
		}
	|	FOR LPAR VAR decl FOROF expr RPAR stmt
		{
			$$ = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + $$);
			fn->hdr->aux = for_of;
			fn->var = $4;
			fn->expr = $6;
			fn->stmt = $8;

			if (parseDebug) printf("stmt -> FOR LPAR VAR decl[%d] FOROF expr[%d] RPAR stmt[%d] %d\n", $4, $6, $8, $$);
		}
	|	FOR LPAR expr FOROF expr RPAR stmt
		{
			pd->table[$3].flag |= flag_lval;

			$$ = newNode(pd, node_forin, sizeof(forInNode), false);
			forInNode *fn = (forInNode *)(pd->table + $$);
			fn->hdr->aux = for_of;
			fn->var = $3;
			fn->expr = $5;
			fn->stmt = $7;

			if (parseDebug) printf("stmt -> FOR LPAR expr[%d] FOROF expr[%d] RPAR stmt[%d] %d\n", $3, $5, $7, $$);
		}
	|	TRY LBRACE stmtlist RBRACE
		{
			$$ = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + $$);
			xn->tryblk = $3;

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE %d\n", $3, $$);
		}
	|	TRY LBRACE stmtlist RBRACE CATCH LPAR NAME RPAR LBRACE stmtlist RBRACE
		{
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = $7;

			$$ = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + $$);
			xn->tryblk = $3;
			xn->binding = node;
			xn->catchblk = $10;

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE CATCH LPAR NAME[%d] RPAR LBRACE stmtlist[%d] RBRACE %d\n", $3, node, $10, $$);
		}
	|	TRY LBRACE stmtlist RBRACE FINALLY LBRACE stmtlist RBRACE
		{
			$$ = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + $$);
			xn->tryblk = $3;
			xn->finallyblk = $7;

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", $3, $7, $$);
		}
	|	TRY LBRACE stmtlist RBRACE CATCH LPAR NAME RPAR LBRACE stmtlist RBRACE FINALLY LBRACE stmtlist RBRACE
		{
			int node = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + node);
			sym->name = $7;

			$$ = newNode(pd, node_xcp, sizeof(xcpNode), true);
			xcpNode *xn = (xcpNode *)(pd->table + $$);
			xn->tryblk = $3;
			xn->binding = node;
			xn->catchblk = $10;
			xn->finallyblk = $14;

			if (parseDebug) printf("stmt -> TRY LBRACE stmtlist[%d] RBRACE CATCH LPAR NAME[%d] RPAR LBRACE stmtlist[%d] RBRACE FINALLY LBRACE stmtlist[%d] RBRACE %d\n", $3, node, $10, $14, $$);
		}
	|	LBRACE stmtlist RBRACE
		{
			$$ = $2;
			if (parseDebug) printf("stmt -> LBRACE stmtlist[%d] RBRACE %d\n", $2, $$);
		}
	|	VAR decllist SEMI
		{
			$$ = $2;
			if (parseDebug) printf("stmt -> VAR decllist SEMI %d\n", $2);
		}
	|	exprlist SEMI
		{
			$$ = $1;
			if (parseDebug) printf("stmt -> exprlist SEMI %d\n", $1);
		}
	;

stmtlist: 
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("stmtlist -> _empty_\n");
		}
	|	stmt stmtlist 
		{
			if ($1 == 0) {
				$$ = $2;
				if (parseDebug) printf("stmtlist -> empty stmt discard\n");
			} else {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if ($2 == 0) {
					ln->hdr->type = node_endlist;
					ln->hdr->aux = aux_endstmt;
					if (parseDebug) printf("stmtlist -> stmt[%d] %d\n", $1, $$);
				} else {
					if (parseDebug) printf("stmtlist -> stmt[%d] stmtlist %d\n", $1, $$);
				}
			}
		}
	;

symbol:
		NAME
		{
			$$ = newNode(pd, node_var, sizeof(symNode), true);
			symNode *sym = (symNode *)(pd->table + $$);
			sym->name = $1;

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("symbol -> NAME[%s] %d\n", sn->str.val, $$);
			}
		}
	;

decl:
		symbol
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl | flag_lval;
			$$ = $1;

			if (parseDebug) printf("decl -> symbol[%d]\n", $1);
		}
	|	symbol ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval | flag_decl;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("decl -> symbol[%d] ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	;

decllist:
		decl
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("decllist -> decl[%d] %d\n", $1, $$);
		}
	|
		decl COMMA decllist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("decllist -> decl[%d] COMMA decllist[%d] %d\n", $1, $3, $$);
		}
	;

enum:
		NAME
		{
			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->left = $1;
			bn->right = 0;

			if (parseDebug) printf("enum -> NAME[%d] %d\n", $1, $$);
		}
	|
		NAME ASSIGN expr
		{
			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("enum -> NAME[%d] ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	;

enumlist:
		enum
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("enumlist -> enum[%d] %d\n", $1, $$);
		}
	|
		enum COMMA enumlist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("enumlist -> enum[%d] COMMA enumlist %d\n", $1, $$);
		}
	;

expr:	
		expr TERN expr COLON expr
		{
			$$ = newNode(pd, node_ternary, sizeof(ternaryNode), true);
			ternaryNode *tn = (ternaryNode *)(pd->table + $$);
			tn->condexpr = $1;
			tn->trueexpr = $3;
			tn->falseexpr = $5;

			if (parseDebug) printf("expr -> expr[%d] TERN expr[%d] COLON expr[%d] %d\n", $1, $3, $5, $$);
		}

	|	ENUM LBRACE enumlist RBRACE
		{
			$$ = newNode(pd, node_enum, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $3;

			if (parseDebug) printf("expr -> ENUM LBRACE enumlist[%d] RBRACE %d\n", $3, $$);
		}
	|	FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE
		{
			int node = 0;

			if ($2) {
				node = newNode(pd, node_var, sizeof(symNode), true);
				symNode *sym = (symNode *)(pd->table + node);
				sym->name = $2;
			}

			$$ = newNode(pd, node_fcnexpr, sizeof(fcnDeclNode), true);
			fcnDeclNode *fd = (fcnDeclNode *)(pd->table + $$);
			fd->hdr->type = node_fcnexpr;
			fd->name = node;
			fd->params = $4;
			fd->body = $7;

			if (parseDebug) printf("funcexpr -> FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", $$);
		}

	|	TYPEOF expr
		{
			$$ = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (parseDebug) printf("expr -> TYPEOF expr %d\n", $$);
		}
	|	INCR expr %prec UMINUS
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> INCR expr[%d] %d\n", $2, $$);
		}
	|
		DECR expr %prec UMINUS
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> DECR expr[%d] %d\n", $2, $$);
		}
	|	expr INCR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> expr[%d] INCR %d\n", $1, $$);
		}
	|
		expr DECR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (parseDebug) printf("expr -> expr[%d] INCR %d\n", $1, $$);
		}

	|	expr RUSHIFT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_rushift;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] RUSHIFT expr[%d] %d\n", $1, $3, $$);
		}
	|	expr RSHIFT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_rshift;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] RSHIFT expr[%d] %d\n", $1, $3, $$);
		}
	|	expr LSHIFT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lshift;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LSHIFT expr[%d] %d\n", $1, $3, $$);
		}
	|	expr LOR expr
		{
			$$ = newNode(pd, node_lor, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LOR expr[%d] %d\n", $1, $3, $$);
		}
	|	expr LAND expr
		{
			$$ = newNode(pd, node_land, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LAND expr[%d] %d\n", $1, $3, $$);
		}
	|	expr LT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lt;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LT expr[%d] %d\n", $1, $3, $$);
		}
	|	expr LE expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_le;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LE expr[%d] %d\n", $1, $3, $$);
		}
	|	expr EQ expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_eq;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] EQ expr[%d] %d\n", $1, $3, $$);
		}
	|	expr NEQ expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_ne;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] NEQ expr[%d] %d\n", $1, $3, $$);
		}
	|	expr IDENTICAL expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_id;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] IDENTICAL expr[%d] %d\n", $1, $3, $$);
		}
	|	expr NOTIDENTICAL expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_nid;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] NOTIDENTICAL expr[%d] %d\n", $1, $3, $$);
		}
	|	expr GE expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_ge;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] GE expr[%d] %d\n", $1, $3, $$);
		}
	|	expr GT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_gt;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] GT expr[%d] %d\n", $1, $3, $$);
		}
	|	expr PLUS expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_add;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] PLUS expr[%d] %d\n", $1, $3, $$);
		}
	|	expr MINUS expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_sub;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] MINUS expr[%d] %d\n", $1, $3, $$);
		}
	|	expr MPY expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mpy;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] MPY expr[%d] %d\n", $1, $3, $$);
		}
	|	expr MOD expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mod;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] MOD expr[%d] %d\n", $1, $3, $$);
		}
	|	expr DIV expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_div;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] DIV expr[%d] %d\n", $1, $3, $$);
		}
	|	expr BITAND expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_and;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] BITAND expr[%d] %d\n", $1, $3, $$);
		}
	|	expr BITXOR expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_xor;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] BITXOR expr[%d] %d\n", $1, $3, $$);
		}
	|	expr BITOR expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_or;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] BITOR expr[%d] %d\n", $1, $3, $$);
		}
	|	MINUS expr %prec UMINUS
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_uminus;
			en->expr = $2;

			if (parseDebug) printf("expr -> UMINUS expr %d\n", $$);
		}
	|	NOT expr
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_not;
			en->expr = $2;

			if (parseDebug) printf("expr -> NOT expr %d\n", $$);
		}
	|	BITNOT expr
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_bitnot;
			en->expr = $2;

			if (parseDebug) printf("expr -> BITNOT expr %d\n", $$);
		}
	|	LPAR exprlist RPAR
		{
			$$ = $2;
			if (parseDebug) printf("expr -> LPAR exprlist[%d] RPAR\n", $2);
		}
	|	expr ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr LSHIFT_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_lshift;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LSHIFT_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr RSHIFT_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_rshift;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] RSHIFT_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr PLUS_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_add;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] PLUS_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr MINUS_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_sub;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] MINUS_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr MPY_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_mpy;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] MPY_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr MOD_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_mod;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] MOD_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr DIV_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_div;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] DIV_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr AND_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_and;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] AND_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr OR_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_or;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] OR_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	expr XOR_ASSIGN expr
		{
			pd->table[$1].flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_xor;
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] XOR_ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	|	NEW expr
		{
			$$ = $2;
			pd->table[$2].aux = aux_newobj;

			if (parseDebug) printf("expr -> NEW expr %d\n", $$);
		}
	|	expr LPAR arglist RPAR
		{
			$$ = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + $$);

			fc->name = $1;
			fc->args = $3;

			if (parseDebug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", $1, $3, $$);
		}
	|	NUM
		{
			if (parseDebug) {
				numNode *nn = (numNode *)(pd->table + $1);

				switch(nn->hdr->aux) {
				  case nn_dbl:
					printf("expr -> DBL[%G] %d\n", nn->dblval, $1);
					break;
				  case nn_int:
					printf("expr -> INT[%" PRIi64 "] %d\n", nn->intval, $1);
					break;
				  case nn_bool:
					printf("expr -> BOOL[%d] %d\n", nn->boolval, $1);
					break;
				  case nn_null:
					printf("expr -> NULL %d\n", $1);
					break;
				  case nn_this:
					printf("expr -> THIS %d\n", $1);
					break;
				  case nn_args:
					printf("expr -> ARGS %d\n", $1);
					break;
				  case nn_undef:
					printf("expr -> UNDEFINED %d\n", $1);
					break;
				  case nn_infinity:
					printf("expr -> INFINITY %d\n", $1);
					break;
				  case nn_nan:
					printf("expr -> NAN %d\n", $1);
					break;
				  default:
					printf("expr -> DEFAULT %d\n", $1);
				}
			}
			$$ = $1;
		}
	|	STRING
		{
			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("expr -> STRING[%s] %d\n", sn->str.val, $1);
			}
			$$ = $1;
		}
	|	DEL expr
		{
			exprNode *en = (exprNode *)(pd->table + $2);
			en->hdr->flag |= ctl_delete;

			if (parseDebug) printf("expr -> DEL expr[%d]\n", $2);
		}
	|	objarraylit
		{
			if (parseDebug) printf("expr -> objarraylit\n");
			$$ = $1;
		}
	|
		expr DOT NAME
		{
			$$ = newNode(pd, node_access, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + $3);
				printf("expr -> expr[%d] DOT NAME[%s] %d\n", $1, sn->str.val, $$);
			}
		}
	|	expr LBRACK expr RBRACK
		{
			$$ = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) printf("expr -> expr[%d] LBRACK expr[%d] RBRACK %d\n", $1, $3, $$);
		}
	|	BITAND symbol
		{
			Node *node = pd->table + $2;
			if (parseDebug) printf ("expr -> BITAND expr[%d]\n", $2);
			node->flag |= flag_lval;
			$$ = $2;
		}
	|	symbol
		{
			$$ = $1;
		}
	;

exprlist:
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("exprlist -> _empty_\n");
		}
	|	expr 
		{
			$$ = $1;
			if (parseDebug) printf("exprlist -> expr[%d]\n", $1);
		}
	|	expr COMMA exprlist
		{
			if ($3 && pd->table[$3].type != node_list) { 
				$$ = newNode(pd, node_endlist, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $3;
			}

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if ($3) {
			  if (parseDebug)
				printf("exprlist -> expr[%d] COMMA exprlist %d\n", $1, $$);
			} else {
			  ln->hdr->type = node_endlist;
			  if (parseDebug) printf("exprlist -> expr[%d]\n", $1);
			}
		}
	;

arglist:
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("arglist -> _empty_\n");
		}
	|	expr
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug)
				printf("arglist -> expr[%d] %d\n", $1, $$);
		}
	|	expr COMMA arglist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if ($3) {
			  if (parseDebug)
				printf("arglist -> expr[%d] COMMA arglist %d\n", $1, $$);
			} else {
			  ln->hdr->type = node_endlist;
			  if (parseDebug)
				printf("arglist -> expr[%d] %d\n", $1, $$);
			}
		}
	;

objarraylit:
		LBRACE elemlist RBRACE 
		{
			$$ = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + $$);
			on->elemlist = $2;

			if (parseDebug) printf("objarraylit -> LBRACE elemlist[%d] RBRACE %d\n", $2, $$);
		}

	|	LBRACK arraylist RBRACK
		{
			$$ = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + $$);
			an->exprlist = $2;

			if (parseDebug) printf("objarraylit -> LBRACK arraylist[%d] RBRACK %d\n", $2, $$);
		}
	;

elem:
		NAME COLON expr
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> NAME[%s] COLON expr[%d] %d\n", sn->str.val, $3, $$);
			}
		}
	|	STRING COLON expr
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (parseDebug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> STRING[%s] COLON expr[%d] %d\n", sn->str.val, $3, $$);
			}
		}
	;

arraylist:
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("arraylist -> _empty_\n");
		}
	|	expr
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("arraylist -> expr[%d] %d\n", $1, $$);
		}
	|	expr COMMA arraylist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if ($3) {
				if (parseDebug) printf("arraylist -> elem[%d] COMMA arraylist %d\n", $1, $$);
			} else {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("arraylist -> expr[%d] %d\n", $1, $$);
			}
		}
	;

elemlist:
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("elemlist -> _empty_\n");
		}
	|	elem
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("elemlist -> elem[%d] %d\n", $1, $$);
		}
	|	elem COMMA elemlist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if ($3) {
				if (parseDebug) printf("elemlist -> elem[%d] COMMA elemlist %d\n", $1, $$);
			} else {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("elemlist -> elem[%d] %d\n", $1, $$);
			}
		}
	;

paramlist:
		%empty
		{
			$$ = 0;
			if (parseDebug) printf("paramlist -> _empty_\n");
		}
	|	symbol 
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;

			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (parseDebug) printf("paramlist -> symbol[%d] %d\n", $1, $$);
		}
	|	symbol COMMA paramlist
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if ($3) {
				if (parseDebug) printf("paramlist -> symbol[%d] COMMA paramlist %d\n", $1, $$);
			} else {
				ln->hdr->type = node_endlist;
				if (parseDebug) printf("paramlist -> symbol[%d] %d\n", $1, $$);
			}
		}
	;
%%

void yyerror( void *scanner, parseData *pd, const char *s)
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineNo, s);
}

