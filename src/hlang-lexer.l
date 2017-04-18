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
"return"						{printf("[return]\n");return RETURN;}
"break"							{printf("[break]\n");return BREAK;}
"continue"						{printf("[continue]\n");return CONTINUE;}
"for"							{printf("[for]\n");return FOR;}
"if"							{printf("[if]\n");return IF;}
"elif"							{printf("[elif]\n");return ELIF;}
"else"							{printf("[else]\n");return ELSE;}
"while"							{printf("[while]\n");return WHILE;}
"is"							{printf("[is]\n"); return IS;}


"&&"							{printf("[land]\n");return LAND;}
"||"							{printf("[lor]\n");return LOR;}

">"							{printf("[gt]\n");return GT;}
"<"							{printf("[lt]\n");return LT;}
"=="							{printf("[eq]\n");return EQ;}
"!="|"<>"						{printf("[nq]\n");return NQ;}
">="							{printf("[ge]\n");return GE;}
"<="							{printf("[le]\n");return LE;}

[+]							{printf("[add]\n"); return ADD;}
[+]{2}							{printf("[increment]\n"); return INCR;}
[-]							{printf("[sub]\n"); return SUB;}
[-]{2}							{printf("[decrement]\n"); return DECR;}
[*]							{printf("[multiply]\n"); return MUL;}
[*]{2}							{printf("[exponential]\n"); return EXP;}
[/]							{printf("[divide]\n"); return DIV;}
[/]{2}							{printf("[truncdiv]\n"); return TRUNCDIV;}

[!]							{printf("[exclamation]\n");return EXCLAMATION;}
[=]							{printf("[assign]\n");return ASSIGN;}
[;]							{printf("[eos]\n"); return EOS;}
[,]							{printf("[comma]\n");return COMMA;}

[(]							{printf("[paranopen]\n");return PARANOPEN;}
[)]							{printf("[paranclose]\n");return PARANCLOSE;}
[{]							{printf("[bropen]\n");return BROPEN;}
[}]							{printf("[brclose]\n");return BRCLOSE;}

[$][0-9]+						{yyclean(); yyappend(yytext+1); printf("[argvar|%s]\n",yylval);return ARGVAR;}
[$][_a-zA-Z0-9]+					{yyclean(); yyappend(yytext+1); printf("[varname|%s]\n",yylval); return VARNAME;}
[$][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"			{yyclean(); yyappend(yytext+1); printf("[melname|%s]\n",yylval); return MELNAME;}


[#][^*].*[^\n]						/*Single line comments, do nothing */

"<%"							{yyclean(); BEGIN _SHELLECHO;}
<_SHELLECHO>">"						{BEGIN INITIAL; printf("[shellecho|%s]\n",yylval);return SHELLECHO;}
<_SHELLECHO>.						{yyappend(yytext);}

"function"						{yyclean(); BEGIN _FUNCTION;}
<_FUNCTION>[ \t\n]+					 /*Eat these up */
<_FUNCTION>[_a-zA-Z][a-zA-Z0-9]*			{yyappend(yytext); BEGIN INITIAL; printf("[func|%s]\n",yylval); return FUNC;}

"#*"							{BEGIN _MLCMNT;}
<_MLCMNT>([^*]|\n)+|.					/* Do nothing */
<_MLCMNT><<EOF>>					/* Error, but treat as if nothing happened */
<_MLCMNT>"*#"						{BEGIN INITIAL;}

([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)			{yyclean(); yyappend(yytext); printf("[string|%s]\n",yylval);return NSTRING;}

[_a-zA-Z0-9]+						{yyclean(); yyappend(yytext); printf("[genstring]%s\n",yylval); return GSTRING;}

["]							{yyclean(); BEGIN _STRING;}
<_STRING>["]						{BEGIN INITIAL; printf("[string|%s]\n",yylval); return STRING;}
<_STRING>.						{yyappend(yytext);}

[ \t\n]							/* Eat up whitespaces */
.							{printf("[error-lexer|%s]\n", yytext);return ERR; }

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
	int lvaltypes[] = {VARNAME, MELNAME, ARGVAR, NSTRING, STRING, GSTRING, SHELLECHO, FUNC, FUNCCALL, -1};
	int i;
	for(i = 0;lvaltypes[i] != -1;i++)
		if(tokentype == lvaltypes[i])
			return 1;
		return 0;
}
