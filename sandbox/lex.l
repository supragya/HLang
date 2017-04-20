/* HLang Lexer Flex
 * created by Supragya Raj
 */

%option noyywrap
%x _MLCMNT _FUNCTION _STRING _SHELLECHO
%{
#include <string.h>
#include "parse.h"
int ctr = 1;

%}

%%

"1"			{printf("1\n"); yylval = "nether"; return ONE;}

"2"			{printf("2\n"); yylval = "noter"; return TWO;}

%%

int set_read_file(){
	if(!fopen("testfile", "r"))
		printf("cannot find file");
	if(!(yyin = fopen("testfile", "r")))
		return 1;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	return 0;
}
