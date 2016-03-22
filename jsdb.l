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

EXP     ([Ee][-+]?[0-9]+)

%%

	/* single charater ops */
"+"         { return PLUS; }     
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return DIV; }
"="         { return ASSIGN; }
"+="        { return PLUS_ASSIGN; }     
"-="        { return MINUS_ASSIGN; }
","         { return COMMA; }
";"         { return SEMI; }
"("         { return LPAR; }
")"         { return RPAR; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"["         { return LBRACK; }
"]"         { return RBRACK; }
":"         { return COLON; }
"."         { return DOT; }

"<"         { return LT; }
"<="        { return LE; }
"=="        { return EQ; }
"!="        { return NEQ; }
">="        { return GE; }
">"         { return GT; }

	/* keywords */
"&"         { return AMPER; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"for"       { return FOR; }
"function"  { return FCN; }
"var"       { return VAR; }
"return"    { return RETURN; }
"continue"  { return CONTINUE; }
"break"     { return BREAK; }

	/* booleans */
"false"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = vt_bool;
		nn->boolval = false;
		return NUM;
	}

"true"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = vt_bool;
		nn->boolval = true;
		return NUM;
	}

"null"	{
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->hdr->aux = vt_null;
		return NUM;
	}

	/* names */
[a-zA-Z$_][a-zA-Z0-9_]* {
        yylval->slot = newNode(pd, node_string, sizeof(stringNode) + strlen(yytext), false);
		stringNode *sn = (stringNode *)(pd->table + yylval->slot);
		sn->hdr->aux = strlen(yytext);
		memcpy (sn->string, yytext, sn->hdr->aux);
        return NAME;
	}

    /* strings */
'[^']*' {
        yylval->slot = newNode(pd, node_string, sizeof(stringNode) + strlen(yytext)-2, false);
		stringNode *sn = (stringNode *)(pd->table + yylval->slot);
		sn->hdr->aux = strlen(yytext) - 2;
		memcpy (sn->string, yytext + 1, sn->hdr->aux);
        return STRING;
    }

\"[^\"]*\" {
        yylval->slot = newNode(pd, node_string, sizeof(stringNode) + strlen(yytext)-2, false);
		stringNode *sn = (stringNode *)(pd->table + yylval->slot);
		sn->hdr->aux = strlen(yytext) - 2;
		memcpy (sn->string, yytext + 1, sn->hdr->aux);
        return STRING;
    }

	/* comments */
"//".*

	/* numbers */
[0-9]+  {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->intval = atoi(yytext);
		nn->hdr->aux = vt_int;
		return NUM;
	}

[0-9]+{EXP} {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = vt_dbl;
		return NUM;
	}
[0-9]+"."[0-9]* {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = vt_dbl;
		return NUM;
	}
[0-9]+"."[0-9]*{EXP} {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = vt_dbl;
		return NUM;
	}
"."[0-9]+ {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = vt_dbl;
		return NUM;
	}
"."[0-9]+{EXP} {
		yylval->slot = newNode(pd, node_num, sizeof(numNode), false);
		numNode *nn = (numNode *)(pd->table + yylval->slot);
		nn->dblval = atof(yytext);
		nn->hdr->aux = vt_dbl;
		return NUM;
	}
	/* line numbers */
\n	{
		pd->lineno++;
	}
	/* the rest */
[ \t]     /* ignore whitespace */
.           { printf("Mystery character %c\n", *yytext); }
<<EOF>>     { return EOS; }
%%
