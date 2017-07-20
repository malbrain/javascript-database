%{
#undef YYSTYPE

#include "js.h"
#include "js.tab.h"
#include "js_malloc.h"

#undef YY_BUF_SIZE
#define YY_BUF_SIZE (16384 - sizeof(rawobj_t) - 2)

void *yyalloc (yy_size_t size,yyscan_t yyscanner ) {
	return js_alloc((uint32_t)size, false);
}

void *yyrealloc (void *old,yy_size_t size,yyscan_t yyscanner ) {
	return js_realloc(old, (uint32_t *)&size, false);
}

void yyfree (void *old,yyscan_t yyscanner ) {
	if (old)
		js_free(old);
}

#define YY_DECL int yylex \
	(YYSTYPE * yylval_param, yyscan_t yyscanner, parseData *pd)
%}

%option noyywrap nodefault reentrant bison-bridge
%option noyyalloc noyyrealloc noyyfree
%option header-file="js.lex.h"
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
"&="		{ return AND_ASSIGN; }		
"^="		{ return XOR_ASSIGN; }		
"|="		{ return OR_ASSIGN; }		
"/="		{ return DIV_ASSIGN; }
"%="		{ return MOD_ASSIGN; }
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

"~"			{ return BITNOT; }
"&"			{ return BITAND; }
"^"			{ return BITXOR; }
"|"			{ return BITOR; }

"++"		{ return INCR; }		
"--"		{ return DECR; }		
"<"			{ return LT; }
"<="		{ return LE; }
"=="		{ return EQ; }
"!="		{ return NEQ; }
">="		{ return GE; }
">"			{ return GT; }
">>"		{ return RSHIFT; }
">>>"		{ return RUSHIFT; }
"<<"		{ return LSHIFT; }
"||"		{ return LOR; }
"&&"		{ return LAND; }
"!=="		{ return NOTIDENTICAL; };
"==="		{ return IDENTICAL; };

	/* keywords */
"if"		{ return IF; }
"else"		{ return ELSE; }
"while"		{ return WHILE; }
"do"		{ return DO; }
"for"		{ return FOR; }
"function"	{ return FCN; }
"var"		{ return VAR; }
"let"		{ return LET; }
"new"		{ return NEW; }
"return"	{ return RETURN; }
"continue"	{ return CONTINUE; }
"break"		{ return BREAK; }
"enum"		{ return ENUM; }
"typeof"	{ return TYPEOF; }
"in"		{ return FORIN; }
"of"		{ return FOROF; }
"try"		{ return TRY; }
"catch"		{ return CATCH; }
"finally"	{ return FINALLY; }
"throw"		{ return THROW; }
"delete"	{ return DEL; }

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
		uint32_t len = yyleng;
		yylval->slot = newNode(pd, node_string, sizeof(stringNode) + len + 1, false);
		stringNode *sn = (stringNode *)(pd->table + yylval->slot);
		memcpy (sn->str.val, yytext, len);
		sn->str.val[len] = 0;
		sn->str.len = len;
		return NAME;
	}

	/* strings */
'(\\.|[^\'])*' {
		yylval->slot = newStrNode(pd, yytext, yyleng);
		return STRING;
	}

\"(\\.|[^\"])*\" {
		yylval->slot = newStrNode(pd, yytext, yyleng);
		return STRING;
	}

	/* single and multi-line comments */
"//".*

"/*"([^*]|(\*+[^*/]))*\*+\/ {
		for (int i = 0; i < yyleng; i++)
		  if (yytext[i] == '\n')
			pd->lineNo++;
	}

	/* numbers */
0[bB][0-1]+ {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->intval = strtoull(yytext, NULL, 2);
		nn->hdr->aux = nn_int;
		return NUM;
	}

0[oO][0-7]+ {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->intval = strtoull(yytext, NULL, 8);
		nn->hdr->aux = nn_int;
		return NUM;
	}

0[xX][0-9a-fA-F]+ {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->intval = strtoull(yytext, NULL, 16);
		nn->hdr->aux = nn_int;
		return NUM;
	}

[0-9]+	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->intval = strtoll(yytext, NULL, 10);
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
		pd->lineNo++;
	}
	/* the rest */
[ \t\r]		/* ignore whitespace */
.				{ printf("script: %s line:%d Mystery character <%.2X>\n", pd->script, pd->lineNo, *yytext); }
<<EOF>>		{ return EOS; }
%%
