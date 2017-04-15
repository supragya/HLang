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
%token	GT	LT	EQ	NQ	GE
%token	LE	ERR	EOL

%%

input:	%empty
	|input function 			
	;

function: functionname enclosement		
	;

functionname: FUNC			{printf("found function %s\n",yylval);}

enclosement: BROPEN line BRCLOSE
	;

line:	%empty
	|line stmt EOS
	;

stmt:	VARDECL varlist
	|MAPDECL maplist
	;

varlist:var
	|varlist COMMA var
	;

maplist:mapvar
	|maplist COMMA mapvar
	;

var:	VARNAME				{printf("variablename %s\n",yylval); }
	|var ASSIGN STRING		{printf("ass_svar %s \n", yylval); }
	|var ASSIGN NVAL		{printf("ass_nvar %s \n", yylval); }
	;

mapvar:	VARNAME
	;
%%

int yyerror(char *s){
	fprintf(stderr, "error: %s\n", s);
	return 0;
}
