/* HLang Lexer Flex 
 * created by Supragya Raj
 */

%option noyywrap
%x _MLCMNT _FUNCTION _STRING _SHELLECHO
%{
#include <string.h>
#include "buildtime_hlang-parser.h"
#include "hlang-lexer.h"

char *yylval;

void yyclean();
void yyappend(char *);
int has_lval(int);
%}

%%

"declare"[ \t\n]*"map"					{printf("[mapdecl]\n");return MAPDECL;}
"declare"						{printf("[vardecl]\n");return VARDECL;}
[$][_a-zA-Z][_a-zA-Z0-9]+				{yyclean(); yyappend(yytext+1); return VARNAME;}
[$][_a-zA-Z][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"		{yyclean(); yyappend(yytext+1); return MELNAME;}
[$][0-9]+						{yyclean(); yyappend(yytext+1); return ARGVAR;}
([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)			{yyclean(); yyappend(yytext); return STRING;}

["]							{yyclean(); BEGIN _STRING;}
<_STRING>["]						{BEGIN INITIAL; return STRING;}
<_STRING>.						{yyappend(yytext);}

[#][^*].*[^\n]						/*Single line comments, do nothing */
"("							{printf("[paranopen]\n");return PARANOPEN;}
")"							{printf("[paranclose]\n");return PARANCLOSE;}	
[{]							{printf("[BROPEN]\n");return BROPEN;}
[}]							{printf("[BRCLOSE]\n");return BRCLOSE;}

"<%"							{yyclean(); BEGIN _SHELLECHO;}
<_SHELLECHO>">"						{BEGIN INITIAL; return SHELLECHO;}
<_SHELLECHO>.						{yyappend(yytext);}								

"function"						{yyclean(); BEGIN _FUNCTION;}
<_FUNCTION>[ \t\n]+					 /*Eat these up */
<_FUNCTION>[a-zA-Z][a-zA-Z0-9]*				{yyappend(yytext); BEGIN INITIAL; return FUNC;}

"#*"							{BEGIN _MLCMNT;}
<_MLCMNT>([^*]|\n)+|.					/* Do nothing */
<_MLCMNT><<EOF>>					/* Error, but treat as if nothing happened */
<_MLCMNT>"*#"						{BEGIN INITIAL;}

"&&"							{return LAND;}
"||"							{return LOR;}
"if"							{printf("[if]\n");return IF;}
"elif"							{printf("[elif]\n");return ELIF;}
"else"							{printf("[else]\n");return ELSE;}
"while"							{return WHILE;}

[_a-zA-Z][a-zA-Z0-9]*[ \t\n]*"("			{yyclean(); yyappend(yytext); return FUNCCALL;}

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

int set_read_file(char *filename){
	if(!fopen(filename, "r"))
		printf("cannot find file");
	if(!(yyin = fopen(filename, "r")))
		return 1;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	return 0;
}

void yyclean(){
	/*Cleans a given string, just adds '\0' in the 0th position */
	yylval[0] = '\0';	
}

void yyappend(char *str){
	int addpoint = 0, strlen = 0;
	while(yylval[addpoint++] != '\0');
	addpoint--;
	while(str[strlen++] != '\0');
	strlen--;
	int i;
	for(i = 0; i<strlen; i++){
		yylval[addpoint+i] = str[i];
	}
	yylval[addpoint+strlen] = '\0';
}

int has_lval(int tokentype){
	int lvaltypes[] = {VARNAME, MELNAME, ARGVAR, NVAL, STRING, SHELLECHO, FUNC, FUNCCALL, -1};
	int i;
	for(i = 0;lvaltypes[i] != -1;i++)
		if(tokentype == lvaltypes[i]) 
			return 1;
		return 0;
}

