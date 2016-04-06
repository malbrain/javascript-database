%define api.pure full
%error-verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "jsdb.h"

#define YY_DECL int yylex \
	(YYSTYPE * yylval_param, yyscan_t yyscanner, parseData *pd)
%}

%union {	// yylval
	uint32_t slot;
}

%{
#include "jsdb.lex.h"

static bool debug = true;

void yyerror( void *scanner, parseData *pd, char *s, ... );
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
%precedence		PLUS_ASSIGN MINUS_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN ASSIGN
%precedence		MPY_ASSIGN DIV_ASSIGN MOD_ASSIGN
%left			LT LE EQ NEQ GT GE
%left			LSHIFT RSHIFT
%left			PLUS MINUS
%left			MPY DIV MOD
%precedence		TYPEOF
%precedence		UMINUS
%precedence		UNEGATE

%type <slot>	enum enumlist
%type <slot>	decl decllist
%type <slot>	arg arglist
%type <slot>	stmt stmtlist
%type <slot>	paramlist
%type <slot>	elem elemlist
%type <slot>	lval objarraylit
%type <slot>	arrayelem arraylist 
%type <slot>	funcdef funcexpr
%type <slot>	fname pgmlist
%type <slot>	mathlist mathexpr
%type <slot>	reqmathlist expr
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
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("pgmlist -> _endlist_ %d\n", $$);
		}
	|	stmt pgmlist 
		{
			if ($1) {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("pgmlist -> stmt pgmlist %d\n", $$);
			} else {
				$$ = $2;
				if (debug) printf("__discardemptystmt__\n");
			}
		}
	|	funcdef pgmlist 
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("pgmlist -> funcdef pgmlist %d\n", $$);
		}
	;

funcdef:
		FCN symbol LPAR paramlist RPAR LBRACE pgmlist RBRACE
		{
			$$ = newNode(pd, node_fcndef, sizeof(fcnDeclNode), true);
			fcnDeclNode *fn = (fcnDeclNode *)(pd->table + $$);
			fn->hdr->type = node_fcndef;
			fn->name = $2;
			fn->params = $4;
			fn->body = $7;

			if (debug) {
				symNode *sym = (symNode *)(pd->table + $2);
				stringNode *sn = (stringNode *)(pd->table + sym->name);
				printf("funcdef -> symbol[%.*s] LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	;

funcexpr:
		FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE
		{
			$$ = newNode(pd, node_fcnexpr, sizeof(fcnDeclNode), true);
			fcnDeclNode *fn = (fcnDeclNode *)(pd->table + $$);
			fn->hdr->type = node_fcnexpr;
			fn->name = $2;
			fn->params = $4;
			fn->body = $7;

			if (debug) printf("funcexpr -> FCN fname LPAR paramlist RPAR LBRACE pgmlist RBRACE %d\n", $$);
		}
	;

fname:
		%empty
		{
			if (debug) printf("fname -> _empty_\n");
			$$ = 0;
		}
	|	symbol
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;
			$$ = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + sym->name);
				printf("fname -> symbol[%.*s] %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	;

stmt:	
		IF LPAR reqmathlist RPAR stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = 0;

			if (debug) printf("stmt -> IF LPAR reqmathlist RPAR stmt %d\n", $$);
		}
	|	IF LPAR reqmathlist RPAR stmt ELSE stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = $7;

			if (debug) printf("stmt -> IF LPAR reqmathlist RPAR stmt ELSE stmt %d\n", $$);
		}
	|	RETURN objarraylit SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("stmt -> RETURN objarraylit SEMI %d\n", $$);
		}
	|	RETURN mathlist SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("stmt -> RETURN mathlist SEMI %d\n", $$);
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
	|	WHILE LPAR reqmathlist RPAR stmt
		{
			$$ = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $3;
			wn->stmt = $5;

			if (debug) printf("stmt -> WHILE LPAR reqmathlist RPAR stmt %d\n", $$);
		}
	|	DO stmt WHILE LPAR reqmathlist RPAR SEMI
		{
			$$ = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $5;
			wn->stmt = $2;

			if (debug) printf("stmt -> DO stmt WHILE LPAR reqmathlist RPAR SEMI %d\n", $$);
		}
	|	FOR LPAR VAR decllist SEMI mathlist SEMI mathlist RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $4;
			fn->cond = $6;
			fn->incr = $8;
			fn->stmt = $10;

			if (debug) printf("stmt -> FOR LPAR VAR decllist SEMI mathlist SEMI mathlist RPAR stmt %d\n", $$);
		}
	|	FOR LPAR mathlist SEMI mathlist SEMI mathlist RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $3;
			fn->cond = $5;
			fn->incr = $7;
			fn->stmt = $9;

			if (debug) printf("stmt -> FOR LPAR mathlist SEMI mathlist SEMI mathlist RPAR stmt %d\n", $$);
		}
	|	LBRACE stmtlist RBRACE
		{
			if (debug) printf("stmt -> LBRACE stmtlist RBRACE\n");
			$$ = $2;
		}
	|	ENUM lval LBRACE enumlist RBRACE
		{
			Node *n = (Node *)(pd->table + $2);
			n->flag |= flag_lval | flag_decl;

			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->left = $2;
			bn->right = $4;

			if (debug) printf("stmt -> ENUM lval LBRACE enumlist RBRACE %d\n", $$);
		}
	|	VAR decllist SEMI
		{
			if (debug) printf("stmt -> VAR decllist SEMI %d\n", $$);
			$$ = $2;
		}
	|	mathlist SEMI
		{
			if (debug) printf("stmt -> mathlist SEMI\n");
			$$ = $1;
		}
	;

stmtlist: 
		%empty
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("stmtlist -> endlist %d\n", $$);
		}
	|	stmt stmtlist 
		{
			if ($1) {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("stmtlist -> stmt stmtlist %d\n", $$);
			} else
				$$ = $2;
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
			if (debug) printf("decl -> symbol\n");
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;
			$$ = $1;
		}
	|	symbol ASSIGN mathexpr
		{
			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval | flag_decl;

			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("decl -> symbol ASSIGN mathexpr[%d] %d\n", $3, $$);
		}
	|	symbol ASSIGN funcexpr
		{
			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval | flag_decl;

			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("decl -> symbol ASSIGN funcexpr %d\n", $$);
		}
	|	symbol ASSIGN objarraylit
		{
			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval | flag_decl;

			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("decl -> symbol ASSIGN objarraylit %d\n", $$);
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
		NAME ASSIGN mathexpr
		{
			$$ = newNode(pd, node_enum, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("enum -> NAME ASSIGN mathexpr %d\n", $$);
		}
	;

enumlist:
		enum
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("enumlist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("enumlist -> enum %d\n", $$);
		}
	|
		enum COMMA enumlist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("enumlist -> enum COMMA enumlist %d\n", $$);
		}
	;

decllist:
		decl
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("decllist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("decllist -> decl %d\n", $$);
		}
	|
		decl COMMA decllist
		{
			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("decllist -> decl COMMA decllist %d\n", $$);
		}
	;

mathexpr:	
		TYPEOF mathexpr
		{
			$$ = newNode(pd, node_typeof, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("mathexpr -> TYPEOF mathexpr %d\n", $$);
		}
	|	INCR lval
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (debug) printf("mathexpr -> INCR lval %d\n", $$);
		}
	|
		DECR lval
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (debug) printf("mathexpr -> DECR lval %d\n", $$);
		}
	|	lval INCR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (debug) printf("mathexpr -> lval INCR %d\n", $$);
		}
	|
		lval DECR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (debug) printf("mathexpr -> lval INCR %d\n", $$);
		}

	|	mathexpr RSHIFT mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_rshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr RSHIFT mathexpr %d\n", $$);
		}
	|	mathexpr LSHIFT mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr LSHIFT mathexpr %d\n", $$);
		}
	|	mathexpr LOR mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lor;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr LOR mathexpr %d\n", $$);
		}
	|	mathexpr LAND mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_land;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr LAND mathexpr %d\n", $$);
		}
	|	mathexpr LT mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_lt;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr LT mathexpr %d\n", $$);
		}
	|	mathexpr LE mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_le;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr LE mathexpr %d\n", $$);
		}
	|	mathexpr EQ mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_eq;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr EQ mathexpr %d\n", $$);
		}
	|	mathexpr NEQ mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_ne;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr NEQ mathexpr %d\n", $$);
		}
	|	mathexpr GE mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_ge;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr GE mathexpr %d\n", $$);
		}
	|	mathexpr GT mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_gt;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr GT mathexpr %d\n", $$);
		}
	|	mathexpr PLUS mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_add;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr PLUS mathexpr %d\n", $$);
		}
	|	mathexpr MINUS mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_sub;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr MINUS mathexpr %d\n", $$);
		}
	|	mathexpr MPY mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mpy;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr MPY mathexpr %d\n", $$);
		}
	|	mathexpr MOD mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mod;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr MOD mathexpr %d\n", $$);
		}
	|	mathexpr DIV mathexpr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_div;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("mathexpr -> mathexpr DIV mathexpr %d\n", $$);
		}
	|	MINUS mathexpr %prec UMINUS
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_uminus;
			en->expr = $2;

			if (debug) printf("mathexpr -> MINUS mathexpr %d\n", $$);
		}
	|	NOT mathexpr %prec UNEGATE
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = neg_not;
			en->expr = $2;

			if (debug) printf("mathexpr -> NOT mathexpr %d\n", $$);
		}
	|	LPAR reqmathlist RPAR
		{
			$$ = $2;
			if (debug) printf("mathexpr -> LPAR reqmathlist RPAR\n");
		}
	|	lval ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval ASSIGN mathexpr %d\n", $$);
		}
	|	lval ASSIGN funcexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval ASSIGN funcexpr %d\n", $$);
		}
	|	lval ASSIGN objarraylit
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval ASSIGN objarraylit %d\n", $$);
		}
	|	lval LSHIFT_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_lshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval LSHIFT_ASSIGN mathexpr %d\n", $$);
		}
	|	lval RSHIFT_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_rshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval RSHIFT_ASSIGN mathexpr %d\n", $$);
		}
	|	lval PLUS_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_add;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval PLUS_ASSIGN mathexpr %d\n", $$);
		}
	|	lval MINUS_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_sub;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval MINUS_ASSIGN mathexpr %d\n", $$);
		}
	|	lval MPY_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_mpy;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval MPY_ASSIGN mathexpr %d\n", $$);
		}
	|	lval MOD_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_mod;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval MOD_ASSIGN mathexpr %d\n", $$);
		}
	|	lval DIV_ASSIGN mathexpr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_div;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval DIV_ASSIGN mathexpr %d\n", $$);
		}
	|	expr
		{
			$$ = $1;
			if (debug) printf("mathexpr -> expr\n");
		}
	;

expr:
		NUM
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
	|	expr LPAR arglist RPAR
		{
			$$ = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + $$);
			fc->name = $1;
			fc->args = $3;

			if (debug) printf("expr -> expr[%d] LPAR arglist[%d] RPAR %d\n", $1, $3, $$);
		}
	|	NEW symbol LPAR arglist RPAR
		{
			$$ = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + $$);
			fc->hdr->flag |= flag_newobj;
			fc->name = $2;
			fc->args = $4;

			if (debug) printf("expr -> NEW expr LPAR arglist RPAR %d\n", $$);
		}
	|	lval
		{
			if (debug) printf("expr -> lval\n");
			$$ = $1;
		}
	;

arrayelem:
		mathexpr
		{
			if (debug) printf("arrayelem -> mathexpr\n");
			$$ = $1;
		}
	|	funcexpr
		{
			if (debug) printf("arrayelem -> funcexpr\n");
			$$ = $1;
		}
	|	objarraylit
		{
			if (debug) printf("arrayelem -> objarraylit\n");
			$$ = $1;
		}
	;

arraylist:
		%empty
		{
			$$ = 0;
			if (debug) printf("arraylist -> _empty_\n");
		}
	|	arrayelem
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("arraylist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("arraylist -> arrayelem %d\n", $$);
		}
	|	arrayelem COMMA arraylist
		{
			if ($3) {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("arraylist -> arrayelem COMMA arraylist %d\n", $$);
			} else {
				$$ = newNode(pd, node_endlist, sizeof(listNode), true);
				if (debug) printf("arraylist -> endlist %d\n", $$);

				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("arraylist -> arrayelem %d\n", $$);
			}
		}
	;

lval:
		symbol
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
				printf("lval -> expr DOT NAME[%.*s] %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	|	expr LBRACK mathexpr RBRACK
		{
			$$ = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("lval -> expr LBRACK mathexpr RBRACK %d\n", $$);
		}
	;

reqmathlist:		// non-empty
		mathexpr 
		{
			$$ = $1;
			if (debug) printf("reqmathlist -> mathexpr %d\n", $$);
		}
	|	mathexpr COMMA reqmathlist
		{
			if (pd->table[$3].type != node_list) { 
				$$ = newNode(pd, node_endlist, sizeof(listNode), true);
				if (debug)
					printf("reqmathlist -> endlist %d\n", $$);
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $3;
			}

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug)
				printf("reqmathlist -> mathexpr COMMA reqmathlist %d\n", $$);
		}
	;

mathlist:
		%empty
		{
			$$ = 0;
			if (debug) printf("mathlist -> _empty_\n");
		}
	|	reqmathlist
		{
			$$ = $1;
			if (debug) printf("mathlist -> reqmathlist %d\n", $$);
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

	|	LBRACK arraylist RBRACK
		{
			$$ = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + $$);
			an->exprlist = $2;

			if (debug) printf("objarraylit -> LBRACK arraylist RBRACK %d\n", $$);
		}
	;

elem:
		NAME COLON mathexpr
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
	|	STRING COLON mathexpr
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
	|	NAME COLON funcexpr
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> NAME[%.*s] COLON funcexpr %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	|	NAME COLON objarraylit
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> NAME[%.*s] COLON objarraylit %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	|	STRING COLON funcexpr
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> STRING[%.*s] COLON funcexpr %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	|	STRING COLON objarraylit
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> STRING[%.*s] COLON objarraylit %d\n", sn->hdr->aux, sn->string, $$);
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
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("elemlist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("elemlist -> elem %d\n", $$);
		}
	|	elem COMMA elemlist
		{
			if ($3) {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("elemlist -> elem COMMA elemlist %d\n", $$);
			} else {
				$$ = newNode(pd, node_endlist, sizeof(listNode), true);
				if (debug) printf("elemlist -> endlist %d\n", $$);

				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("elemlist -> elem %d\n", $$);
			}
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
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("paramlist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_decl;
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("paramlist -> symbol %d\n", $$);
		}
	|	symbol COMMA paramlist
		{
			if ($3) {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				symNode *sym = (symNode *)(pd->table + $1);
				sym->hdr->flag |= flag_decl;
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("paramlist -> symbol COMMA paramlist %d\n", $$);
			} else {
				$$ = newNode(pd, node_endlist, sizeof(listNode), true);
				if (debug) printf("paramlist -> endlist %d\n", $$);

				$$ = newNode(pd, node_list, sizeof(listNode), false);
				symNode *sym = (symNode *)(pd->table + $1);
				sym->hdr->flag |= flag_decl;
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;

				if (debug) printf("paramlist -> symbol %d\n", $$);
			}
		}
	;

arg:
		AMPER symbol
		{
			if (debug) printf("arg -> AMPER symbol\n");
			symNode *sym = (symNode *)(pd->table + $2);
			sym->hdr->type = node_ref;
			$$ = $2;
		}
	|	mathexpr
		{
			if (debug) printf("arg -> mathexpr\n");
			$$ = $1;
		}
	|	funcexpr
		{
			if (debug) printf("arg -> funcexpr\n");
			$$ = $1;
		}
	|	objarraylit
		{
			if (debug) printf("arg -> objarraylit\n");
			$$ = $1;
		}
	;

arglist:
		%empty
		{
			$$ = 0;
			if (debug) printf("arglist -> _empty_\n");
		}
	|	arg
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("arglist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("arglist -> arg[%d] %d\n", $1, $$);
		}
	|	arg COMMA arglist
		{
			if ($1) {
				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;
			} else {
				$$ = newNode(pd, node_endlist, sizeof(listNode), true);
				if (debug) printf("arglist -> endlist %d\n", $$);

				$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $1;
			}

			if (debug) printf("arglist -> arg COMMA arglist %d\n", $$);
		}
	;

%%

void yyerror( void *scanner, parseData *pd, char *s, ... )
{
	fprintf(stderr, "error in %s: line: %d %s\n", pd->script, pd->lineno, s);
}

