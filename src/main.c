/* HLang Main Entry function 
 * created by Supragya Raj
 */

#include <stdio.h>
#include "buildtime_hlang-parser.h"
#include "buildtime_hlang-lexer.h"
#include "hlang-lexer.h"
#define DATABUFLEN 2000

/* Function declarations */
void welcome();
void complete();

int main(char **argv){
	/* Print welcome message on screen */
	welcome();

	/* Allocate yylval legit space */
	yylval = malloc(sizeof(char)*DATABUFLEN);
	if(!yylval){
		printf("No memory for yylval\n");
		return 1;
	}

	/* Test files list */
	char testset[][100] = {	"tests/variable_declarations.hl",
				"tests/selections.hl",
				"tests/functions.hl",
				"tests/comments.hl",
				"tests/elastic_horse_regressions.hl",
				"" };
	/* Iterate through all the tests */
	unsigned int i = 0;
	while(strcmp(testset[i],"")){		
		printf("\n>>>PARSING FILE \"%s\"\n", testset[i]);
		if(set_read_file(testset[i]))
			return 1; //Cannot find file
		else
			yyparse();
		i++;
	}

	/* Print complete message on screen */
}

void welcome(){
	printf("+-------------------------------+\n");
	printf("|       HLang Interpreter       |\n");
	printf("+--------------------------0.1b-+\n");
}

void complete(){	
	printf("\n+-COMPLETE----------------------+\n");
}
