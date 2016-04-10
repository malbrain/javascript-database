%define api.pure full
%error-verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "jsdb.h"

#define YY_DECL
#include "jsdb.tab.h"
#include "jsdb.lex.h"

int yylex (YYSTYPE * yymathexpr_param, yyscan_t yyscanner, parseData *pd);
%}

%union {	// yymathexpr
	uint32_t slot;
}

%{
static bool debug = true;

void yyerror( void *scanner, parseData *pd, const char *s, ... );
%}

%lex-param		{ void *scanner } { parseData *pd }
%parse-param	{ void *scanner } { parseData *pd }

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
%token			NEW
%token			RETURN
%token			CONTINUE
%token			BREAK
%token			AMPER
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

%right			RPAR ELSE
%left			LOR
%left			LAND
%left			PLUS_ASSIGN MINUS_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN ASSIGN MPY_ASSIGN DIV_ASSIGN MOD_ASSIGN
%left			LT LE EQ NEQ GT GE
%left			LSHIFT RSHIFT
%left			PLUS MINUS
%left			MPY DIV MOD
%precedence		TYPEOF NOT
%precedence		UMINUS
%precedence		NAME
%precedence		LPAR
%precedence		DOT LBRACK
%right			INCR DECR

%type <slot>	enum enumlist
%type <slot>	decl decllist
%type <slot>	stmt stmtlist
%type <slot>	paramlist arglist
%type <slot>	elem elemlist
%type <slot>	objarraylit
%type <slot>	funcdef
%type <slot>	fname pgmlist
%type <slot>	list expr
%type <slot>	symbol

%start script
%%

script:
		EOS
		{
			if (debug) printf("script -> EOS\n");
			pd->beginning = 0;
			YYACCEPT;
		}
	|	pgmlist
		{
			if (debug) printf("script -> pgmlist\n");
			pd->beginning = $1;
			YYACCEPT;
		}
	;

pgmlist: 
		%empty
		{
			$$ = 0;
			if (debug) printf("pgmlist -> _empty_\n");
		}
	|	stmt pgmlist 
		{
			if ($1 == 0) {
				$$ = $2;
				if (debug) printf("pgmlist -> empty stmt discard\n");
			} else {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if ($2 == 0) {
					ln->hdr->type = node_endlist;
					if (debug) printf("pgmlist -> stmt[%d] %d\n", $1, $$);
				} else {
					if (debug) printf("pgmlist -> stmt[%d] pgmlist %d\n", $1, $$);
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
				if (debug) printf("pgmlist -> funcdef[%d] %d\n", $1, $$);
			} else {
				if (debug) printf("pgmlist -> funcdef[%d] pgmlist %d\n", $1, $$);
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
			fcnDeclNode *fn = (fcnDeclNode *)(pd->table + $$);
			fn->hdr->type = node_fcndef;
			fn->name = node;
			fn->params = $4;
			fn->body = $7;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + sym->name);
				printf("funcdef -> symbol[%.*s] LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	;

fname:
		%empty
		{
			if (debug) printf("fname -> _empty_\n");
			$$ = 0;
		}
	|	NAME
		{
			$$ = 1;
		}
	;

stmt:	
		IF LPAR list RPAR stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = 0;

			if (debug) printf("stmt -> IF LPAR list RPAR stmt %d\n", $$);
		}
	|	IF LPAR list RPAR stmt ELSE stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = $7;

			if (debug) printf("stmt -> IF LPAR list RPAR stmt ELSE stmt %d\n", $$);
		}
	|	RETURN list SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("stmt -> RETURN list SEMI %d\n", $$);
		}
	|	BREAK SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->flag |= flag_break;
			en->expr = 0;

			if (debug) printf("stmt -> BREAK SEMI %d\n", $$);
		}
	|	CONTINUE SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->flag |= flag_continue;
			en->expr = 0;

			if (debug) printf("stmt -> CONTINUE SEMI %d\n", $$);
		}
	|	WHILE LPAR list RPAR stmt
		{
			$$ = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $3;
			wn->stmt = $5;

			if (debug) printf("stmt -> WHILE LPAR list RPAR stmt %d\n", $$);
		}
	|	DO stmt WHILE LPAR list RPAR SEMI
		{
			$$ = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $5;
			wn->stmt = $2;

			if (debug) printf("stmt -> DO stmt WHILE LPAR list RPAR SEMI %d\n", $$);
		}
	|	FOR LPAR VAR decllist SEMI list SEMI list RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $4;
			fn->cond = $6;
			fn->incr = $8;
			fn->stmt = $10;

			if (debug) printf("stmt -> FOR LPAR VAR decllist SEMI list SEMI list RPAR stmt %d\n", $$);
		}
	|	FOR LPAR list SEMI list SEMI list RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $3;
			fn->cond = $5;
			fn->incr = $7;
			fn->stmt = $9;

			if (debug) printf("stmt -> FOR LPAR list SEMI list SEMI list RPAR stmt %d\n", $$);
		}
	|	LBRACE stmtlist RBRACE
		{
			if (debug) printf("stmt -> LBRACE stmtlist RBRACE\n");
			$$ = $2;
		}
	|	ENUM expr LBRACE enumlist RBRACE
		{
			Node *n = (Node *)(pd->table + $2);
			n->flag |= flag_lval | flag_decl;

			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->left = $2;
			bn->right = $4;

			if (debug) printf("stmt -> ENUM expr LBRACE enumlist RBRACE %d\n", $$);
		}
	|	VAR decllist SEMI
		{
			if (debug) printf("stmt -> VAR decllist SEMI %d\n", $$);
			$$ = $2;
		}
	|	list SEMI
		{
			if (debug) printf("stmt -> list SEMI\n");
			$$ = $1;
		}
	;

stmtlist: 
		%empty
		{
			$$ = 0;
			if (debug) printf("stmtlist -> _empty_\n");
		}
	|	stmt stmtlist 
		{
			if ($1 == 0) {
				$$ = $2;
				if (debug) printf("stmtlist -> empty stmt discard\n");
			} else {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if ($2 == 0) {
					ln->hdr->type = node_endlist;
					if (debug) printf("stmtlist -> stmt[%d] %d\n", $1, $$);
				} else {
					if (debug) printf("stmtlist -> stmt[%d] stmtlist %d\n", $1, $$);
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

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("symbol -> NAME[%.*s] %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	;

decl:
		symbol
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;
			$$ = $1;

			if (debug) printf("decl -> symbol[%d]\n", $1);
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

			if (debug) printf("decl -> symbol[%d] ASSIGN expr[%d] %d\n", $1, $3, $$);
		}
	;

enum:
		NAME
		{
			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->left = $1;
			bn->right = 0;

			if (debug) printf("enum -> NAME %d\n", $$);
		}
	|
		NAME ASSIGN expr
		{
			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("enum -> NAME ASSIGN expr %d\n", $$);
		}
	;

enumlist:
		enum
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("enumlist -> enum[%d] %d\n", $1, $$);
		}
	|
		enum COMMA enumlist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("enumlist -> enum[%d] COMMA enumlist %d\n", $1, $$);
		}
	;

decllist:
		decl
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("decllist -> decl[%d] %d\n", $1, $$);
		}
	|
		decl COMMA decllist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("decllist -> decl[%d] COMMA decllist %d\n", $1, $$);
		}
	;

expr:	
		FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE
		{
			int node = 0;

			if ($2) {
				node = newNode(pd, node_var, sizeof(symNode), true);
				symNode *sym = (symNode *)(pd->table + node);
				sym->name = $2;
			}

			$$ = newNode(pd, node_fcnexpr, sizeof(fcnDeclNode), true);
			fcnDeclNode *fn = (fcnDeclNode *)(pd->table + $$);
			fn->hdr->type = node_fcnexpr;
			fn->name = node;
			fn->params = $4;
			fn->body = $7;

			if (debug) printf("funcexpr -> FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", $$);
		}

	|	TYPEOF expr
		{
			$$ = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("expr -> TYPEOF expr %d\n", $$);
		}
	|	INCR expr
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (debug) printf("expr -> INCR expr %d\n", $$);
		}
	|
		DECR expr
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (debug) printf("expr -> DECR expr %d\n", $$);
		}
	|	expr INCR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (debug) printf("expr -> expr INCR %d\n", $$);
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

			if (debug) printf("expr -> expr INCR %d\n", $$);
		}

	|	expr RSHIFT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_rshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr RSHIFT expr %d\n", $$);
		}
	|	expr LSHIFT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LSHIFT expr %d\n", $$);
		}
	|	expr LOR expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lor;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LOR expr %d\n", $$);
		}
	|	expr LAND expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_land;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LAND expr %d\n", $$);
		}
	|	expr LT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lt;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LT expr %d\n", $$);
		}
	|	expr LE expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_le;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LE expr %d\n", $$);
		}
	|	expr EQ expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_eq;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr EQ expr %d\n", $$);
		}
	|	expr NEQ expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_ne;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr NEQ expr %d\n", $$);
		}
	|	expr GE expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_ge;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr GE expr %d\n", $$);
		}
	|	expr GT expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_gt;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr GT expr %d\n", $$);
		}
	|	expr PLUS expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_add;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr PLUS expr %d\n", $$);
		}
	|	expr MINUS expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_sub;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr MINUS expr %d\n", $$);
		}
	|	expr MPY expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mpy;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr MPY expr %d\n", $$);
		}
	|	expr MOD expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mod;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr MOD expr %d\n", $$);
		}
	|	expr DIV expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_div;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr DIV expr %d\n", $$);
		}
	|	MINUS expr %prec UMINUS
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_uminus;
			en->expr = $2;

			if (debug) printf("expr -> UMINUS expr %d\n", $$);
		}
	|	NOT expr
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_not;
			en->expr = $2;

			if (debug) printf("expr -> NOT expr %d\n", $$);
		}
	|	LPAR list RPAR
		{
			$$ = $2;
			if (debug) printf("expr -> LPAR list[%d] RPAR\n", $2);
		}
	|	expr ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr ASSIGN expr %d\n", $$);
		}
	|	expr LSHIFT_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_lshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LSHIFT_ASSIGN expr %d\n", $$);
		}
	|	expr RSHIFT_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_rshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr RSHIFT_ASSIGN expr %d\n", $$);
		}
	|	expr PLUS_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_add;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr PLUS_ASSIGN expr %d\n", $$);
		}
	|	expr MINUS_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_sub;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr MINUS_ASSIGN expr %d\n", $$);
		}
	|	expr MPY_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_mpy;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr MPY_ASSIGN expr %d\n", $$);
		}
	|	expr MOD_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_mod;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr MOD_ASSIGN expr %d\n", $$);
		}
	|	expr DIV_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_div;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr DIV_ASSIGN expr %d\n", $$);
		}
	|	expr LPAR arglist RPAR
		{
			$$ = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + $$);
			fc->name = $1;
			fc->args = $3;

			if (debug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", $1, $3, $$);
		}
	|	NUM
		{
			if (debug) {
				numNode *nn = (numNode *)(pd->table + $1);
				if (nn->hdr->aux == nn_dbl)
					printf("expr -> DBL[%G] %d\n", nn->dblval, $1);
				if (nn->hdr->aux == nn_int)
					printf("expr -> INT[%lld] %d\n", nn->intval, $1);
				if (nn->hdr->aux == nn_bool)
					printf("expr -> BOOL[%d] %d\n", nn->boolval, $1);
				if (nn->hdr->aux == nn_null)
					printf("expr -> NULL %d\n", $1);
				if (nn->hdr->aux == nn_this)
					printf("expr -> THIS %d\n", $1);
				if (nn->hdr->aux == nn_args)
					printf("expr -> ARGUMENTS %d\n", $1);
				if (nn->hdr->aux == nn_undef)
					printf("expr -> UNDEFINED %d\n", $1);
				if (nn->hdr->aux == nn_infinity)
					printf("expr -> INFINITY %d\n", $1);
				if (nn->hdr->aux == nn_nan)
					printf("expr -> NAN %d\n", $1);
			}
			$$ = $1;
		}
	|	STRING
		{
			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("expr -> STRING[%.*s] %d\n", sn->hdr->aux, sn->string, $1);
			}
			$$ = $1;
		}
	|	NEW symbol LPAR arglist RPAR
		{
			$$ = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + $$);
			fc->hdr->flag |= flag_newobj;
			fc->name = $2;
			fc->args = $4;

			if (debug) printf("expr -> NEW expr LPAR list RPAR %d\n", $$);
		}
	|	symbol
		{
			$$ = $1;
		}
	|	expr DOT NAME
		{
			$$ = newNode(pd, node_access, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $3);
				printf("expr -> expr DOT NAME[%.*s] %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	|	expr LBRACK expr RBRACK
		{
			$$ = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr LBRACK expr RBRACK %d\n", $$);
		}
	|	objarraylit
		{
			if (debug) printf("expr -> objarraylit\n");
			$$ = $1;
		}
	;

list:
		%empty
		{
			$$ = 0;
			if (debug) printf("list -> _empty_\n");
		}
	|	expr 
		{
			$$ = $1;
			if (debug) printf("list -> expr[%d]\n", $1);
		}
	|	expr COMMA list
		{
			if (pd->table[$3].type != node_list) { 
				$$ = newNode(pd, node_endlist, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $3;
			}

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug)
				printf("list -> expr[%d] COMMA list %d\n", $1, $$);
		}
	;

arglist:
		%empty
		{
			$$ = 0;
			if (debug) printf("arglist -> _empty_\n");
		}
	|	expr 
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug)
				printf("arglist -> expr[%d] %d\n", $1, $$);
		}
	|	expr COMMA arglist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug)
				printf("arglist -> expr[%d] COMMA arglist %d\n", $1, $$);
		}
	;

objarraylit:
		LBRACE elemlist RBRACE 
		{
			$$ = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + $$);
			on->elemlist = $2;

			if (debug) printf("objarraylit -> LBRACE elemlist RBRACE %d\n", $$);
		}

	|	LBRACK list RBRACK
		{
			$$ = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + $$);
			an->exprlist = $2;

			if (debug) printf("objarraylit -> LBRACK arraylist RBRACK %d\n", $$);
		}
	;

elem:
		NAME COLON expr
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> NAME[%.*s] COLON expr %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	|	STRING COLON expr
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> STRING[%.*s] COLON expr %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	;

elemlist:
		%empty
		{
			$$ = 0;
			if (debug) printf("elemlist -> _empty_\n");
		}
	|	elem
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("elemlist -> elem[%d] %d\n", $1, $$);
		}
	|	elem COMMA elemlist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("elemlist -> elem[%d] COMMA elemlist %d\n", $1, $$);
		}
	;

paramlist:
		%empty
		{
			$$ = 0;
			if (debug) printf("paramlist -> _empty_\n");
		}
	|	symbol 
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;

			$$ = newNode(pd, node_endlist, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("paramlist -> symbol[%d] %d\n", $1, $$);
		}
	|	symbol COMMA paramlist
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("paramlist -> symbol[%d] COMMA paramlist %d\n", $1, $$);
		}
	;
%%

void yyerror( void *scanner, parseData *pd, const char *s, ... )
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineno, s);
}

