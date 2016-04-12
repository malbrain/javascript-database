%{
#undef YYSTYPE
#include "jsdb.h"
#include "jsdb.tab.h"
#define YY_DECL int yylex \
	(YYSTYPE * yylval_param, yyscan_t yyscanner, parseData *pd)
%}

%option noyywrap nodefault reentrant bison-bridge
%option header-file="jsdb.lex.h"
%option extra-type="parseData *"

EXP		([Ee][-+]?[0-9]+)

%%

	/* single charater ops */
"+"			{ return PLUS; }		
"-"			{ return MINUS; }
"*"			{ return MPY; }
"/"			{ return DIV; }
"%"			{ return MOD; }
"="			{ return ASSIGN; }
"+="		{ return PLUS_ASSIGN; }		
"-="		{ return MINUS_ASSIGN; }
"*="		{ return MPY_ASSIGN; }		
"/="		{ return DIV_ASSIGN; }
"<<="		{ return LSHIFT_ASSIGN; }
">>="		{ return RSHIFT_ASSIGN; }
","			{ return COMMA; }
";"			{ return SEMI; }
"("			{ return LPAR; }
")"			{ return RPAR; }
"{"			{ return LBRACE; }
"}"			{ return RBRACE; }
"["			{ return LBRACK; }
"]"			{ return RBRACK; }
":"			{ return COLON; }
"."			{ return DOT; }
"!"			{ return NOT; }
"?"			{ return TERN; }

"++"		{ return INCR; }		
"--"		{ return DECR; }		
"<"			{ return LT; }
"<="		{ return LE; }
"=="		{ return EQ; }
"!="		{ return NEQ; }
">="		{ return GE; }
">"			{ return GT; }
">>"		{ return RSHIFT; }
"<<"		{ return LSHIFT; }
"||"		{ return LOR; }
"&&"		{ return LAND; }

	/* keywords */
"&"			{ return AMPER; }
"if"		{ return IF; }
"else"		{ return ELSE; }
"while"		{ return WHILE; }
"do"		{ return DO; }
"for"		{ return FOR; }
"function"	{ return FCN; }
"var"		{ return VAR; }
"new"		{ return NEW; }
"return"	{ return RETURN; }
"continue"	{ return CONTINUE; }
"break"		{ return BREAK; }
"enum"		{ return ENUM; }
"typeof"	{ return TYPEOF; }

"arguments"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_args;
		return NUM;
	}

"this"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_this;
		return NUM;
	}

	/* booleans */

"false"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_bool;
		nn->boolval = false;
		return NUM;
	}

"true"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_bool;
		nn->boolval = true;
		return NUM;
	}

"undefined"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_undef;
		return NUM;
	}

"NaN"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_nan;
		return NUM;
	}

"Infinity"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_infinity;
		return NUM;
	}

"null"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = nn_null;
		return NUM;
	}

	/* names */
[a-zA-Z$_][a-zA-Z0-9_]* {
		uint32_t len = strlen(yytext);
		yylval->slot = newNode(pd, node_string, sizeof(stringNode) + len, false);
		stringNode *sn = (stringNode *)(pd->table + yylval->slot);
		memcpy (sn->string, yytext, len);
		sn->hdr->aux = len;
		return NAME;
	}

	/* strings */
'(\\.|[^\'])*' {
		yylval->slot = newStrNode(pd, yytext);
		return STRING;
	}

\"(\\.|[^\\"])*\" {
		yylval->slot = newStrNode(pd, yytext);
		return STRING;
	}

	/* single and multi-line comments */
"//".*
"/*"([^*]|(\*+[^*/]))*\*+\/

	/* numbers */
[0-9]+	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->intval = atoi(yytext);
		nn->hdr->aux = nn_int;
		return NUM;
	}

[0-9]+{EXP} {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = nn_dbl;
		return NUM;
	}
[0-9]+"."[0-9]* {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = nn_dbl;
		return NUM;
	}
[0-9]+"."[0-9]*{EXP} {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = nn_dbl;
		return NUM;
	}
"."[0-9]+ {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = nn_dbl;
		return NUM;
	}
"."[0-9]+{EXP} {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = nn_dbl;
		return NUM;
	}
	/* line numbers */
\n	{
		pd->lineno++;
	}
	/* the rest */
[ \t]		/* ignore whitespace */
.				{ printf("Mystery character %c\n", *yytext); }
<<EOF>>		{ return EOS; }
%%
