/* HLang Parser Bison
 * created by Supragya Raj
 */

%{
#include <stdio.h>
#include "buildtime_hlang-lexer.h"
#include "hlang-lexer.h"
#include "hlang-parser.h"
#include "variable_mgmt.h"
#include "ast.h"

%}
%define api.value.type {char *}
%error-verbose

/* Terminals */
%token	MAPDECL	VARDECL	VARNAME	MELNAME	ARGVAR
%token	NSTRING	STRING	GSTRING	BROPEN	BRCLOSE	SHELLECHO
%token	FUNC	IF	ELIF	ELSE	WHILE	RETURN	BREAK	CONTINUE	FOR	IS
%token	EOS	PARANOPEN	PARANCLOSE	EXCLAMATION
%token	ASSIGN	FUNCCALL	COMMA
%token	GT	LT	EQ	NQ	GE	LAND	LOR
%token	LE	ERR	EOL
%token	ADD	INCR	SUB	DECR	MUL	EXP	DIV	TRUNCDIV

%%

script:
	%empty
	|script function
	/*|script alltokens    /* FOR DEBUGGING PROCESS    */
	;

function:
	FUNC enclosement				{printf("\t<FUNCTION>\n"); ast_add_function($1);}
	;

enclosement:
	BROPEN code BRCLOSE				{printf("\t<ENCLOSEMENT>\n");}
	;

code:
	%empty
	|code sequential_constuct			{printf("\t<CODE: SEQUENTIAL CONSTRUCT>\n"); ast_add_seq("Lauda lehsan");}
	|code selective_constructs			{printf("\t<CODE: SELECTIVE CONSTRUCTS>\n");}
	|code iterative_constructs			{printf("\t<CODE: ITERATIVE CONSTRUCTIS>\n");}
	;




/* Different constructs */

sequential_constuct:
	mapvariables_declaration EOS			{printf("\t<SEQUENTIAL CONSTRUCT: MAP VARIABLE DECLARATIONS>\n");}
	|generalvariables_declaration EOS		{printf("\t<SEQUENTIAL CONSTRUCT: GEN VARIABLE DECLARATIONS>\n");}
	|SHELLECHO EOS					{printf("\t<SEQUENTIAL CONSTRUCT: SHELL ECHO>\n");}
	|functioncall EOS				{printf("\t<SEQUENTIAL CONSTRUCT: FUNCTIONCALL>\n");}
	|assignments EOS				{printf("\t<SEQUENTIAL CONSTRUCT: VARIABLE ASSIGNMENT>\n");}
	|return_statement EOS				{printf("\t<SEQUENTIAL CONSTRUCT: RETURN STATEMENT>\n");}
	;

selective_constructs:
	ifblock elseifblocks elseblock			{printf("\t<SELECTIVE: IF ELSEIF ELSE BLOCKS>\n");}
	|ifblock elseifblocks				{printf("\t<SELECTIVE: IF ELSEIF BLOCKS>\n");}
	|ifblock elseblock				{printf("\t<SELECTIVE: IF ELSE BLOCKS>\n");}
	|ifblock					{printf("\t<SELECTIVE: IF ONLY BLOCK>\n");}
	;

ifblock:
	IF PARANOPEN conditions PARANCLOSE enclosement	{printf("\t<IF BLOCK>\n");}

elseblock:
	ELSE enclosement				{printf("\t<ELSE BLOCK>\n");}

elseifblocks:
	ELIF PARANOPEN conditions PARANCLOSE enclosement		{printf("\t<ELSEIF BLOCK: ONE>\n");}
	|elseifblocks ELIF PARANOPEN conditions PARANCLOSE enclosement	{printf("\t<ELSEIF BLOCK: RECURSIVE>\n");}
	;

iterative_constructs:
	whileloop					{printf("\t<ITERATIVE CONSTRUCTS: WHILE LOOP>\n");}
	|forloop					{printf("\t<ITERATIVE CONSTRUCTS: FOR LOOP>\n");}
	;



/* Map variables declaration code syntax */
mapvariables_declaration:
	MAPDECL map_variablelist			{printf("\t<MAPVARIABLE DECLARATIONS: MAP VAR LIST>\n");}
	;

map_variablelist:
	map_discrete_variable				{printf("\t<MAPVARIABLELIST: DISCRETE VARIABLE FOUND>\n");}
	|map_variablelist COMMA map_discrete_variable	{printf("\t<MAPVARIABLELIST: COMMA DISCRETE>\n");}
	;

map_discrete_variable:
	VARNAME						{printf("\t<MAP DISCRETE VARIABLE: VARNAME FOUND| %s>\n", $1);}
	|VARNAME ASSIGN BROPEN keyvalpairs BRCLOSE	{printf("\t<MAP DISCRETE VARIABLE: KEYVALPAIRS>\n");}
	;

keyvalpairs:
	keytype IS datatype				{printf("\t<KEYVALPAIRS: FOUND DISCRETE>\n");}
	|keyvalpairs COMMA keytype IS datatype		{printf("\t<KEYVALPAIRS: FOUND COMMA>\n");}
	|datatype					{printf("\t<KEYVALPAIRS: FOUND JUST DATATYPE>\n");}
	|keyvalpairs COMMA datatype			{printf("\t<KEYVALPAIRS: FOUND COMMA JUST DATATYPE>\n");}
	;

keytype:
	STRING						{printf("\t<KEYTYPE: STRING>\n");}
	|NSTRING					{printf("\t<KEYTYPE: NSTRING>\n");}
	;

datatype:
	STRING						{printf("\t<DATATYPE: STRING>\n");}
	|NSTRING					{printf("\t<DATATYPE: NSTRING>\n");}
	;



/* General variables declaration code syntax */
generalvariables_declaration:
	VARDECL gen_variablelist			{printf("\t<GENVARIABLE DECLARATIONS: GEN VAR LIST>\n");}
	;

gen_variablelist:
	gen_discrete_variable				{printf("\t<GENVARIABLE DECLARATIONS: DISCRETE VARIABLE FOUND>\n");}
	|gen_variablelist COMMA gen_discrete_variable	{printf("\t<GENVARIABLE DECLARATIONS: COMMA DISCRETE>\n");}
	;

gen_discrete_variable:
	VARNAME						{printf("\t<GEN DISCRETE VARIABLE: VARNAME| New variable %s>\n", $1);}
	|VARNAME ASSIGN expression			{printf("\t<GEN DISCRETE VARIABLE: VARNAME ASSIGN EXPRESSION| New variable %s with value %s>\n", $1, $3);}
	|MELNAME					{printf("\t<GEN DISCRETE VARIABLE: MELNAME| New mapelement %s>\n", $1);}
	|MELNAME ASSIGN expression			{printf("\t<GEN DISCRETE VARIABLE: MELNAME ASSIGN EXPRESSION| New element %s with val %s>\n",$1, $3);}
	;



/* Variable assignment sequential constructs */
assignments:
	VARNAME ASSIGN expression			{printf("\t<ASSIGNMENT: VARNAME ASSIGN EXPRESSION>\n");}
	|MELNAME ASSIGN expression			{printf("\t<ASSIGNMENT: MELNAME ASSIGN EXPRESSION>\n");}
	|VARNAME ASSIGN BROPEN keyvalpairs BRCLOSE	{printf("\t<ASSIGNMENT: VARNAME ASSIGN KEYVALPAIRS>\n");}
	|unaryop assignmentvar				{printf("\t<ASSIGNMENT: PRE OPERATION>\n");}
	|assignmentvar unaryop				{printf("\t<ASSIGNMENT: POST OPERATION>\n");}
	;

assignmentvar:
	VARNAME						{printf("\t<ASSIGNMENTVAR: VARNAME>\n");}
	|MELNAME					{printf("\t<ASSIGNMENTVAR: MELNAME>\n");}
	;

unaryop:
	DECR						{printf("\t<ASSIGNMENTVAR: DECREMENT>\n");}
	|INCR						{printf("\t<ASSIGNMENTVAR: INCREMENT>\n");}
	;



/* Arithmetic and otherwise */

expression:
	expression2						{printf("\t<EXPRESSION: EXPRESSION2>\n");}
	|expression ADD expression2				{printf("\t<EXPRESSION: ADD EXPRESSION2>\n");}
	|expression SUB expression2				{printf("\t<EXPRESSION: SUB EXPRESSION2>\n");}
	;

expression2:
	expression3						{printf("\t<EXPRESSION2: EXPRESSION3>\n");}
	|expression2 MUL expression3				{printf("\t<EXPRESSION2: MUL EXPRESSION3>\n");}
	|expression2 DIV expression3				{printf("\t<EXPRESSION2: DIV EXPRESSION3>\n");}
	|expression2 TRUNCDIV expression3			{printf("\t<EXPRESSION2: TRUNCDIV EXPRESSION3>\n");}
	;

expression3:
	expr_unary_preceder PARANOPEN expression PARANCLOSE expr_successor	{$$ = $3; printf("\t<EXPRESSION3: UNARYPREC EXPRESSION EXPRSUCC>\n");}
	|expr_unary_preceder discrete_term expr_successor	{$$ = $2; printf("\t<EXPRESSION3: UNARYPREC TERM EXPRSUCC>\n");}
	;

expr_unary_preceder:
	%empty							{printf("\t<EXPR UNARY PREC: NONE>\n");}
	|SUB							{printf("\t<EXPR UNARY PREC: NEGATIVE>\n");}
	|ADD							{printf("\t<EXPR UNARY PREC: POSITIVE>\n");}
	;

expr_successor:
	%empty							{printf("\t<EXPR UNARY SUCC: NONE>\n");}
	|EXCLAMATION						{printf("\t<EXPR UNARY SUCC: FACTORIAL>\n");}
	|EXP discrete_term					{printf("\t<EXPR UNARY SUCC: EXPONENTIAL>\n");}
	|EXP PARANOPEN expression PARANCLOSE			{printf("\t<EXPR UNARY SUCC: EXPONENTIAL EXPR>\n");}
	;

discrete_term:
	VARNAME							{printf("\t<DISCRETE TERM: VARNAME>\n");}
	|MELNAME						{printf("\t<DISCRETE TERM: MELNAME>\n");}
	|STRING							{printf("\t<DISCRETE TERM: STRING>\n");}
	|NSTRING						{printf("\t<DISCRETE TERM: NSTRING>\n");}
	|ARGVAR							{printf("\t<DISCRETE TERM: ARGVAR>\n");}
	|functioncall						{printf("\t<DISCRETE TERM: FUNCTIONCALL>\n");}
	|SHELLECHO						{printf("\t<DISCRETE TERM: SHELLECHO>\n");}
	;



/* Boolean conditions set */
conditions:
	conditions_and_only				{printf("\t<CONDITIONS: AND ONLY CONDITION>\n");}
	|conditions LOR conditions_and_only		{printf("\t<CONDITIONS: LOR AND CONDITION>\n");}
	;

conditions_and_only:
	discrete_condition				{printf("\t<AND ONLY CONDITION: DISCRETE CONDITION>\n");}
	|conditions_and_only LAND discrete_condition	{printf("\t<AND ONLY CONDITION: LAND AND CONDITION>\n");}
	;

discrete_condition:
	unary_condition_opr PARANOPEN conditions PARANCLOSE	{printf("\t<DISCRETE CONDITION: UNARY PARAN CONDITION>\n");}
	|conditioncomponent relopr conditioncomponent	{printf("\t<DISCRETE CONDITION: BASIC CONDITION>\n");}
	;

unary_condition_opr:
	%empty						{printf("\t<NO UNARY TO CONDITION>\n");}
	|EXCLAMATION					{printf("\t<NOT FOUND: UNARY TO CONDITION>\n");}
	;

conditioncomponent:
	functioncall					{printf("\t<CONDITIONCOMPONENT: FUNCTIONCALL>\n");}
	|SHELLECHO					{printf("\t<CONDITIONCOMPONENT: SHELLECHO>\n");}
	|MELNAME					{printf("\t<CONDITIONCOMPONENT: MELNAME>\n");}
	|VARNAME					{printf("\t<CONDITIONCOMPONENT: VARNAME>\n");}
	|NSTRING					{printf("\t<CONDITIONCOMPONENT: NSTRING>\n");}
	|STRING						{printf("\t<CONDITIONCOMPONENT: STRING>\n");}
	;

relopr:
	EQ						{printf("\t<RELOPR: EQ>\n");}
	|NQ						{printf("\t<RELOPR: NQ>\n");}
	|LT						{printf("\t<RELOPR: LT>\n");}
	|GT						{printf("\t<RELOPR: GT>\n");}
	|LE						{printf("\t<RELOPR: LE>\n");}
	|GE						{printf("\t<RELOPR: GE>\n");}
	;


/* Functioncall set */
functioncall:
	GSTRING PARANOPEN funccallargs PARANCLOSE	{printf("\t<FUNCTION CALL>\n");}

funccallargs:
	%empty						{printf("\t<FUNCTION CALL ARGUMENTS: NONE>\n");}
	|discrete_argument				{printf("\t<FUNCTION CALL ARGUMENTS: DISCRETE ARGUMENT>\n");}
	|funccallargs COMMA discrete_argument		{printf("\t<FUNCTION CALL ARGUMENTS: COMMA DISCRETE ARGUMENT>\n");}
	;

discrete_argument:
	NSTRING						{printf("\t<FUNCTION CALL ARGUMENTS : NSTRING>\n");}
	|STRING						{printf("\t<FUNCTION CALL ARGUMENTS : STRING>\n");}
	|VARNAME					{printf("\t<FUNCTION CALL ARGUMENTS : VARNAME>\n");}
	|ARGVAR						{printf("\t<FUNCTION CALL ARGUMENTS : ARGVAR>\n");}
	|MELNAME					{printf("\t<FUNCTION CALL ARGUMENTS : MELNAME>\n");}
	|functioncall					{printf("\t<FUNCTION CALL ARGUMENTS : FUNCTIONCALL>\n");}
	;

/* Return statement */
return_statement:
	RETURN BROPEN keyvalpairs BRCLOSE	{printf("\t<RETURN KEYVALPAIRS>\n");}
	|RETURN expression			{printf("\t<RETURN EXPRESSION>\n");}
	;


/* Iterative while */
whileloop:
	WHILE PARANOPEN conditions PARANCLOSE enclosement	{printf("\t<WHILE LOOP>\n");}
	;


/* Iterative for */
forloop:
	FOR PARANOPEN forinit EOS conditions EOS forvarmodif PARANCLOSE enclosement	{printf("\t<FOR LOOP>\n");}
	;

forinit:
	%empty						{printf("\t<FORINIT: EMPTY>\n");}
	|generalvariables_declaration			{printf("\t<FORINIT: GENVARIABLES DECLARATIONS>\n");}
	|mapvariables_declaration			{printf("\t<FORINIT: MAPVARIABLES DECLARATIONS>\n");}
	|assignments					{printf("\t<FORINIT: ASSIGNMENTS>\n");}
	;

forvarmodif:
	%empty						{printf("\t<FORVARMODIF: EMPTY>\n");}
	|assignments					{printf("\t<FORVARMODIF: ASSIGNMENTS>\n");}
	;

/*alltokens: IS|RETURN|BREAK|CONTINUE|FOR|ADD|INCR|SUB|DECR|MUL|EXP|DIV|TRUNCDIV|MAPDECL|VARDECL|VARNAME|MELNAME|ARGVAR|NSTRING|GSTRING|STRING|BROPEN|BRCLOSE|SHELLECHO|FUNC|IF|ELIF|ELSE|WHILE|EOS|PARANOPEN|PARANCLOSE|ASSIGN|FUNCCALL|COMMA|GT|LT|EQ|NQ|GE|LAND|LOR|LE|ERR|EOL;*/
%%

int yyerror(const char *s){
	fprintf(stdout, "{{error: %s}}\n", s);
	return 0;
}
