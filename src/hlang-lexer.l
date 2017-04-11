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
int funccallopen = 0;
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
	SLCMNT		=	608,
	MLCMNT		=	609,
	BROPEN		=	610,
	BRCLOSE		=	611,
	SHELLECHO	=	612,
	FUNC		=	613,
	IF			=	614,
	ELIF		=	615,
	ELSE		=	616,
	WHILE		=	617,
	EOS			=	618,
	PARANOPEN	=	619,
	PARANCLOSE	=	620,
	ASSIGN		=	621,
	FUNCCALL	=	622,
	COMMA		=	623,
	GT			=	624,
	LT			=	625,
	EQ			=	626,
	NQ			=	627,
	GE			=	628,
	LE			=	629
};

%}

%%

"declare"[ \t\n]*"map"						{strcpy(yylval, "0"); return MAPDECL;}
"declare"									{strcpy(yylval, "0"); return VARDECL;}
[$][_a-zA-Z][_a-zA-Z0-9]+					{strcpy(yylval, yytext+1); return VARNAME;}
[$][_a-zA-Z][_a-zA-Z0-9]+"["[a-zA-Z0-9]+"]"	{strcpy(yylval, yytext+1); return MELNAME;}
[$][0-9]+									{strcpy(yylval, yytext+1); return ARGVAR;}
([0-9]+|[0-9]+[.][0-9]*|[.][0-9]+)			{strcpy(yylval, yytext); return NVAL;}

["]											{strclean(yylval); BEGIN _STRING;}
<_STRING>["]								{BEGIN INITIAL; return STRING}
<_STRING>.									{strappend(yylval, yytext);}/*eat up*/

[#][^*].*[^\n]								{strcpy(yylval, yytext+1); return SLCMNT;}
[{]											{strcpy(yylval, "0"); return BROPEN;}
[}]											{strcpy(yylval, "0"); return BRCLOSE;}

"<%"							{printf("<SHELLECHO>{"); BEGIN SHELLECHO;}
<SHELLECHO>">"					{printf("} "); BEGIN INITIAL;}
<SHELLECHO>.					{printf(yytext);}								

"function"						{printf("<FUNC>");
								 BEGIN FUNCTION;}
<FUNCTION>[ \t\n]+				 /*Eat these up */
<FUNCTION>[a-zA-Z][a-zA-Z0-9]*	{printf("{%s} ", yytext);
								 BEGIN INITIAL;}

"#*"							{printf("<MLCMNT>{");
								 BEGIN MLCMNT;}
<MLCMNT>([^*]|\n)+|.			{printf(yytext);}
<MLCMNT><<EOF>>					{printf("[Unterminated MLCMNT]"); return 0;}
<MLCMNT>"*#"					{printf("} ");
								 BEGIN(INITIAL);}

"if"							{printf("<IF>{0} ");}
"elif"							{printf("<ELIF>{0} ");}
"else"							{printf("<ELSE>{0} ");}
"while"							{printf("<WHILE>{0} ");}

[a-zA-Z][a-zA-Z0-9]*"("			{printf("<FUNCCALL>{%s} <PARANOPEN>{0} ",yytext);}
[(]								{printf("<PARANOPEN>{0} ");}
[)]								{printf("<PARANCLOSE>{0} ");}	

">"								{printf("<GT>{0} ");}
"<"								{printf("<LT>{0} ");}
"=="							{printf("<EQ>{0} ");}
"!="|"<>"						{printf("<NQ>{0} ");}
">="							{printf("<GE>{0} ");}
"<="							{printf("<LE>{0} ");}

[=]								{printf("<ASSIGN>{0} "); }
[;]								{printf("<EOS>{0} "); }
[,]								{printf("<COMMA>{0} ");}
[ \t]							/* Eat up whitespaces */
								/* [\n] */
.								{printf("**E:%s** ", yytext); } 

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
		DEBUG?printf("\n"):0;
		yylex();
		DEBUG?printf("\n\n"):0;

		/* Increment the counter */
		i++;
	}

	DEBUG?printf(">>>Hlang lexer completes\n"):0;
	return 0;
}
