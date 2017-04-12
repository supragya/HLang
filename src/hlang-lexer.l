/* HLANG INTERPRETER development stage 0 
 * created by SUPRAGYA RAJ
 */

%option noyywrap
%x _MLCMNT _FUNCTION _STRING _SHELLECHO
%{
#include <string.h>
#define TESTFOLDER "tests/"
#define VERSION 0.01
#define DEBUG 1
char yylval[2000];


/* DEFINING TYPES 
 * Anything that does not matches the above is shown as **E{string}** during tests
 */

enum yytokentype{
	MAPDECL		=	601,
	VARDECL		=	602,
	VARNAME		=	603,
	MELNAME		=	604,
	ARGVAR		=	605,
	NVAL		=	606,
	STRING		=	607,
	BROPEN		=	610,
	BRCLOSE		=	611,
	SHELLECHO	=	612,
	FUNC		=	613,
	IF		=	614,
	ELIF		=	615,
	ELSE		=	616,
	WHILE		=	617,
	EOS		=	618,
	PARANOPEN	=	619,
	PARANCLOSE	=	620,
	ASSIGN		=	621,
	FUNCCALL	=	622,
	COMMA		=	623,
	GT		=	624,
	LT		=	625,
	EQ		=	626,
	NQ		=	627,
	GE		=	628,
	LE		=	629,
	ERR		=	630
};

void strclean(char *);
void strappend(char *, char *);
int has_lval(int);
%}

%%

"declare"[ \t\n]*"map"					{strclean(yylval); return MAPDECL;}
"declare"						{strclean(yylval); return VARDECL;}
[$][_a-zA-Z][_a-zA-Z0-9]+				{strcpy(yylval, yytext+1); return VARNAME;}
[$][_a-zA-Z][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"		{strcpy(yylval, yytext+1); return MELNAME;}
[$][0-9]+						{strcpy(yylval, yytext+1); return ARGVAR;}
([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)			{strcpy(yylval, yytext); return NVAL;}

["]							{strclean(yylval); BEGIN _STRING;}
<_STRING>["]						{BEGIN INITIAL; return STRING;}
<_STRING>.						{strappend(yylval, yytext);}

[#][^*].*[^\n]						/*Single line comments, do nothing */
[{]							{strclean(yylval); return BROPEN;}
[}]							{strclean(yylval); return BRCLOSE;}

"<%"							{strclean(yylval); BEGIN _SHELLECHO;}
<_SHELLECHO>">"						{BEGIN INITIAL; return SHELLECHO;}
<_SHELLECHO>.						{strappend(yylval, yytext);}								

"function"						{strclean(yylval); BEGIN _FUNCTION;}
<_FUNCTION>[ \t\n]+					 /*Eat these up */
<_FUNCTION>[a-zA-Z][a-zA-Z0-9]*				{strappend(yylval, yytext); BEGIN INITIAL;}

"#*"							{BEGIN _MLCMNT;}
<_MLCMNT>([^*]|\n)+|.					/* Do nothing */
<_MLCMNT><<EOF>>					/* Error, but treat as if nothing happened */
<_MLCMNT>"*#"						{BEGIN INITIAL;}

"if"							{return IF;}
"elif"							{return ELIF;}
"else"							{return ELSE;}
"while"							{return WHILE;}

[a-zA-Z][a-zA-Z0-9]*"("					{return FUNC;}
[(]							{return PARANOPEN;}
[)]							{return PARANCLOSE;}	

">"							{return GT;}
"<"							{return LT;}
"=="							{return EQ;}
"!="|"<>"						{return NQ;}
">="							{return GE;}
"<="							{return LE;}

[=]							{return ASSIGN;}
[;]							{return EOS;}
[,]							{return COMMA;}
[ \t]							/* Eat up whitespaces */
							/* [\n] NEWLINE REMOVER TO BE INSERTED */
.							{return ERR; } 

%%

int main(char **argv){
	/* Debug welcome */
	DEBUG?printf("+--------------------------\n"):0;
	DEBUG?printf("|HLANG LEXER %0.2f\n", VERSION):0;
	DEBUG?printf("+--------------------------\n"):0;
	
	/*Define testfiles */
	char tests[][50]	=	{	"variable_declarations.hl",
								"comments.hl",
								"functions.hl",
								"selections.hl",
								"elastic_horse_regressions.hl",
								""};
	unsigned int i = 0;
	int tok;
	/* Test file parsing */
	while(strcmp(tests[i], "")){

		/* Absolutize file name */
		char workfile[200] = TESTFOLDER;
		strcat(workfile, tests[i]);

		/* Opening the test file for parsing */
		if(!(yyin = fopen(workfile, "r"))){
			DEBUG?printf(">>>Error opening %s, aborting\n", workfile):0;
			perror(argv[1]);
			return 1;
		}
		DEBUG?printf(">>>%s opened successfully\n", workfile):0;

		/* Start lexical analysis on the file */
		while(tok = yylex()){
			printf("%d ", tok);
			if(has_lval(tok)){
				printf(yylval);
			}
			printf("\n ");
			if(tok == FUNC){
				printf("%d", PARANOPEN);
			}
		}	
		DEBUG?printf(">>>Hlang lexer completes\n"):0;
		return 0;
		/* Increment the counter */
		i++;
	}
}

void strclean(char *str){
	/*Cleans a given string, just adds '\0' in the 0th position */
	str[0] = '\0';	
}

void strappend(char *str1, char *str2){
	int str1addpoint = 0, str2len = 0;
	while(str1[str1addpoint++] != '\0');
	while(str2[str2len++] != '\0');
	str2len--;
	
	int i;
	for(i = 0; i<str2len; i++){
		str1[str1addpoint+i] = str2[i];
	}
	str1[str1addpoint+str2len] = '\0';
}

int has_lval(int tokentype){
	int lvaltypes[] = {VARNAME, MELNAME, ARGVAR, NVAL, STRING, SHELLECHO, FUNC, FUNCCALL, -1};
	int i;
	for(i = 0;lvaltypes[i] != -1;i++)
		if(tokentype == lvaltypes[i]) 
			return 1;
	return 0;
}







