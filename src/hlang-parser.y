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
%token	NSTRING	STRING	GSTRING	BROPEN	BRCLOSE	SHELLECHO
%token	FUNC	IF	ELIF	ELSE	WHILE	RETURN	BREAK	CONTINUE	FOR	IS
%token	EOS	PARANOPEN	PARANCLOSE
%token	ASSIGN	FUNCCALL	COMMA
%token	GT	LT	EQ	NQ	GE	LAND	LOR
%token	LE	ERR	EOL
%token	ADD	INCR	SUB	DECR	MUL	EXP	DIV	TRUNCDIV

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
	|code iterative_constructs			{printf("\t<CODE: ITERATIVE CONSTRUCTIS>\n");}
	|code SHELLECHO	EOS				{printf("\t<CODE: SHELL ECHO>\n");}
	|code functioncall EOS				{printf("\t<CODE: FUNCTIONCALL>\n");}
	|code variable_assignment EOS			{printf("\t<CODE: VARIABLE ASSIGNMENT>\n");}
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
	|variablename BROPEN mapelementsinit BRCLOSE	{printf("\t<LASTVARIABLE MAP MAPINIT>\n");}
	;

mapelementsinit:
	midmapelementinit lastmapelementinit		{printf("\t<MAPELEMENTSINIT>\n");}
	;

midmapelementinit:
	%empty						{printf("\t<MIDMAPELEMENTINIT: NONE>\n");}
	|midmapelementinit COMMA lastmapelementinit	{printf("\t<MIDMAPELEMENTINIT: NEW>\n");}
	;

lastmapelementinit:
	GSTRING IS NSTRING				{printf("\t<LASTMAPELEMENTINIT GSTRING IS NSTRING>\n");}
	|GSTRING IS STRING				{printf("\t<LASTMAPELEMENTINIT GSTRING IS STRING>\n");}
	;

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
	|variablename ASSIGN assignval			{printf("\t<LASTVARIABLE GEN: VARNAME ASSIGN ASSIGNVAL: %s>\n", $3);}
	;

assignval:
	STRING						{printf("\t<ASSIGNVAL STRING>\n");}
	|NSTRING					{printf("\t<ASSIGNVAL NSTRING>\n");}
	|VARNAME					{printf("\t<ASSIGNVAL VARNAME>\n");}
	|ARGVAR						{printf("\t<ASSIGNVAL ARGVAR>\n");}
	|SHELLECHO					{printf("\t<ASSIGNVAL SHELLECHO\n");}
	|functioncall					{printf("\t<ASSIGNVAL FUNCTIONCALL>\n");}
	|arithmetic_expr				{printf("\t<ASSIGNVAL ARITHMETIC EXPR>\n");}
	;

variablename:
	VARNAME						{printf("\t<VARIABLENAME:%s>\n",yylval);}
	|MELNAME					{printf("\t<MELNAME:%s>\n",yylval);}
	;

variable_assignment:
	variablename ASSIGN assignval			{printf("\t<VARIABLE ASSIGNMENT: VARAIBLENAME ASSIGN ASSIGNVAL>\n");}
	;

selective_constructs:
	if_part elif_parts else_part			{printf("\t<SELECTIVE CONSTRUCTS>\n");}
	;

if_part:
	IF PARANOPEN conditions PARANCLOSE enclosement			{printf("\t<IF PART>\n");}
	;

elif_parts:
	%empty						{printf("\t<ELIF PARTS: NONE>\n");}
	|elif_parts elif_part				{printf("\t<ELIF PARTS: NEW>\n");}
	;

elif_part:
	ELIF PARANOPEN conditions PARANCLOSE enclosement			{printf("\t<ELIF PART: FOUND SINGULAR>\n");}
	;

else_part:
	%empty						{printf("\t<ELSE PART: NONE>\n");}
	|ELSE conditions enclosement			{printf("\t<ELSE PART: FOUND>\n");}
	;

conditions:
	condition					{printf("\t<CONDITIONS>\n");}
	|conditions LOR condition			{printf("\t<CONDITIONS: OR FOUND>\n");}
	;

condition:
	VARNAME relopr NSTRING				{printf("\t<CONDITION: VARNAME RELOPR NSTRING>\n");}
	|NSTRING relopr VARNAME				{printf("\t<CONDITION: NSTRING RELOPR VARNAME>\n");}
	|VARNAME relopr STRING				{printf("\t<CONDITION: VARNAME RELOPR STRING>\n");}
	|STRING relopr VARNAME				{printf("\t<CONDITION: STRING RELOPR VARNAME>\n");}
	|VARNAME relopr functioncall			{printf("\t<CONDITION: VARNAME RELOPR FUNCTIONCALL>\n");}
	|functioncall relopr VARNAME			{printf("\t<CONDITION: FUNCTIONCALL RELOPR VARNAME>\n");}
	|VARNAME relopr SHELLECHO			{printf("\t<CONDITION: VARNAME RELOPR SHELLECHO>\n");}
	|SHELLECHO relopr VARNAME			{printf("\t<CONDITION: SHELLECHO RELOPR VARNAME>\n");}
	|MELNAME relopr NSTRING				{printf("\t<CONDITION: MELNAME RELOPR NSTRING>\n");}
	|NSTRING relopr MELNAME				{printf("\t<CONDITION: NSTRING RELOPR MELNAME>\n");}
	|MELNAME relopr STRING				{printf("\t<CONDITION: MELNAME RELOPR STRING>\n");}
	|STRING relopr MELNAME				{printf("\t<CONDITION: STRING RELOPR MELNAME>\n");}
	|MELNAME relopr functioncall			{printf("\t<CONDITION: MELNAME RELOPR FUNCTIONCALL>\n");}
	|functioncall relopr MELNAME			{printf("\t<CONDITION: FUNCTIONCALL RELOPR MELNAME>\n");}
	|MELNAME relopr SHELLECHO			{printf("\t<CONDITION: MELNAME RELOPR SHELLECHO>\n");}
	|SHELLECHO relopr MELNAME			{printf("\t<CONDITION: SHELLECHO RELOPR MELNAME>\n");}
	|functioncall relopr NSTRING			{printf("\t<CONDITION: FUNCTIONCALL RELOPR NSTRING>\n");}
	|NSTRING relopr functioncall			{printf("\t<CONDITION: NSTRING RELOPR FUNCTIONCALL>\n");}
	|functioncall relopr STRING			{printf("\t<CONDITION: FUNCTIONCALL RELOPR STRING>\n");}
	|STRING relopr functioncall			{printf("\t<CONDITION: STRING RELOPR FUNCTIONCALL>\n");}
	|SHELLECHO relopr NSTRING			{printf("\t<CONDITION: SHELLECHO RELOPR NSTRING>\n");}
	|NSTRING relopr SHELLECHO			{printf("\t<CONDITION: NSTRING RELOPR SHELLECHO>\n");}
	|SHELLECHO relopr STRING			{printf("\t<CONDITION: SHELLECHO RELOPR STRING>\n");}
	|STRING relopr SHELLECHO			{printf("\t<CONDITION: STRING RELOPR SHELLECHO>\n");}
	;

relopr:
	EQ						{printf("\t<RELOPR: EQ>\n");}
	|NQ						{printf("\t<RELOPR: NQ>\n");}
	|LT						{printf("\t<RELOPR: LT>\n");}
	|GT						{printf("\t<RELOPR: GT>\n");}
	|LE						{printf("\t<RELOPR: LE>\n");}
	|GE						{printf("\t<RELOPR: GE>\n");}
	;

functioncall:
	GSTRING PARANOPEN funccallargs PARANCLOSE	{printf("\t<FUNCTION CALL>\n");}

funccallargs:
	%empty						{printf("\t<FUNCTION CALL ARGUMENTS: NONE>\n");}
	|mid_funccallargs last_funccallarg		{printf("\t<FUNCTION CALL ARGUMENTS: NEW>\n");}
	;

mid_funccallargs:
	%empty						{printf("\t<FUNCTION CALL ARGUMENTS MID: NONE>\n");}
	|mid_funccallargs last_funccallarg COMMA	{printf("\t<FUNCTION CALL ARGUMENTS MID: FOUND NEW>\n");}
	;

last_funccallarg:
	NSTRING						{printf("\t<FUNCTION CALL ARGUMENTS LAST: NSTRING>\n");}
	|STRING						{printf("\t<FUNCTION CALL ARGUMENTS LAST: STRING>\n");}
	|VARNAME					{printf("\t<FUNCTION CALL ARGUMENTS LAST: VARNAME>\n");}
	|ARGVAR						{printf("\t<FUNCTION CALL ARGUMENTS LAST: ARGVAR>\n");}
	|MELNAME					{printf("\t<FUNCTION CALL ARGUMENTS LAST: MELNAME>\n");}
	|functioncall					{printf("\t<FUNCTION CALL ARGUMENTS LAST: FUNCTIONCALL>\n");}
	;

iterative_constructs:
	whileloop					{printf("\t<ITERATIVE CONSTRUCTS: WHILE LOOP>\n");}
	;

whileloop:
	WHILE PARANOPEN conditions PARANCLOSE enclosement	{printf("\t<WHILE LOOP>\n");}
	;

arithmetic_expr:
	variablename ADD NSTRING			{printf("\t<ARITHMETIC EXPR: VARIABLENAME ADD VARIABLENAME>\n");}
	;

/*alltokens: IS|RETURN|BREAK|CONTINUE|FOR|ADD|INCR|SUB|DECR|MUL|EXP|DIV|TRUNCDIV|MAPDECL|VARDECL|VARNAME|MELNAME|ARGVAR|NSTRING|GSTRING|STRING|BROPEN|BRCLOSE|SHELLECHO|FUNC|IF|ELIF|ELSE|WHILE|EOS|PARANOPEN|PARANCLOSE|ASSIGN|FUNCCALL|COMMA|GT|LT|EQ|NQ|GE|LAND|LOR|LE|ERR|EOL;*/
%%

int yyerror(const char *s){
	fprintf(stdout, "{{error: %s}}\n", s);
	return 0;
}
