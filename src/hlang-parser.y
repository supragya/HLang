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

functionname: FUNC			{printf("found function %s\n",yylval);}
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

selective: if other_ifs
	;

other_ifs: elseifs else
	|else
	;

if:	IF conditions enclosement	{printf("<IF>\n");}
	;

elseifs: %empty
	|elseifs elseif					{printf("<ELSEIF>\n");}
	;

elseif:	ELIF conditions enclosement
	;

else:	ELSE enclosement				{printf("<ELSE>\n");}
	;

conditions: PARANOPEN condition_in PARANCLOSE
	;

condition_in: andonlyconditions
	|condition_in LOR andonlyconditions
	;

andonlyconditions: basicconditions
	|andonlyconditions LAND basicconditions	{printf("andonly %s\n", yylval);} 
	;

basicconditions: VARNAME relopr STRING
	|VARNAME relopr NVAL
	;

iterative: WHILE PARANOPEN conditions PARANCLOSE enclosement
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

int yyerror(const char *s){
	fprintf(stderr, "error: %s\n", s);
	return 0;
}
