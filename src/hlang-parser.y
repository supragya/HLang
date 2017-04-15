/* HLang Parser Bison 
 * created by Supragya Raj
 */

%{
#include <stdio.h>
#include "buildtime_hlang-lexer.h"
#include "hlang-lexer.h"
#include "hlang-parser.h"
#include "variable_mgmt.h"

%}
%define api.value.type {char *}

/* Terminals */
%token	MAPDECL	VARDECL	VARNAME	MELNAME	ARGVAR
%token	NVAL	STRING	BROPEN	BRCLOSE	SHELLECHO
%token	FUNC	IF	ELIF	ELSE	WHILE
%token	EOS	PARANOPEN	PARANCLOSE
%token	ASSIGN	FUNCCALL	COMMA
%token	GT	LT	EQ	NQ	GE	LAND	LOR
%token	LE	ERR	EOL

%%

input:	%empty
	|input function 			
	;

function: functionname enclosement		
	;

functionname: FUNC			{printf("<FUNCTION|%s>\n",yylval);}
	;

enclosement: BROPEN code BRCLOSE
	;

code:	%empty
	|code sequential		{printf("<SIQ>\n");}
	|code selective			{printf("<SEL>\n");}
	|code iterative			{printf("<ITR>\n");}
	;

sequential: stmt EOS
	;

selective: if elseifs else
	;

if:	IF conditions enclosement
	;

elseifs:%empty
	|elseifs elseif
	;

elseif:	ELIF enclosement
	;

else:	%empty
	|ELSE enclosement
	;

conditions: BROPEN relopr BRCLOSE
	|BROPEN VARNAME relopr STRING BRCLOSE	{printf("<IFPOPENCLOSE>\n");}
	;

iterative: WHILE conditions enclosement
	;

relopr:	GT
	|LT
	|EQ
	|NQ
	|GE
	|LE
	;

stmt:	VARDECL varlist		{printf("<VARDECL|%s>\n", yylval);}
	|MAPDECL maplist	{printf("<MAPDECL|%s>\n", yylval);}
	|PARANOPEN VARNAME relopr STRING PARANCLOSE	{printf("<TSTPOPENCLOSE>\n");}
	;

varlist:var
	|varlist COMMA var
	;

maplist:mapvar
	|maplist COMMA mapvar
	;

var:	VARNAME				{printf("<VARNAME|%s>\n",yylval); }
	|var ASSIGN string		{printf("<VAR-ASSIGN-FUNCCALL|%s>\n", yylval); }
	|var ASSIGN ARGVAR		{printf("<VAR-ASSIGN-ARGVAR|%s>\n",yylval);}
;

string:	STRING
	|functioncall
	;

functioncall:FUNCCALL arguments PARANCLOSE
	;

arguments:arg otherargs
	;

otherargs: %empty
	|COMMA arguments
	;

arg:	STRING				{printf("<ARGSTRING|%s>\n",yylval);}
	|VARNAME			{printf("<ARGVARNM|%s>\n",yylval);}
	|MELNAME			{printf("<ARGMLNAME|%s>\n",yylval);}
	;

mapvar:	VARNAME
	;
%%

int yyerror(const char *s){
	fprintf(stdout, "{{error: %s}}\n", s);
	return 0;
}
