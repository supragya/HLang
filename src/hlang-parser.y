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

script:
	%empty
	|script function
	/*|script alltokens*/
	;

function:
	functionname enclosement			{printf("\t<FUNCTION>\n");}
	;

functionname:
	FUNC						{printf("\t<FUNCTIONNAME:%s>\n"	,yylval);}
	;

enclosement:
	BROPEN code BRCLOSE				{printf("\t<ENCLOSEMENT>\n");	}
	;

code:
	%empty
	|code variable_declarations 			{printf("\t<CODE: VARIABLE DECLARATIONS>\n");}
	|code selective_constructs			{printf("\t<CODE: SELECTIVE CONSTRUCTS>\n");}
	;

variable_declarations:
	mapvariables_declaration EOS			{printf("\t<VARIABLE DECLARATIONS: MAP>\n");}
	|generalvariables_declaration EOS		{printf("\t<VARIABLE DECLARATIONS: GEN>\n");}
	;

/* Map variables declaration code syntax */
mapvariables_declaration:
	MAPDECL midvariables_map lastvariable_map	{printf("\t<MAPVARIABLE DECLARATIONS>\n");}
	;

midvariables_map:
	%empty
	|midvariables_map lastvariable_map COMMA	{printf("\t<MIDVARIABLES MAP>\n");}
	;

lastvariable_map:
	variablename					{printf("\t<LASTVARIABLE MAP>\n");}

/* General variables declaration code syntax */
generalvariables_declaration:
	VARDECL midvariables_gen lastvariable_gen	{printf("\t<GENVARIABLE DECLARATIONS>\n");}
	;

midvariables_gen:
	%empty
	|midvariables_gen lastvariable_gen COMMA	{printf("\t<MIDVARIABLES GEN>\n");}
	;

lastvariable_gen:
	variablename					{printf("\t<LASTVARIABLE GEN: VARNAME>\n");}
	|variablename ASSIGN STRING			{printf("\t<LASTVARIABLE GEN: VARNAME ASSIGN STRING: %s>\n", $3);}
	;

variablename:
	VARNAME						{printf("\t<VARIABLENAME:%s>\n",yylval);}
	;

selective_constructs:
	if_part elif_parts else_part			{printf("\t<SELECTIVE CONSTRUCTS>\n");}
	;

if_part:
	IF conditions enclosement			{printf("\t<IF PART>\n");}
	;

elif_parts:
	%empty						{printf("\t<ELIF PARTS: NONE>\n");}
	|elif_parts elif_part				{printf("\t<ELIF PARTS: NEW>\n");}
	;

elif_part:
	ELIF conditions enclosement			{printf("\t<ELIF PART: FOUND SINGULAR>\n");}
	;

else_part:
	%empty						{printf("\t<ELSE PART: NONE>\n");}
	|ELSE conditions enclosement			{printf("\t<ELSE PART: FOUND>\n");}
	;

conditions:
	PARANOPEN ASSIGN PARANCLOSE			{printf("\t<TEMP CONDITION>\n");}
	;

/*alltokens: MAPDECL|VARDECL|VARNAME|MELNAME|ARGVAR|NVAL|STRING|BROPEN|BRCLOSE|SHELLECHO|FUNC|IF|ELIF|ELSE|WHILE|EOS|PARANOPEN|PARANCLOSE|ASSIGN|FUNCCALL|COMMA|GT|LT|EQ|NQ|GE|LAND|LOR|LE|ERR|EOL;*/
%%

int yyerror(const char *s){
	fprintf(stdout, "{{error: %s}}\n", s);
	return 0;
}
