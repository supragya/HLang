/* HLANG INTERPRETER development stage 0 
 * created by SUPRAGYA RAJ
 */

/* DEFINING TYPES:

	MAPDECL:	Declare map variable
	VARDECL:	Declare general variable
	VARNAME:	Variable name
	MELNAME:	Map element name
	ARGVAR :	Argument variable
	NVAL   :	Numeric type
	STR    :	String
	SLCMNT :	Single Line comments
	MLCMNT :	Multi Line comments
	BROPEN :	Brace open
	BRCLOSE:	Brace close
	SHELLECHO:	Shell echo to the shell terminal
	FUNC   :	Function
	IF     :	if selection construct
	ELIF   :	else if selection construct
	ELSE   :	else selection construct
	WHILE  :	while iterative construct
	EOS    :	End of statement
	PARANOPEN:	Paranthesis open
	PARANCLOSE:	Paranthesis close
	ASSIGN :	Assignment operator
	FUNCCALL:	Function call
	COMMA  :	comma
	GT     :	Relational operator greater than
	LT     :	Relational operator lesser than
	EQ     :	Relational operator equal to
	NQ     :	Relational operator not equal to
	GE     :	Relational operator greater than or equal to
	LE     :	Relational operator lesser than or equal to

	Anything that does not matches the above is shown as **E{string}** during tests
*/


%option noyywrap
%x MLCMNT FUNCTION STR SHELLECHO
%{
#include <string.h>
#define TESTFOLDER "../tests/"
#define VERSION 0.01
#define DEBUG 1
int funccallopen = 0;
%}

%%

"declare"[ \t\n]*"map"			{printf("<MAPDECL>{0} ");}
"declare"						{printf("<VARDECL>{0} ");}
[$][_a-zA-Z][_a-zA-Z0-9]+		{printf("<VARNAME>{%s} ", yytext+1); }
[$][_a-zA-Z][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"	{printf("<MELNAME>{%s} ", yytext+1); }
[$][0-9]+						{printf("<ARGVAR>{%s} ", yytext+1);}
([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)	{printf("<NVAL>{%s} ", yytext); }
["]								{printf("<STR>{"); BEGIN STR;}
<STR>["]						{printf("} "); BEGIN INITIAL;}
<STR>.							{printf(yytext);}/*eat up*/
[#][^*].*[^\n]					{printf("<SLCMNT>{%s}",yytext+1);}
[{]								{printf("<BROPEN> ");}
[}]								{printf("<BRCLOSE> ");}

"<%"							{printf("<SHELLECHO>{"); BEGIN SHELLECHO;}
<SHELLECHO>">"					{printf("} "); BEGIN INITIAL;}
<SHELLECHO>.					{printf(yytext);}								

"function"						{printf("<FUNC>");
								 BEGIN FUNCTION;}
<FUNCTION>[ \t\n]+				 /*Eat these up */
<FUNCTION>[a-zA-Z][a-zA-Z0-9]*	{printf("{%s} ", yytext);
								 BEGIN INITIAL;}

"#*"							{printf("<MLCMNT>{");
								 BEGIN MLCMNT;}
<MLCMNT>([^*]|\n)+|.			{printf(yytext);}
<MLCMNT><<EOF>>					{printf("[Unterminated MLCMNT]"); return 0;}
<MLCMNT>"*#"					{printf("} ");
								 BEGIN(INITIAL);}

"if"							{printf("<IF>{0} ");}
"elif"							{printf("<ELIF>{0} ");}
"else"							{printf("<ELSE>{0} ");}
"while"							{printf("<WHILE>{0} ");}

[a-zA-Z][a-zA-Z0-9]*"("			{printf("<FUNCCALL>{%s} <PARANOPEN>{0} ",yytext);}
[(]								{printf("<PARANOPEN>{0} ");}
[)]								{printf("<PARANCLOSE>{0} ");}	

">"								{printf("<GT>{0} ");}
"<"								{printf("<LT>{0} ");}
"=="							{printf("<EQ>{0} ");}
"!="|"<>"						{printf("<NQ>{0} ");}
">="							{printf("<GE>{0} ");}
"<="							{printf("<LE>{0} ");}

[=]								{printf("<ASSIGN>{0} "); }
[;]								{printf("<EOS>{0} "); }
[,]								{printf("<COMMA>{0} ");}
[ \t]							/* Eat up whitespaces */
								/* [\n] */
.								{printf("**E:%s** ", yytext); } 

%%

int main(char **argv){
	/* Debug welcome */
	DEBUG?printf("+--------------------------\n"):0;
	DEBUG?printf("|HLANG LEXER %0.2f\n", VERSION):0;
	DEBUG?printf("+--------------------------\n"):0;
	
	/*Define testfiles */
	char tests[][50]	=	{	"variable_declarations.hl",
								"comments.hl",
								"functions.hl",
								"selections.hl",
								"elastic_horse_regressions.hl",
								""};
	unsigned int i = 0;

	/* Test file parsing */
	while(strcmp(tests[i], "")){

		/* Absolutize file name */
		char workfile[200] = TESTFOLDER;
		strcat(workfile, tests[i]);

		/* Opening the test file for parsing */
		if(!(yyin = fopen(workfile, "r"))){
			DEBUG?printf(">>>Error opening %s, aborting\n", workfile):0;
			perror(argv[1]);
			return 1;
		}
		DEBUG?printf(">>>%s opened successfully\n", workfile):0;

		/* Start lexical analysis on the file */
		DEBUG?printf("\n"):0;
		yylex();
		DEBUG?printf("\n\n"):0;

		/* Increment the counter */
		i++;
	}

	DEBUG?printf(">>>Hlang lexer completes\n"):0;
	return 0;
}
