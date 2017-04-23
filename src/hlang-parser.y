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
#include "verbose.h"

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
	FUNC enclosement				{if(PARSERVERBOSE())printf("\t<FUNCTION>\n"); ast_add_function($1);}
	;

enclosement:
	BROPEN code BRCLOSE				{if(PARSERVERBOSE())printf("\t<ENCLOSEMENT>\n");}
	;

code:
	%empty
	|code sequential_constuct			{if(PARSERVERBOSE())printf("\t<CODE: SEQUENTIAL CONSTRUCT>\n"); ast_add_seq("seq");}
	|code selective_constructs			{if(PARSERVERBOSE())printf("\t<CODE: SELECTIVE CONSTRUCTS>\n"); ast_add_sel("sel");}
	|code iterative_constructs			{if(PARSERVERBOSE())printf("\t<CODE: ITERATIVE CONSTRUCTIS>\n"); ast_add_iter("iter");	}
	;




/* Different constructs */

sequential_constuct:
	mapvariables_declaration EOS			{if(PARSERVERBOSE())printf("\t<SEQUENTIAL CONSTRUCT: MAP VARIABLE DECLARATIONS>\n"); ast_add_seq_mapdecl();}
	|generalvariables_declaration EOS		{if(PARSERVERBOSE())printf("\t<SEQUENTIAL CONSTRUCT: GEN VARIABLE DECLARATIONS>\n"); ast_add_seq_vardecl();}
	|SHELLECHO EOS					{if(PARSERVERBOSE())printf("\t<SEQUENTIAL CONSTRUCT: SHELL ECHO>\n"); ast_add_seq_shellecho($1);}
	|functioncall EOS				{if(PARSERVERBOSE())printf("\t<SEQUENTIAL CONSTRUCT: FUNCTIONCALL>\n"); ast_add_seq_functioncall($1);}
	|assignments EOS				{if(PARSERVERBOSE())printf("\t<SEQUENTIAL CONSTRUCT: VARIABLE ASSIGNMENT>\n");}
	|return_statement EOS				{if(PARSERVERBOSE())printf("\t<SEQUENTIAL CONSTRUCT: RETURN STATEMENT>\n"); ast_add_seq_return();}
	;

selective_constructs:
	ifblock elseifblocks elseblock			{if(PARSERVERBOSE())printf("\t<SELECTIVE: IF ELSEIF ELSE BLOCKS>\n");}
	|ifblock elseifblocks				{if(PARSERVERBOSE())printf("\t<SELECTIVE: IF ELSEIF BLOCKS>\n");}
	|ifblock elseblock				{if(PARSERVERBOSE())printf("\t<SELECTIVE: IF ELSE BLOCKS>\n");}
	|ifblock					{if(PARSERVERBOSE())printf("\t<SELECTIVE: IF ONLY BLOCK>\n");}
	;

ifblock:
	IF PARANOPEN conditions PARANCLOSE enclosement	{if(PARSERVERBOSE())printf("\t<IF BLOCK>\n");}

elseblock:
	ELSE enclosement				{if(PARSERVERBOSE())printf("\t<ELSE BLOCK>\n");}

elseifblocks:
	ELIF PARANOPEN conditions PARANCLOSE enclosement		{if(PARSERVERBOSE())printf("\t<ELSEIF BLOCK: ONE>\n");}
	|elseifblocks ELIF PARANOPEN conditions PARANCLOSE enclosement	{if(PARSERVERBOSE())printf("\t<ELSEIF BLOCK: RECURSIVE>\n");}
	;

iterative_constructs:
	whileloop					{if(PARSERVERBOSE())printf("\t<ITERATIVE CONSTRUCTS: WHILE LOOP>\n");}
	|forloop					{if(PARSERVERBOSE())printf("\t<ITERATIVE CONSTRUCTS: FOR LOOP>\n");}
	;



/* Map variables declaration code syntax */
mapvariables_declaration:
	MAPDECL map_variablelist			{if(PARSERVERBOSE())printf("\t<MAPVARIABLE DECLARATIONS: MAP VAR LIST>\n");}
	;

map_variablelist:
	map_discrete_variable				{if(PARSERVERBOSE())printf("\t<MAPVARIABLELIST: DISCRETE VARIABLE FOUND>\n");}
	|map_variablelist COMMA map_discrete_variable	{if(PARSERVERBOSE())printf("\t<MAPVARIABLELIST: COMMA DISCRETE>\n");}
	;

map_discrete_variable:
	VARNAME						{if(PARSERVERBOSE())printf("\t<MAP DISCRETE VARIABLE: VARNAME FOUND| %s>\n", $1); ast_add_mapdeclnode($1);}
	|VARNAME ASSIGN BROPEN keyvalpairs BRCLOSE	{if(PARSERVERBOSE())printf("\t<MAP DISCRETE VARIABLE: KEYVALPAIRS| %s>\n", $1); ast_add_mapdeclnode($1);}
	;

keyvalpairs:
	keytype IS datatype				{if(PARSERVERBOSE())printf("\t<KEYVALPAIRS: FOUND DISCRETE>\n"); ast_make_keyvalpair($1,$3);}
	|keyvalpairs COMMA keytype IS datatype		{if(PARSERVERBOSE())printf("\t<KEYVALPAIRS: FOUND COMMA>\n"); ast_make_keyvalpair($3,$5);}
	|datatype					{if(PARSERVERBOSE())printf("\t<KEYVALPAIRS: FOUND JUST DATATYPE>\n"); ast_make_keyvalpair($1, "0");}
	|keyvalpairs COMMA datatype			{if(PARSERVERBOSE())printf("\t<KEYVALPAIRS: FOUND COMMA JUST DATATYPE>\n"); ast_make_keyvalpair($3, "0");}
	;

keytype:
	STRING						{if(PARSERVERBOSE())printf("\t<KEYTYPE: STRING>\n");}
	|NSTRING					{if(PARSERVERBOSE())printf("\t<KEYTYPE: NSTRING>\n");}
	;

datatype:
	STRING						{if(PARSERVERBOSE())printf("\t<DATATYPE: STRING>\n");}
	|NSTRING					{if(PARSERVERBOSE())printf("\t<DATATYPE: NSTRING>\n");}
	;



/* General variables declaration code syntax */
generalvariables_declaration:
	VARDECL gen_variablelist			{if(PARSERVERBOSE())printf("\t<GENVARIABLE DECLARATIONS: GEN VAR LIST>\n");}
	;

gen_variablelist:
	gen_discrete_variable				{if(PARSERVERBOSE())printf("\t<GENVARIABLE DECLARATIONS: DISCRETE VARIABLE FOUND>\n");}
	|gen_variablelist COMMA gen_discrete_variable	{if(PARSERVERBOSE())printf("\t<GENVARIABLE DECLARATIONS: COMMA DISCRETE>\n");}
	;

gen_discrete_variable:
	VARNAME						{if(PARSERVERBOSE())printf("\t<GEN DISCRETE VARIABLE: VARNAME| New variable %s>\n", $1); ast_make_vardecl_assignment($1, "1");}
	|VARNAME ASSIGN expression			{if(PARSERVERBOSE())printf("\t<GEN DISCRETE VARIABLE: VARNAME ASSIGN EXPRESSION| New variable %s with value %s>\n", $1, $3);ast_make_vardecl_assignment($1, "2");}
	|MELNAME					{if(PARSERVERBOSE())printf("\t<GEN DISCRETE VARIABLE: MELNAME| New mapelement %s>\n", $1); ast_make_vardecl_assignment($1, "3");}
	|MELNAME ASSIGN expression			{if(PARSERVERBOSE())printf("\t<GEN DISCRETE VARIABLE: MELNAME ASSIGN EXPRESSION| New element %s with val %s>\n",$1, $3); ast_make_vardecl_assignment($1, "4");}
	;



/* Variable assignment sequential constructs */
assignments:
	VARNAME ASSIGN expression			{if(PARSERVERBOSE())printf("\t<ASSIGNMENT: VARNAME ASSIGN EXPRESSION>\n");}
	|MELNAME ASSIGN expression			{if(PARSERVERBOSE())printf("\t<ASSIGNMENT: MELNAME ASSIGN EXPRESSION>\n");}
	|VARNAME ASSIGN BROPEN keyvalpairs BRCLOSE	{if(PARSERVERBOSE())printf("\t<ASSIGNMENT: VARNAME ASSIGN KEYVALPAIRS>\n");}
	|unaryop assignmentvar				{if(PARSERVERBOSE())printf("\t<ASSIGNMENT: PRE OPERATION>\n");}
	|assignmentvar unaryop				{if(PARSERVERBOSE())printf("\t<ASSIGNMENT: POST OPERATION>\n");}
	;

assignmentvar:
	VARNAME						{if(PARSERVERBOSE())printf("\t<ASSIGNMENTVAR: VARNAME>\n");}
	|MELNAME					{if(PARSERVERBOSE())printf("\t<ASSIGNMENTVAR: MELNAME>\n");}
	;

unaryop:
	DECR						{if(PARSERVERBOSE())printf("\t<ASSIGNMENTVAR: DECREMENT>\n");}
	|INCR						{if(PARSERVERBOSE())printf("\t<ASSIGNMENTVAR: INCREMENT>\n");}
	;



/* Arithmetic and otherwise */

expression:
	expression2						{if(PARSERVERBOSE())printf("\t<EXPRESSION: EXPRESSION2>\n");}
	|expression ADD expression2				{if(PARSERVERBOSE())printf("\t<EXPRESSION: ADD EXPRESSION2>\n");}
	|expression SUB expression2				{if(PARSERVERBOSE())printf("\t<EXPRESSION: SUB EXPRESSION2>\n");}
	;

expression2:
	expression3						{if(PARSERVERBOSE())printf("\t<EXPRESSION2: EXPRESSION3>\n");}
	|expression2 MUL expression3				{if(PARSERVERBOSE())printf("\t<EXPRESSION2: MUL EXPRESSION3>\n");}
	|expression2 DIV expression3				{if(PARSERVERBOSE())printf("\t<EXPRESSION2: DIV EXPRESSION3>\n");}
	|expression2 TRUNCDIV expression3			{if(PARSERVERBOSE())printf("\t<EXPRESSION2: TRUNCDIV EXPRESSION3>\n");}
	;

expression3:
	expr_unary_preceder PARANOPEN expression PARANCLOSE expr_successor	{$$ = $3; if(PARSERVERBOSE())printf("\t<EXPRESSION3: UNARYPREC EXPRESSION EXPRSUCC>\n");}
	|expr_unary_preceder discrete_term expr_successor	{$$ = $2; if(PARSERVERBOSE())printf("\t<EXPRESSION3: UNARYPREC TERM EXPRSUCC>\n");}
	;

expr_unary_preceder:
	%empty							{if(PARSERVERBOSE())printf("\t<EXPR UNARY PREC: NONE>\n");}
	|SUB							{if(PARSERVERBOSE())printf("\t<EXPR UNARY PREC: NEGATIVE>\n");}
	|ADD							{if(PARSERVERBOSE())printf("\t<EXPR UNARY PREC: POSITIVE>\n");}
	;

expr_successor:
	%empty							{if(PARSERVERBOSE())printf("\t<EXPR UNARY SUCC: NONE>\n");}
	|EXCLAMATION						{if(PARSERVERBOSE())printf("\t<EXPR UNARY SUCC: FACTORIAL>\n");}
	|EXP discrete_term					{if(PARSERVERBOSE())printf("\t<EXPR UNARY SUCC: EXPONENTIAL>\n");}
	|EXP PARANOPEN expression PARANCLOSE			{if(PARSERVERBOSE())printf("\t<EXPR UNARY SUCC: EXPONENTIAL EXPR>\n");}
	;

discrete_term:
	VARNAME							{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: VARNAME>\n"); ast_add_expr3_discrete_term_variable($1);}
	|MELNAME						{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: MELNAME>\n"); ast_add_expr3_discrete_term_variable($1);}
	|STRING							{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: STRING>\n"); ast_add_expr3_discrete_term_string($1);}
	|NSTRING						{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: NSTRING>\n"); ast_add_expr3_discrete_term_string($1);}
	|ARGVAR							{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: ARGVAR>\n"); ast_add_expr3_discrete_term_variable($1);}
	|functioncall						{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: FUNCTIONCALL>\n");}
	|SHELLECHO						{if(PARSERVERBOSE())printf("\t<DISCRETE TERM: SHELLECHO>\n");}
	;



/* Boolean conditions set */
conditions:
	conditions_and_only				{if(PARSERVERBOSE())printf("\t<CONDITIONS: AND ONLY CONDITION>\n");}
	|conditions LOR conditions_and_only		{if(PARSERVERBOSE())printf("\t<CONDITIONS: LOR AND CONDITION>\n");}
	;

conditions_and_only:
	discrete_condition				{if(PARSERVERBOSE())printf("\t<AND ONLY CONDITION: DISCRETE CONDITION>\n");}
	|conditions_and_only LAND discrete_condition	{if(PARSERVERBOSE())printf("\t<AND ONLY CONDITION: LAND AND CONDITION>\n");}
	;

discrete_condition:
	unary_condition_opr PARANOPEN conditions PARANCLOSE	{if(PARSERVERBOSE())printf("\t<DISCRETE CONDITION: UNARY PARAN CONDITION>\n");}
	|conditioncomponent relopr conditioncomponent	{if(PARSERVERBOSE())printf("\t<DISCRETE CONDITION: BASIC CONDITION>\n");}
	;

unary_condition_opr:
	%empty						{if(PARSERVERBOSE())printf("\t<NO UNARY TO CONDITION>\n");}
	|EXCLAMATION					{if(PARSERVERBOSE())printf("\t<NOT FOUND: UNARY TO CONDITION>\n");}
	;

conditioncomponent:
	functioncall					{if(PARSERVERBOSE())printf("\t<CONDITIONCOMPONENT: FUNCTIONCALL>\n");}
	|SHELLECHO					{if(PARSERVERBOSE())printf("\t<CONDITIONCOMPONENT: SHELLECHO>\n");}
	|MELNAME					{if(PARSERVERBOSE())printf("\t<CONDITIONCOMPONENT: MELNAME>\n");}
	|VARNAME					{if(PARSERVERBOSE())printf("\t<CONDITIONCOMPONENT: VARNAME>\n");}
	|NSTRING					{if(PARSERVERBOSE())printf("\t<CONDITIONCOMPONENT: NSTRING>\n");}
	|STRING						{if(PARSERVERBOSE())printf("\t<CONDITIONCOMPONENT: STRING>\n");}
	;

relopr:
	EQ						{if(PARSERVERBOSE())printf("\t<RELOPR: EQ>\n");}
	|NQ						{if(PARSERVERBOSE())printf("\t<RELOPR: NQ>\n");}
	|LT						{if(PARSERVERBOSE())printf("\t<RELOPR: LT>\n");}
	|GT						{if(PARSERVERBOSE())printf("\t<RELOPR: GT>\n");}
	|LE						{if(PARSERVERBOSE())printf("\t<RELOPR: LE>\n");}
	|GE						{if(PARSERVERBOSE())printf("\t<RELOPR: GE>\n");}
	;


/* Functioncall set */
functioncall:
	GSTRING PARANOPEN funccallargs PARANCLOSE	{if(PARSERVERBOSE())printf("\t<FUNCTION CALL>\n"); $$=$1;}

funccallargs:
	%empty						{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS: NONE>\n");}
	|discrete_argument				{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS: DISCRETE ARGUMENT>\n");}
	|funccallargs COMMA discrete_argument		{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS: COMMA DISCRETE ARGUMENT>\n");}
	;

discrete_argument:
	NSTRING						{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS : NSTRING>\n"); ast_add_arguments_string($1);}
	|STRING						{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS : STRING>\n"); ast_add_arguments_string($1);}
	|VARNAME					{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS : VARNAME>\n"); ast_add_arguments_varname($1);}
	|ARGVAR						{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS : ARGVAR>\n"); ast_add_arguments_varname($1);}
	|MELNAME					{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS : MELNAME>\n"); ast_add_arguments_varname($1);}
	|functioncall					{if(PARSERVERBOSE())printf("\t<FUNCTION CALL ARGUMENTS : FUNCTIONCALL>\n");}
	;

/* Return statement */
return_statement:
	RETURN BROPEN keyvalpairs BRCLOSE	{if(PARSERVERBOSE())printf("\t<RETURN KEYVALPAIRS>\n");}
	|RETURN expression			{if(PARSERVERBOSE())printf("\t<RETURN EXPRESSION>\n"); ast_set_return_val_varname("dota");}
	;


/* Iterative while */
whileloop:
	WHILE PARANOPEN conditions PARANCLOSE enclosement	{if(PARSERVERBOSE())printf("\t<WHILE LOOP>\n");}
	;


/* Iterative for */
forloop:
	FOR PARANOPEN forinit EOS conditions EOS forvarmodif PARANCLOSE enclosement	{if(PARSERVERBOSE())printf("\t<FOR LOOP>\n");}
	;

forinit:
	%empty						{if(PARSERVERBOSE())printf("\t<FORINIT: EMPTY>\n");}
	|generalvariables_declaration			{if(PARSERVERBOSE())printf("\t<FORINIT: GENVARIABLES DECLARATIONS>\n");}
	|mapvariables_declaration			{if(PARSERVERBOSE())printf("\t<FORINIT: MAPVARIABLES DECLARATIONS>\n");}
	|assignments					{if(PARSERVERBOSE())printf("\t<FORINIT: ASSIGNMENTS>\n");}
	;

forvarmodif:
	%empty						{if(PARSERVERBOSE())printf("\t<FORVARMODIF: EMPTY>\n");}
	|assignments					{if(PARSERVERBOSE())printf("\t<FORVARMODIF: ASSIGNMENTS>\n");}
	;

/*alltokens: IS|RETURN|BREAK|CONTINUE|FOR|ADD|INCR|SUB|DECR|MUL|EXP|DIV|TRUNCDIV|MAPDECL|VARDECL|VARNAME|MELNAME|ARGVAR|NSTRING|GSTRING|STRING|BROPEN|BRCLOSE|SHELLECHO|FUNC|IF|ELIF|ELSE|WHILE|EOS|PARANOPEN|PARANCLOSE|ASSIGN|FUNCCALL|COMMA|GT|LT|EQ|NQ|GE|LAND|LOR|LE|ERR|EOL;*/
%%

int yyerror(const char *s){
	fprintf(stdout, "{{error: %s}}\n", s);
	return 0;
}
