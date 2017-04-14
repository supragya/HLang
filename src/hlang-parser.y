/* HLang Parser Bison 
 * created by Supragya Raj
 */

%{
#include <stdio.h>
#include "../headers/buildtime_hlang-lexer.h"
#include "../headers/hlang-lexer.h"
#define DATABUFLEN 2000
int yyerror(char *);
%}

%define api.value.type {char *}

/* Terminals */
%token	MAPDECL	VARDECL	VARNAME	MELNAME	ARGVAR
%token	NVAL	STRING	BROPEN	BRCLOSE	SHELLECHO
%token	FUNC	IF	ELIF	ELSE	WHILE
%token	EOS	PARANOPEN	PARANCLOSE
%token	ASSIGN	FUNCCALL	COMMA
%token	GT	LT	EQ	NQ	GE
%token	LE	ERR	EOL

%%

input:
	%empty
	|input line 	
	;

line:	stmt EOS
	;

stmt:	VARDECL varlist
	|MAPDECL maplist
	;

varlist:var
	|varlist COMMA var
	;

maplist:mapvar
	|maplist COMMA mapvar
	;

var:	VARNAME				{printf("varname ");}
	|VARNAME ASSIGN STRING		{printf("ass_nvar %s ", yylval);}
	|VARNAME ASSIGN NVAL		{printf("ass_svar %s ", yylval);}
	;

mapvar:	VARNAME
	;
%%

int main(char **argv){
	printf("+-------------------------------+\n");
	printf("|       HLang Interpreter       |\n");
	printf("+-------------------------------+\n");

	yylval = malloc(sizeof(char)*DATABUFLEN);

	char testset[][100] = {	"tests/variable_declarations.hl",
				"tests/selections.hl",
				"tests/functions.hl",
				"tests/comments.hl",
				"tests/elastic_horse_regressions.hl",
				"" };
	unsigned int i = 0;
	while(strcmp(testset[i],"")){		
		printf("\n>>>PARSING FILE \"%s\"+\n", testset[i]);
		if(set_read_file(testset[i]))
			return 1; //Cannot find file
		else
			yyparse();
		i++;
	}
	printf("\n+-COMPLETE----------------------+\n");
}

int yyerror(char *s){
	fprintf(stderr, "error: %s\n", s);
	return 0;
}
