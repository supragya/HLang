
%{
#include <stdio.h>
#include "lex.h"
int yyerror(const char *s);
int set_read_file();
%}
%define api.value.type {char *}
%verbose
%error-verbose

/* Terminals */
%token	ONE TWO

%%

start:
	onevar twovar			{printf("\t<ONEVAR: %d, TWOVAR: %d>\n", $1, $2);}
	;

onevar:
	ONE				{printf("\t<ONE: %d>\n", $$);}
	;

twovar:
	TWO				{printf("\t<TWO: %d>\n", $$);}
	;


%%

int main(char **argv){
	set_read_file();
	int r = yyparse();
	return 0;
}

int yyerror(const char *s){
	fprintf(stdout, "{{error: %s}}\n", s);
	return 0;
}
