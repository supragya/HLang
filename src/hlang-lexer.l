/* HLANG INTERPRETER development stage 0 
 * created by SUPRAGYA RAJ
 * basic lexing done here
 */


%option noyywrap
%x MLCMNT FUNCTION
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
[$][0-9]+						{printf("<ARGVAR>{%s} ", yytext+1);}
([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)	{printf("<NVAL>{%s} ", yytext); }
["].*["]							{printf("<STR>{%s} ", yytext);}
[#][^*].*[^\n]					{printf("<SLCMNT>{%s}",yytext+1);}
[{]								{printf("<BROPEN> ");}
[}]								{printf("<BRCLOSE> ");}

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

[a-zA-Z][a-zA-Z0-9]*"("			{printf("<FUNCCALL>{%s} ",yytext); funccallopen = 1;}
[)]								{funccallopen?printf("<FUNCCALLEND>{0} "):0; funccallopen = 0;}	

[=]								{printf("<ASSIGN>{0} "); }
[;]								{printf("<EOS>{0} "); }
[,]								{printf("<COMMA>{0} ");}
[ \t]							/* Eat up whitespaces */
.								{printf("**E:%s** ", yytext); } 

%%

int main(char **argv){
	/* Debug welcome */
	DEBUG?printf("+--------------------------\n"):0;
	DEBUG?printf(" HLANG LEXER %0.2f\n", VERSION):0;
	DEBUG?printf("+--------------------------\n"):0;
	
	/*Define testfiles */
	char tests[][50]	=	{	"variable_declarations.hl",
								"comments.hl",
								"functions.hl",
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
