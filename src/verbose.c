#include <stdio.h>
#include "verbose.h"

int verbosedata[3] = {0,0,0};

int LEXERVERBOSE(){
	return verbosedata[0];
}
int PARSERVERBOSE(){
	return verbosedata[1];
}
int ASTVERBOSE(){
	return verbosedata[2];
}

void verbose_set_lexer(){
	verbosedata[0] = 1;
}
void verbose_set_parser(){
	verbosedata[1] = 1;
}
void verbose_set_ast(){
	verbosedata[2] = 1;
}

void verbose_status_display(){
	printf("[HLang interpreter verbose settings]: Modules set to verbose:");
	if(verbosedata[0])
		printf(" {lexer} ");
	if(verbosedata[1])
		printf(" {parser} ");
	if(verbosedata[2])
		printf(" {ast} ");
	if(!verbosedata[0] && !verbosedata[1] &&!verbosedata[2])
		printf(" NONE ");
	printf("\n");
}
