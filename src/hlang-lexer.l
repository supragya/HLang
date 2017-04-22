/* HLang Lexer Flex
 * created by Supragya Raj
 */

%option noyywrap
%x _MLCMNT _FUNCTION _STRING _SHELLECHO
%{
#include <string.h>
#include "buildtime_hlang-parser.h"
#include "hlang-lexer.h"
#include "verbose.h"

void yyclean();
void yyappend(char *);
int has_lval(int);
%}

%%

"declare"						{if(LEXERVERBOSE())printf("[vardecl]\n");return VARDECL;}
"return"						{if(LEXERVERBOSE())printf("[return]\n");return RETURN;}
"declare"[ \t\n]*"map"					{if(LEXERVERBOSE())printf("[mapdecl]\n");return MAPDECL;}
"break"							{if(LEXERVERBOSE())printf("[break]\n");return BREAK;}
"continue"						{if(LEXERVERBOSE())printf("[continue]\n");return CONTINUE;}
"for"							{if(LEXERVERBOSE())printf("[for]\n");return FOR;}
"if"							{if(LEXERVERBOSE())printf("[if]\n");return IF;}
"elif"							{if(LEXERVERBOSE())printf("[elif]\n");return ELIF;}
"while"							{if(LEXERVERBOSE())printf("[while]\n");return WHILE;}
"else"							{if(LEXERVERBOSE())printf("[else]\n");return ELSE;}
"is"							{if(LEXERVERBOSE())printf("[is]\n"); return IS;}


"&&"							{if(LEXERVERBOSE())printf("[land]\n");return LAND;}
"||"							{if(LEXERVERBOSE())printf("[lor]\n");return LOR;}

">"							{if(LEXERVERBOSE())printf("[gt]\n");return GT;}
"<"							{if(LEXERVERBOSE())printf("[lt]\n");return LT;}
"=="							{if(LEXERVERBOSE())printf("[eq]\n");return EQ;}
"!="|"<>"						{if(LEXERVERBOSE())printf("[nq]\n");return NQ;}
">="							{if(LEXERVERBOSE())printf("[ge]\n");return GE;}
"<="							{if(LEXERVERBOSE())printf("[le]\n");return LE;}

[+]							{if(LEXERVERBOSE())printf("[add]\n"); return ADD;}
[+]{2}							{if(LEXERVERBOSE())printf("[increment]\n"); return INCR;}
[-]							{if(LEXERVERBOSE())printf("[sub]\n"); return SUB;}
[-]{2}							{if(LEXERVERBOSE())printf("[decrement]\n"); return DECR;}
[*]							{if(LEXERVERBOSE())printf("[multiply]\n"); return MUL;}
[*]{2}							{if(LEXERVERBOSE())printf("[exponential]\n"); return EXP;}
[/]							{if(LEXERVERBOSE())printf("[divide]\n"); return DIV;}
[/]{2}							{if(LEXERVERBOSE())printf("[truncdiv]\n"); return TRUNCDIV;}

[!]							{if(LEXERVERBOSE())printf("[exclamation]\n");return EXCLAMATION;}
[=]							{if(LEXERVERBOSE())printf("[assign]\n");return ASSIGN;}
[;]							{if(LEXERVERBOSE())printf("[eos]\n"); return EOS;}
[,]							{if(LEXERVERBOSE())printf("[comma]\n");return COMMA;}

[(]							{if(LEXERVERBOSE())printf("[paranopen]\n");return PARANOPEN;}
[)]							{if(LEXERVERBOSE())printf("[paranclose]\n");return PARANCLOSE;}
[{]							{if(LEXERVERBOSE())printf("[bropen]\n");return BROPEN;}
[}]							{if(LEXERVERBOSE())printf("[brclose]\n");return BRCLOSE;}

[$][0-9]+						{yyclean(); yyappend(yytext+1); if(LEXERVERBOSE())printf("[argvar|%s]\n",yylval);return ARGVAR;}
[$][_a-zA-Z0-9]+					{yyclean(); yyappend(yytext+1); if(LEXERVERBOSE())printf("[varname|%s]\n",yylval); return VARNAME;}
[$][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"			{yyclean(); yyappend(yytext+1); if(LEXERVERBOSE())printf("[melname|%s]\n",yylval); return MELNAME;}


[#][^*].*[^\n]						/*Single line comments, do nothing */

"<%"							{yyclean(); BEGIN _SHELLECHO;}
<_SHELLECHO>">"						{BEGIN INITIAL; if(LEXERVERBOSE())printf("[shellecho|%s]\n",yylval);return SHELLECHO;}
<_SHELLECHO>.						{yyappend(yytext);}

"function"						{yyclean(); BEGIN _FUNCTION;}
<_FUNCTION>[ \t\n]+					 /*Eat these up */
<_FUNCTION>[_a-zA-Z][a-zA-Z0-9]*			{yyappend(yytext); BEGIN INITIAL; if(LEXERVERBOSE())printf("[func|%s]\n",yylval); return FUNC;}

"#*"							{BEGIN _MLCMNT;}
<_MLCMNT>([^*]|\n)+|.					/* Do nothing */
<_MLCMNT><<EOF>>					/* Error, but treat as if nothing happened */
<_MLCMNT>"*#"						{BEGIN INITIAL;}

([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)			{yyclean(); yyappend(yytext); if(LEXERVERBOSE())printf("[nstring|%s]\n",yylval);return NSTRING;}

[_a-zA-Z0-9]+						{yyclean(); yyappend(yytext); if(LEXERVERBOSE())printf("[genstring]%s\n",yylval); return GSTRING;}

["]							{yyclean(); BEGIN _STRING;}
<_STRING>["]						{BEGIN INITIAL; if(LEXERVERBOSE())printf("[string|%s]\n",yylval); return STRING;}
<_STRING>.						{yyappend(yytext);}

[ \t\n]							/* Eat up whitespaces */
.							{if(LEXERVERBOSE())printf("[error-lexer|%s]\n", yytext);return ERR; }

%%

int set_read_file(char *filename){
	if(!fopen(filename, "r"))
		if(LEXERVERBOSE())printf("cannot find file");
	if(!(yyin = fopen(filename, "r")))
		return 1;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	return 0;
}

void yyclean(){
	/*Cleans a given string, just adds '\0' in the 0th position */
	yylval = malloc(sizeof(char)*3000);
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
