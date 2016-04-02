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

static bool debug = false;

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
%token			RBRACK
%token			SEMI
%token			ENUM
%token			INCR
%token			DECR

%right			RPAR ELSE
%precedence		PLUS_ASSIGN MINUS_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN ASSIGN
%left			LT LE EQ NEQ GT GE
%left			LSHIFT RSHIFT
%left			PLUS MINUS
%left			TIMES DIV
%precedence		LBRACK LPAR
%precedence		DOT
%precedence		UMINUS

%type <slot>	enum enumlist
%type <slot>	expr exprlist
%type <slot>	decl decllist
%type <slot>	arg arglist
%type <slot>	stmt stmtlist
%type <slot>	paramlist
%type <slot>	elem elemlist
%type <slot>	lval objlit
%type <slot>	arrayelem arraylist 
%type <slot>	funcdef funcexpr
%type <slot>	fname pgmlist
%type <slot>	symbol optexpr

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
			if (debug) printf("pgmlist -> _empty_\n");
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
				if (debug) printf("__emptystmt__\n");
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
		IF LPAR expr RPAR stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = 0;

			if (debug) printf("stmt -> IF LPAR expr RPAR stmt %d\n", $$);
		}
	|	IF LPAR expr RPAR stmt ELSE stmt
		{
			$$ = newNode(pd, node_ifthen, sizeof(ifThenNode), false);
			ifThenNode *ifthen = (ifThenNode *)(pd->table + $$);
			ifthen->condexpr = $3;
			ifthen->thenstmt = $5;
			ifthen->elsestmt = $7;

			if (debug) printf("stmt -> IF LPAR expr RPAR stmt ELSE stmt %d\n", $$);
		}
	|	RETURN optexpr SEMI
		{
			$$ = newNode(pd, node_return, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("stmt -> RETURN optexpr SEMI %d\n", $$);
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
	|	WHILE LPAR expr RPAR stmt
		{
			$$ = newNode(pd, node_while, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $3;
			wn->stmt = $5;

			if (debug) printf("stmt -> WHILE LPAR expr RPAR stmt %d\n", $$);
		}
	|	DO stmt WHILE LPAR expr RPAR SEMI
		{
			$$ = newNode(pd, node_dowhile, sizeof(whileNode), false);
			whileNode *wn = (whileNode *)(pd->table + $$);
			wn->cond = $5;
			wn->stmt = $2;

			if (debug) printf("stmt -> DO stmt WHILE LPAR expr RPAR SEMI %d\n", $$);
		}
	|	FOR LPAR VAR decllist SEMI expr SEMI expr RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $4;
			fn->cond = $6;
			fn->incr = $8;
			fn->stmt = $10;

			if (debug) printf("stmt -> FOR LPAR expr SEMI expr SEMI expr RPAR stmt %d\n", $$);
		}
	|	FOR LPAR expr SEMI expr SEMI expr RPAR stmt
		{
			$$ = newNode(pd, node_for, sizeof(forNode), false);
			forNode *fn = (forNode *)(pd->table + $$);
			fn->init = $3;
			fn->cond = $5;
			fn->incr = $7;
			fn->stmt = $9;

			if (debug) printf("stmt -> FOR LPAR expr SEMI expr SEMI expr RPAR stmt %d\n", $$);
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

			if (debug) printf("stmt -> ENUM lval LBRACE enumlist RBRACE \n");
		}
	|	VAR decllist SEMI
		{
			if (debug) printf("stmt -> VAR decllist SEMI\n");
			$$ = $2;
		}
	|	exprlist SEMI
		{
			if (debug) printf("stmt -> exprlist SEMI\n");
			$$ = $1;
		}
	|	SEMI
		{
			if (debug) printf("stmt -> _empty_\n");
			$$ = 0;
		}
	;

stmtlist: 
		%empty
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("stmtlist -> _empty_ %d\n", $$);
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
	|	symbol ASSIGN expr
		{
			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval | flag_decl;

			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("decl -> symbol ASSIGN expr[%d] %d\n", $3, $$);
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
	|	symbol ASSIGN objlit
		{
			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval | flag_decl;

			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("decl -> symbol ASSIGN objlit %d\n", $$);
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

expr:	
		INCR symbol
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (debug) printf("expr -> INCR symbol %d\n", $$);
		}
	|
		DECR symbol
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_before;
			en->expr = $2;

			Node *node = pd->table + $2;
			node->flag |= flag_lval;

			if (debug) printf("expr -> DECR symbol %d\n", $$);
		}
	|	symbol INCR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = incr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (debug) printf("expr -> symbol INCR %d\n", $$);
		}
	|
		symbol DECR
		{
			$$ = newNode(pd, node_incr, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->hdr->aux = decr_after;
			en->expr = $1;

			Node *node = pd->table + $1;
			node->flag |= flag_lval;

			if (debug) printf("expr -> symbol INCR %d\n", $$);
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
	|	expr TIMES expr
		{
			$$ = newNode(pd, node_math, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = math_mpy;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> expr TIMES expr %d\n", $$);
			if (debug) printf("expr -> expr TIMES expr\n");
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
	|	LPAR exprlist RPAR
		{
			if (debug) printf("expr -> LPAR exprlist RPAR\n");
			$$ = $2;
		}
	|	MINUS expr %prec UMINUS
		{
			$$ = newNode(pd, node_neg, sizeof(exprNode), false);
			exprNode *en = (exprNode *)(pd->table + $$);
			en->expr = $2;

			if (debug) printf("expr -> MINUS expr %d\n", $$);
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
	|	lval ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval ASSIGN expr %d\n", $$);
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
	|	lval ASSIGN objlit
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_assign;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval ASSIGN objlit %d\n", $$);
		}
	|	lval LSHIFT_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_lshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval PLUS_ASSIGN expr %d\n", $$);
		}
	|	lval RSHIFT_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_rshift;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval PLUS_ASSIGN expr %d\n", $$);
		}
	|	lval PLUS_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_add;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval PLUS_ASSIGN expr %d\n", $$);
		}
	|	lval MINUS_ASSIGN expr
		{
			symNode *sym = (symNode *)(pd->table + $1);
			sym->hdr->flag |= flag_lval;

			$$ = newNode(pd, node_assign, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->hdr->aux = pm_sub;
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("expr -> lval MINUS_ASSIGN expr %d\n", $$);
		}
	|	expr LPAR arglist RPAR
		{
			$$ = newNode(pd, node_fcncall, sizeof(fcnCallNode), false);
			fcnCallNode *fc = (fcnCallNode *)(pd->table + $$);
			fc->name = $1;
			fc->args = $3;

			if (debug) printf("expr -> expr[%d] LPAR arglist RPAR %d\n", $1, $$);
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
	|	LBRACK arraylist RBRACK
		{
			$$ = newNode(pd, node_array, sizeof(arrayNode), false);
			arrayNode *an = (arrayNode *)(pd->table + $$);
			an->exprlist = $2;

			if (debug) printf("expr -> LBRACK arraylist RBRACK %d\n", $$);
		}
	|	lval
		{
			if (debug) printf("expr -> lval\n");
			$$ = $1;
		}
	;

optexpr:
		%empty
		{
			if (debug) printf("optexpr -> _empty_\n");
			$$ = 0;
		}
	|	expr
		{
			if (debug) printf("optexpr -> expr\n");
			$$ = $1;
		}
	;

arrayelem:
		expr
		{
			if (debug) printf("arrayelem -> expr\n");
			$$ = $1;
		}
	|	funcexpr
		{
			if (debug) printf("arrayelem -> funcexpr\n");
			$$ = $1;
		}
	|	objlit
		{
			if (debug) printf("arrayelem -> objlit\n");
			$$ = $1;
		}
	;

arraylist:
		%empty
		{
			$$ = 0;
			if (debug) printf("arraylist -> _empty_ %d\n", $$);
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
	|	expr LBRACK expr RBRACK
		{
			$$ = newNode(pd, node_lookup, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) printf("lval -> expr LBRACK expr RBRACK %d\n", $$);
		}
	;

exprlist:		// non-empty
		expr 
		{
			$$ = $1;
			if (debug) printf("exprlist -> expr %d\n", $$);
		}
	|	expr COMMA exprlist
		{
			if (pd->table[$3].type != node_list) { 
				$$ = newNode(pd, node_endlist, sizeof(listNode), true);
				if (debug) printf("exprlist -> endlist %d\n", $$);
					$$ = newNode(pd, node_list, sizeof(listNode), false);
				listNode *ln = (listNode *)(pd->table + $$);
				ln->elem = $3;
			}
				$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

				if (debug) printf("exprlist -> expr COMMA exprlist %d\n", $$);
		}
	;

objlit:
		LBRACE elemlist RBRACE 
		{
			$$ = newNode(pd, node_obj, sizeof(objNode), false);
			objNode *on = (objNode *)(pd->table + $$);
			on->elemlist = $2;

			if (debug) printf("objlit -> LBRACE elemlist RBRACE %d\n", $$);
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
	|	NAME COLON objlit
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> NAME[%.*s] COLON objlit %d\n", sn->hdr->aux, sn->string, $$);
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
	|	STRING COLON objlit
		{
			$$ = newNode(pd, node_elem, sizeof(binaryNode), false);
			binaryNode *bn = (binaryNode *)(pd->table + $$);
			bn->right = $3;
			bn->left = $1;

			if (debug) {
				stringNode *sn = (stringNode *)(pd->table + $1);
				printf("elem -> STRING[%.*s] COLON objlit %d\n", sn->hdr->aux, sn->string, $$);
			}
		}
	;

elemlist:
		%empty
		{
			$$ = 0;
			if (debug) printf("elemlist -> _empty_ %d\n", $$);
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
			if (debug) printf("paramlist -> _empty_ %d\n", $$);
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
	|	expr
		{
			if (debug) printf("arg -> expr\n");
			$$ = $1;
		}
	|	funcexpr
		{
			if (debug) printf("arg -> funcexpr\n");
			$$ = $1;
		}
	|	objlit
		{
			if (debug) printf("arg -> objlit\n");
			$$ = $1;
		}
	;

arglist:
		%empty
		{
			$$ = 0;
			if (debug) printf("arglist -> endlist %d\n", $$);
		}
	|	arg
		{
			$$ = newNode(pd, node_endlist, sizeof(listNode), true);
			if (debug) printf("arglist -> endlist %d\n", $$);

			$$ = newNode(pd, node_list, sizeof(listNode), false);
			listNode *ln = (listNode *)(pd->table + $$);
			ln->elem = $1;

			if (debug) printf("arglist -> arg %d\n", $$);
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

