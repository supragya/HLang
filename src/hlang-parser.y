/* HLang Parser Bison 
 * created by Supragya Raj
 */

%{
#include <stdio.h>
%}

/* Terminals */
%token	MAPDECL	VARDECL	VARNAME	MELNAME	ARGVAR
%token	NVAL	STRING	BROPEN	BRCLOSE	SHELLECHO
%token	FUNC	IF	ELIF	ELSE	WHILE
%token	EOS	PARANOPEN	PARANCLOSE
%token	ASSIGN	FUNCCALL	COMMA
%token	GT	LT	EQ	NQ	GE
%token	LE	ERR	EOL

%%

start:	
	|start VARDECL EOL 	{printf("%d\n", $1);}
	;
%%

int main(char **argv){
	yyparse();
}

int yyerror(char *s){
	fprintf(stderr, "error: %s\n", s);
	return 0;
}
