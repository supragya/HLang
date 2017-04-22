#include <stdio.h>
#include "verbose.h"

int verbosedata[] = {0,0,0,0,0,0};

int LEXERVERBOSE(){
	return verbosedata[0];
}
int PARSERVERBOSE(){
	return verbosedata[1];
}
int ASTVERBOSE(){
	return verbosedata[2];
}
int EXECVERBOSE(){
	return verbosedata[3];
}
int VMSVERBOSE(){
	return verbosedata[4];
}
int SMSVERBOSE(){
	return verbosedata[5];
}

void verbose_set_verbose(char ch){
	switch(ch){
		case 'l': verbosedata[0]=1;break;
		case 'p': verbosedata[1]=1;break;
		case 'a': verbosedata[2]=1;break;
		case 'e': verbosedata[3]=1;break;
		case 'v': verbosedata[4]=1;break;
		case 's': verbosedata[5]=1;break;
	}
}

void verbose_status_display(){
	printf("[HLang interpreter verbose settings]: Modules set to verbose:");
	if(verbosedata[0])
		printf(" {lexer} ");
	if(verbosedata[1])
		printf(" {parser} ");
	if(verbosedata[2])
		printf(" {ast} ");
	if(verbosedata[3])
		printf(" {exec} ");
	if(verbosedata[4])
		printf(" {vms} ");
	if(verbosedata[5])
		printf(" {sms} ");
	if(!verbosedata[0] && !verbosedata[1] &&!verbosedata[2] &&!verbosedata[3] &&!verbosedata[4] &&!verbosedata[5])
		printf(" NONE ");
	printf("\n");
}
