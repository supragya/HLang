/* HLang Main Entry function
 * created by Supragya Raj
 */

#include <stdio.h>
#include "buildtime_hlang-parser.h"
#include "buildtime_hlang-lexer.h"
#include "hlang-lexer.h"
#include "version.h"
#include "variable_mgmt.h"
#include "scope_mgmt.h"
#include "ast.h"

#define DATABUFLEN 2000

/* Function declarations */
void welcome();
void complete();

int main(char **argv){
	/* Print welcome message on screen */
	welcome();

	if (vms_init()){
		printf(">>>Error: Cannot initialise variable management subsystem\n");
		return 1;
	}
	if (sms_init()){
		printf(">>>Error: Cannot initialise scope management subsystem\n");
		return 1;
	}
	if (ast_init()){
		printf(">>>Error: Cannot initialise abstract syntax tree\n");
		return 1;
	}
	/* Begin testing ast */
	/* Allocate yylval legit space */
	yylval = malloc(sizeof(char)*DATABUFLEN);
	if(!yylval){
		printf(">>>No memory for yylval\n");
		return 1;
	}

	/* Test files list */
	char testset[][100] = {	"tests/aux_test.hl",

				"" };
	/* Iterate through all the tests */
	unsigned int i = 0;
	while(strcmp(testset[i],"")){
		printf("\n>>>PARSING FILE \"%s\"===============================================\n", testset[i]);
		if(set_read_file(testset[i]))
			return 1; //Cannot find file
		else
			yyparse();
		i++;
	}

	/* Print complete message on screen */
}

void welcome(){
	printf("+--------------------------------------------+\n");
	printf("|      HLang Interpreter - %s   |\n", version_name[VERSION]);
	printf("+-%s------------------------------------%s-+\n", WIP?"wip":"---", DRAFT );
	printf("\nAll verbose outputs are as follows:\n[lexer output]\n\t<PARSER OUTPUT>\n");
}

void complete(){
	printf("\n+-COMPLETE----------------------+\n");
}
