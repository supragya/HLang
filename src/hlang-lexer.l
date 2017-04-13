/* HLang Lexer Flex 
 * created by Supragya Raj
 */

%option noyywrap
%x _MLCMNT _FUNCTION _STRING _SHELLECHO
%{
#include <string.h>
#include "../headers/buildtime_hlang-parser.h"
#define TESTFOLDER "tests/"
#define VERSION 0.01
#define DEBUG 1
#define DATABUFLEN 2000
char datastring[DATABUFLEN];

void yyclean();
void yyappend(char *);
int has_lval(int);
%}

%%

"declare"[ \t\n]*"map"					{return MAPDECL;}
"declare"						{return VARDECL;}
[$][_a-zA-Z][_a-zA-Z0-9]+				{yyclean(); yyappend(yytext+1); return VARNAME;}
[$][_a-zA-Z][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"		{yyclean(); yyappend(yytext+1); return MELNAME;}
[$][0-9]+						{yyclean(); yyappend(yytext+1); return ARGVAR;}
([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)			{yyclean(); yyappend(yytext); return NVAL;}

["]							{yyclean(); BEGIN _STRING;}
<_STRING>["]						{BEGIN INITIAL; return STRING;}
<_STRING>.						{yyappend(yytext);}

[#][^*].*[^\n]						/*Single line comments, do nothing */
[{]							{return BROPEN;}
[}]							{return BRCLOSE;}

"<%"							{yyclean(); BEGIN _SHELLECHO;}
<_SHELLECHO>">"						{BEGIN INITIAL; return SHELLECHO;}
<_SHELLECHO>.						{yyappend(yytext);}								

"function"						{yyclean(); BEGIN _FUNCTION;}
<_FUNCTION>[ \t\n]+					 /*Eat these up */
<_FUNCTION>[a-zA-Z][a-zA-Z0-9]*				{yyappend(yytext); BEGIN INITIAL;}

"#*"							{BEGIN _MLCMNT;}
<_MLCMNT>([^*]|\n)+|.					/* Do nothing */
<_MLCMNT><<EOF>>					/* Error, but treat as if nothing happened */
<_MLCMNT>"*#"						{BEGIN INITIAL;}

"if"							{return IF;}
"elif"							{return ELIF;}
"else"							{return ELSE;}
"while"							{return WHILE;}

[a-zA-Z][a-zA-Z0-9]*"("					{yyclean(); yyappend(yytext); return FUNC;}
[(]							{return PARANOPEN;}
[)]							{return PARANCLOSE;}	

">"							{return GT;}
"<"							{return LT;}
"=="							{return EQ;}
"!="|"<>"						{return NQ;}
">="							{return GE;}
"<="							{return LE;}

[=]							{return ASSIGN;}
[;]							{return EOS;}
[,]							{return COMMA;}
[ \t\n]							/* Eat up whitespaces */
.							{return ERR; } 

%%

int min(char **argv){
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
	int tok;
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
		while(tok = yylex()){
			DEBUG?printf("%d ", tok):0;
			if(has_lval(tok)){
				DEBUG?printf(" with val "):0;
				DEBUG?printf("%s",datastring):0;
			}
			DEBUG?printf("\n"):0;
			if(tok == FUNC){
				DEBUG?printf("%d\n", PARANOPEN):0;
			}
		}	
		/* Increment the counter */
		i++;
	}
	DEBUG?printf(">>>HLang lexer completes\n"):0;
	return 0;
}

void yyclean(){
	/*Cleans a given string, just adds '\0' in the 0th position */
	datastring[0] = '\0';	
}

void yyappend(char *str){
	int yyaddpoint = 0, strlen = 0;
	while(datastring[yyaddpoint++] != '\0');
	yyaddpoint--;
	while(str[strlen++] != '\0');
	strlen--;
	int i;
	for(i = 0; i<strlen; i++){
		datastring[yyaddpoint+i] = str[i];
	}
	datastring[yyaddpoint+strlen] = '\0';
}

int has_lval(int tokentype){
	int lvaltypes[] = {VARNAME, MELNAME, ARGVAR, NVAL, STRING, SHELLECHO, FUNC, FUNCCALL, -1};
	int i;
	for(i = 0;lvaltypes[i] != -1;i++)
		if(tokentype == lvaltypes[i]) 
			return 1;
	return 0;
}

