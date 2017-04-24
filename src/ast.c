/* HLang Abstract Syntax Tree
 * created by Supragya Raj
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ast.h"
#include "verbose.h"

int ast_init(){
	rootnode = malloc(sizeof(struct ast_root_node));
	currentconstructhead = NULL;
	if(!rootnode)
		return 1;
	rootnode->functions = NULL;
	currentsequentialhead = NULL;
	currentkeyvalpairshead = NULL;
	currentmapvarlisthead = NULL;
	currentvardeclassignmentlisthead = NULL;
	currentfunccallargshead = NULL;
	currentreturnvalhead = NULL;
	currentexpression1head = NULL;
	currentexpression2head = NULL;
	currentexpression3head = NULL;
	currentexpression3discretermhead = NULL;
	currentexpression3unaryprecederhead = NULL;
	currentexpression3successorhead = NULL;
	currentvarassignmentshead = NULL;
	return 0;
}

void ast_add_function(char *functionname){
	if(ASTVERBOSE())printf(":AST: Adding functionname: %s\n", functionname);
	struct ast_function_node *temp_function = malloc(sizeof(struct ast_function_node));

	temp_function->executionlist = currentconstructhead;
	currentconstructhead = NULL;

	temp_function->functionname = malloc(sizeof(char)*(strlen(functionname)+1));
	strcpy(temp_function->functionname, functionname);
	temp_function->next = rootnode->functions;
	rootnode->functions = temp_function;

	if(ASTVERBOSE() && temp_function->executionlist == NULL)printf(":AST: This function is empty\n");
	ast_walk_constructs(temp_function->executionlist);
	if(ASTVERBOSE())printf(":AST: Current functions in AST: ");
	while(temp_function != NULL){
		if(ASTVERBOSE())printf("  {%s}  ", temp_function->functionname);
		temp_function = temp_function->next;
	}
	if(ASTVERBOSE())printf("\n");
}

void ast_add_seq(){
	struct ast_sequentialnode *newseqnode;
	newseqnode = malloc(sizeof(struct ast_sequentialnode));
	//newseqnode->next.ctype = NONE;
	if(currentsequentialhead != NULL){
		newseqnode->childtype = currentsequentialhead->childtype;
		switch(newseqnode->childtype){
			case AST_GENVARDECL: if(ASTVERBOSE())printf(":AST: Registering sequential construct GENVARDECL\n");newseqnode->child.genvardecl = currentsequentialhead->child.genvardecl; break;
			case AST_MAPVARDECL: if(ASTVERBOSE())printf(":AST: Registering sequential construct MAPVARDECL\n");newseqnode->child.mapvardecl = currentsequentialhead->child.mapvardecl; break;
			case AST_ASSIGNMENTS: if(ASTVERBOSE())printf(":AST: Registering sequential construct ASSIGNMENTS\n");newseqnode->child.assignments = currentsequentialhead->child.assignments; break;
			case AST_RETURN: if(ASTVERBOSE())printf(":AST: Registering sequential construct RETURN\n");newseqnode->child._return = currentsequentialhead->child._return; break;
			case AST_SHELLECHO: if(ASTVERBOSE())printf(":AST: Registering sequential construct SHELLECHO\n");newseqnode->child.shellecho = currentsequentialhead->child.shellecho; break;
			case AST_FUNCTIONCALL: if(ASTVERBOSE())printf(":AST: Registering sequential construct FUNCTIONCALL\n");newseqnode->child.functioncall = currentsequentialhead->child.functioncall; break;
		}
		free(currentsequentialhead);
		currentsequentialhead = NULL;
		// if(ASTVERBOSE())printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
		// if(ASTVERBOSE())printf(":AST: newseqnode made with name %s and value %s\n", newseqnode->name, newseqnode->child.shellecho->value);
	}
	if(ASTVERBOSE())printf(":AST: newseqnode made\n");
	if(currentconstructhead == NULL){
		if(ASTVERBOSE())printf(":AST: currentconstructhead is empty at this point, allocating memory\n");
		currentconstructhead = malloc(sizeof(struct ast_construct));
		currentconstructhead->ptr.sequential = newseqnode;
		currentconstructhead->ctype = SEQUENTIAL;
		currentconstructhead->next = NULL;
	}
	else{
		// if(ASTVERBOSE())printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = currentconstructhead;
		while(temp_construct->next != NULL){
			temp_construct = temp_construct->next;
		}
		temp_construct->next = malloc(sizeof(struct ast_construct));
		temp_construct = temp_construct->next;
		temp_construct->ctype = SEQUENTIAL;
		temp_construct->ptr.sequential= newseqnode;
	}
	ast_walk_constructs(currentconstructhead);
}

void ast_add_sel(char *name){
	struct ast_selectivenode *newselnode;
	newselnode = malloc(sizeof(struct ast_selectivenode));
	newselnode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newselnode->name, name);
	// newselnode->next.ctype = NONE;
	// if(ASTVERBOSE())printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	if(ASTVERBOSE())printf(":AST: newselnode made with name %s\n", newselnode->name);
	if(currentconstructhead == NULL){
		currentconstructhead = malloc(sizeof(struct ast_construct));
		currentconstructhead->ptr.selective = newselnode;
		currentconstructhead->ctype = SELECTIVE;
		currentconstructhead->next = NULL;
	}
	else{
		// if(ASTVERBOSE())printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = currentconstructhead;
		while(temp_construct->next != NULL){
			temp_construct = temp_construct->next;
		}
		temp_construct->next = malloc(sizeof(struct ast_construct));
		temp_construct = temp_construct->next;
		temp_construct->ctype = SELECTIVE;
		temp_construct->ptr.selective = newselnode;
	}
	ast_walk_constructs(currentconstructhead);
}

void ast_add_iter(char *name){
	struct ast_iterativenode *newiternode;
	newiternode = malloc(sizeof(struct ast_iterativenode));
	newiternode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newiternode->name, name);
	//newiternode->next.ctype = NONE;
	// if(ASTVERBOSE())printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	if(ASTVERBOSE())printf(":AST: newiternode made with name %s\n", newiternode->name);
	if(currentconstructhead == NULL){
		currentconstructhead = malloc(sizeof(struct ast_construct));
		currentconstructhead->ptr.iterative = newiternode;
		currentconstructhead->ctype = ITERATIVE;
		currentconstructhead->next = NULL;
	}
	else{
		// if(ASTVERBOSE())printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = currentconstructhead;
		while(temp_construct->next != NULL){
			temp_construct = temp_construct->next;
		}
		temp_construct->next = malloc(sizeof(struct ast_construct));
		temp_construct = temp_construct->next;
		temp_construct->ctype = ITERATIVE;
		temp_construct->ptr.iterative = newiternode;
	}
	ast_walk_constructs(currentconstructhead);
}

void ast_walk_constructs(struct ast_construct *head){
	if(ASTVERBOSE())printf(":AST: Construct walk::::  ");
	struct ast_construct *temp_construct = head;
	while(temp_construct != NULL){
		switch(temp_construct->ctype){
			case SEQUENTIAL: if(ASTVERBOSE())printf("  [SEQUENTIAL]   ");break;
			case SELECTIVE: if(ASTVERBOSE())printf("   [SELECTIVE]   "); break;
			case ITERATIVE: if(ASTVERBOSE())printf("   [ITERATIVE]   "); break;
		}
		temp_construct = temp_construct->next;
	}
	if(ASTVERBOSE())printf("\n" );
}

void ast_add_seq_shellecho(char *echo){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_SHELLECHO;
	char *temp = malloc(sizeof(char)*(strlen(echo)+1));
	strcpy(temp,echo);
	currentsequentialhead->child.shellecho = malloc(sizeof(struct ast_sequential_shellecho));
	currentsequentialhead->child.shellecho->value = temp;
	if(ASTVERBOSE())printf(":AST: Added shellecho: %s\n", currentsequentialhead->child.shellecho->value);
}

void ast_make_keyvalpair(char *key, char *value){
	struct keyvalpairs *temp = malloc(sizeof(struct keyvalpairs));
	temp->key = malloc(sizeof(char)*(strlen(key)+1));
	temp->value = malloc(sizeof(char)*(strlen(value)+1));
	strcpy(temp->key, key);
	strcpy(temp->value, value);
	temp->next = currentkeyvalpairshead;
	currentkeyvalpairshead = temp;
	if(ASTVERBOSE())printf(":AST: Made keyvaluepair: {%s|%s}\n", temp->key, temp->value);

	if(ASTVERBOSE())printf(":AST: Current keyvalpairs: ");
	temp = currentkeyvalpairshead;
	while(temp!=NULL){
		if(ASTVERBOSE())printf("  {%s|%s}  ", temp->key, temp->value);
		temp = temp->next;
	}
	if(ASTVERBOSE())printf("\n");
}

void ast_add_mapdeclnode(char *mapname){
	struct mapvarlist *newmapvar = malloc(sizeof(struct mapvarlist));
	newmapvar->mapname = malloc(sizeof(char)*(strlen(mapname)+1));
	strcpy(newmapvar->mapname, mapname);
	newmapvar->keyvalpairs = currentkeyvalpairshead;
	currentkeyvalpairshead = NULL;
	newmapvar->next = currentmapvarlisthead;
	currentmapvarlisthead = newmapvar;
	if(ASTVERBOSE())printf(":AST: Registered mapdeclaration node with data: ");
	struct keyvalpairs *temp = newmapvar->keyvalpairs;
	while(temp!=NULL){
		if(ASTVERBOSE())printf("  {%s|%s}  ", temp->key, temp->value);
		temp = temp->next;
	}
	if(ASTVERBOSE())printf("\n");
}

void ast_add_seq_mapdecl(){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_MAPVARDECL;
	currentsequentialhead->child.mapvardecl = malloc(sizeof(struct ast_sequential_mapvardecl));
	currentsequentialhead->child.mapvardecl->mapvarlist = currentmapvarlisthead;
	currentmapvarlisthead = NULL;
}

void ast_add_seq_vardecl(){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_GENVARDECL;
	currentsequentialhead->child.genvardecl = malloc(sizeof(struct ast_sequential_genvardecl));
	currentsequentialhead->child.genvardecl->list = currentvardeclassignmentlisthead;
	currentvardeclassignmentlisthead = NULL;
}

void ast_make_vardecl_assignment(char *varname, char *value){
	struct vardecl_assignmentlist *tempvardeclassignmentlist = malloc(sizeof(struct vardecl_assignmentlist));
	tempvardeclassignmentlist->varname = malloc(sizeof(char)*(strlen(varname)+1));
	tempvardeclassignmentlist->value = malloc(sizeof(char)*(strlen(value)+1));
	strcpy(tempvardeclassignmentlist->varname, varname);
	strcpy(tempvardeclassignmentlist->value, value);
	tempvardeclassignmentlist->next = currentvardeclassignmentlisthead;
	currentvardeclassignmentlisthead = tempvardeclassignmentlist;
	if(ASTVERBOSE())printf(":AST: Made vardecl assignment :{%s|%s}\n", tempvardeclassignmentlist->varname, tempvardeclassignmentlist->value);

	if(ASTVERBOSE())printf(":AST: Current vardecl list state: ");
	tempvardeclassignmentlist = currentvardeclassignmentlisthead;
	while(tempvardeclassignmentlist!=NULL){
		if(ASTVERBOSE())printf("  {%s|%s}  ", tempvardeclassignmentlist->varname, tempvardeclassignmentlist->value);
		tempvardeclassignmentlist = tempvardeclassignmentlist->next;
	}
	if(ASTVERBOSE())printf("\n");
}

void ast_make_vardecl_assignment_defaultval(char *varname){
	ast_make_vardecl_assignment(varname, "0");
}

void ast_add_arguments_string(char *argstr){
	struct functioncallargs *temparg = malloc(sizeof(struct functioncallargs));
	temparg->argtype = F_STRING;
	temparg->argument_str = malloc(sizeof(char)*(strlen(argstr)+1));
	temparg->next = NULL;
	strcpy(temparg->argument_str, argstr);

	if(currentfunccallargshead == NULL)
		currentfunccallargshead = temparg;
	else{
		struct functioncallargs *traverser = currentfunccallargshead;
		while(traverser->next != NULL)
			traverser = traverser->next;
		traverser->next = temparg;
	}

	if(ASTVERBOSE())printf(":AST: Added functioncall argument {stringtype|%s}\n", temparg->argument_str);
	if(ASTVERBOSE())printf(":AST: Current argument list: ");
	temparg = currentfunccallargshead;
	while(temparg != NULL){
		if(temparg->argtype == F_STRING){
			if(ASTVERBOSE())printf("  {stringtype|%s}  ", temparg->argument_str);
		}
		else if(temparg->argtype == F_VARNAME){
			if(ASTVERBOSE())printf("  {VARNAME|%s}  ", temparg->argument_str);
		}
		temparg = temparg->next;
	}
	if(ASTVERBOSE())printf("\n");
}

void ast_add_arguments_varname(char *argstr){
	struct functioncallargs *temparg = malloc(sizeof(struct functioncallargs));
	temparg->argtype = F_VARNAME;
	temparg->argument_str = malloc(sizeof(char)*(strlen(argstr)+1));
	temparg->next = NULL;
	strcpy(temparg->argument_str, argstr);

	if(currentfunccallargshead == NULL)
		currentfunccallargshead = temparg;
	else{
		struct functioncallargs *traverser = currentfunccallargshead;
		while(traverser->next != NULL)
			traverser = traverser->next;
		traverser->next = temparg;
	}

	if(ASTVERBOSE())printf(":AST: Added functioncall argument {vartype|%s}\n", temparg->argument_str);
	if(ASTVERBOSE())printf(":AST: Current argument list: ");
	temparg = currentfunccallargshead;
	while(temparg != NULL){
		if(temparg->argtype == F_STRING){
			if(ASTVERBOSE())printf("  {stringtype|%s}  ", temparg->argument_str);
		}
		else if(temparg->argtype == F_VARNAME){
			if(ASTVERBOSE())printf("  {VARNAME|%s}  ", temparg->argument_str);
		}
		temparg = temparg->next;
	}
	if(ASTVERBOSE())printf("\n");
}
void ast_add_seq_functioncall(char *functionname){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_FUNCTIONCALL;
	currentsequentialhead->child.functioncall = malloc(sizeof(struct ast_sequential_functioncall));
	currentsequentialhead->child.functioncall->functionname = malloc(sizeof(char)*(strlen(functionname)+1));
	strcpy(currentsequentialhead->child.functioncall->functionname, functionname);
	currentsequentialhead->child.functioncall->args = currentfunccallargshead;
	currentfunccallargshead = NULL;
	if(ASTVERBOSE())printf(":AST: Found a functioncall to {%s}\n", currentsequentialhead->child.functioncall->functionname);
}
void ast_set_returnval_expression(){
	currentreturnvalhead = malloc(sizeof(struct returnval));
	currentreturnvalhead->type = R_EXPRESSION;
	currentreturnvalhead->expr = currentexpression1head->data;
	currentexpression1head = currentexpression1head->next;
	if(ASTVERBOSE())printf(":AST: Setting the return value to expression\n");
	ast_display_exprll_status();
}
void ast_add_seq_return(){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_RETURN;
	currentsequentialhead->child._return = malloc(sizeof(struct ast_sequential_return));
	currentsequentialhead->child._return->retdata = currentreturnvalhead;
	currentreturnvalhead = NULL;
}
void ast_add_expr3_discrete_term_string(char *str){
	if(ASTVERBOSE())printf(":AST: Adding expression 3 discrete term to the expression with string %s\n", str);
	struct expr_discrete_termll *tempterm = malloc(sizeof(struct expr_discrete_termll));
	tempterm->next = currentexpression3discretermhead;
	currentexpression3discretermhead = tempterm;
	tempterm->data = malloc(sizeof(struct expr_discrete_term));
	tempterm->data->type = DISCRETE_STRING;
	tempterm->data->termdata = malloc(sizeof(char)*(strlen(str)+1));
	strcpy(tempterm->data->termdata, str);
	ast_display_expr3_discrete_termll_status();
}
void ast_add_expr3_discrete_term_variable(char *varname){
	if(ASTVERBOSE())printf(":AST: Adding expression 3 discrete term to the expression with varname %s\n", varname);
	struct expr_discrete_termll *tempterm = malloc(sizeof(struct expr_discrete_termll));
	tempterm->next = currentexpression3discretermhead;
	currentexpression3discretermhead = tempterm;
	tempterm->data = malloc(sizeof(struct expr_discrete_term));
	tempterm->data->type = DISCRETE_VARIABLE;
	tempterm->data->termdata = malloc(sizeof(char)*(strlen(varname)+1));
	strcpy(tempterm->data->termdata, varname);
	ast_display_expr3_discrete_termll_status();
}
void ast_add_expr3_discrete_term_functioncall(){
}
void ast_add_expr3_discrete_term_shellecho(char *shellecho){
}
void ast_add_expr3_unprec(enum expr_unary_preceder_type type){
	if(ASTVERBOSE())printf(":AST: Adding expression 3 unary precedor type: ");
	switch(type){
		case UNARY_NEG: if(ASTVERBOSE())printf(" UNARY_NEG \n"); break;
		case UNARY_POS: if(ASTVERBOSE())printf(" UNARY POS \n"); break;
	}

	struct expr_unary_precederll *temp = malloc(sizeof(struct expr_unary_precederll));
	temp->next = currentexpression3unaryprecederhead;
	currentexpression3unaryprecederhead = temp;
	temp->data = malloc(sizeof(struct expr_unary_preceder));
	temp->data->precedertype = type;
	//ast_display_expr3_unary_precll_status();
}
void ast_display_expr3_discrete_termll_status(){
	if(ASTVERBOSE())printf(":AST: Current expression3 discrete term LL state: ");
	struct expr_discrete_termll *tempterm = currentexpression3discretermhead;
	while(tempterm != NULL){
		switch(tempterm->data->type){
			case DISCRETE_STRING: printf("  {string|%s}  ", tempterm->data->termdata); break;
			case DISCRETE_VARIABLE: printf("  {variable|%s}  ", tempterm->data->termdata); break;
		}
		tempterm = tempterm->next;
	}
	printf("\n");
}
void ast_display_expr3_unary_precll_status(){
	if(ASTVERBOSE())printf(":AST: Current expression3 unary prec LL state: ");
	struct expr_unary_precederll *temp = currentexpression3unaryprecederhead;
	while(temp != NULL){
		switch(temp->data->precedertype){
			case UNARY_NEG: if(ASTVERBOSE())printf(" UNARY_NEG "); break;
			case UNARY_POS: if(ASTVERBOSE())printf(" UNARY POS "); break;
		}
		temp = temp->next;
	}
	if(ASTVERBOSE())printf("\n");
}
void ast_add_expr3_unprecdiscrsucc(){
	if(ASTVERBOSE())printf(":AST: Combining for expression 3 using format UNPREC DISCR SUCC  ");
	struct expr_expression3ll *temp = malloc(sizeof(struct expr_expression3ll));
	temp->next = currentexpression3head;
	currentexpression3head = temp;
	temp->data = malloc(sizeof(struct expr_expression3));
	temp->data->type = UNPREC_DISCR_SUCC;
	temp->data->unprec = currentexpression3unaryprecederhead==NULL?NULL:currentexpression3unaryprecederhead->data;
	if(currentexpression3unaryprecederhead)
		currentexpression3unaryprecederhead = currentexpression3unaryprecederhead->next;
	temp->data->succ = currentexpression3successorhead==NULL?NULL:currentexpression3successorhead->data;
	if(currentexpression3successorhead)
		currentexpression3successorhead = currentexpression3successorhead->next;
	temp->data->disc_term = currentexpression3discretermhead->data;
	if(currentexpression3discretermhead)
		currentexpression3discretermhead = currentexpression3discretermhead->next;
	if(ASTVERBOSE())printf("{");
	if(temp->data->unprec != NULL){
		switch(temp->data->unprec->precedertype){
			case UNARY_NEG: if(ASTVERBOSE())printf("UNARY_NEG|"); break;
			case UNARY_POS: if(ASTVERBOSE())printf("UNARY POS|"); break;
		}
	}
	else{
		if(ASTVERBOSE())printf("(null)|");
	}
	switch (temp->data->disc_term->type) {
		case DISCRETE_STRING: if(ASTVERBOSE())printf("string = %s|", temp->data->disc_term->termdata); break;
		case DISCRETE_VARIABLE: if(ASTVERBOSE())printf("variable = %s|", temp->data->disc_term->termdata); break;
		//case DISCRETE_FUNCTIONCALL: if(ASTVERBOSE())printf("string = %s|", temp->data->disc_term->termdata); break;
		//case DISCRETE_SHELLECHO: if(ASTVERBOSE())printf("string = %s|", temp->data->disc_term->termdata); break;
	}
	printf("|(null)}\n");
	//ast_display_expr3_discrete_termll_status();
	//ast_display_expr3_unary_precll_status();
	ast_display_exprll_status();
}
void ast_display_exprll_status(){
	if(ASTVERBOSE())printf(":AST: Current expression LL state: ");
	int cnt = 0;
	struct expr_expression1ll *temp1 = currentexpression1head;
	struct expr_expression2ll *temp2 = currentexpression2head;
	struct expr_expression3ll *temp3 = currentexpression3head;
	while(temp1 != NULL){
		cnt++;
		temp1 = temp1->next;
	}
	if(ASTVERBOSE())printf("[expr1 %d] ", cnt);
	cnt = 0;
	while(temp2 != NULL){
		cnt++;
		temp2 = temp2->next;
	}
	if(ASTVERBOSE())printf("[expr2 %d] ", cnt);
	cnt =0;
	while(temp3 != NULL){
		cnt++;
		temp3 = temp3->next;
	}
	if(ASTVERBOSE())printf("[expr3 %d] \n", cnt);
}
void ast_add_expr_expr2(){
	if(ASTVERBOSE())printf(":AST: expression2 shifted to expression1\n");
	struct expr_expression1ll *temp = malloc(sizeof(struct expr_expression1ll));
	temp->next = currentexpression1head;
	currentexpression1head = temp;
	temp->data = malloc(sizeof(struct expr_expression));
	temp->data->op = OP_NONE_EXPR1;
	temp->data->expression1 = NULL;
	temp->data->expression2 = currentexpression2head->data;
	currentexpression2head = currentexpression2head->next;
	ast_display_exprll_status();
}
void ast_add_expr2_expr3(){
	if(ASTVERBOSE())printf(":AST: expression3 shifted to expression2\n");
	struct expr_expression2ll *temp = malloc(sizeof(struct expr_expression2ll));
	temp->next = currentexpression2head;
	currentexpression2head = temp;
	temp->data = malloc(sizeof(struct expr_expression2));
	temp->data->op = OP_NONE_EXPR2;
	temp->data->expression2 = NULL;
	temp->data->expression3 = currentexpression3head->data;
	currentexpression3head = currentexpression3head->next;

	ast_display_exprll_status();
}
void ast_add_expr_op_expr2(enum expr_expression1_operation op){
	if(ASTVERBOSE())printf(":AST: expression2 shifted to expression1 with op ");
	switch(op){
		case OP_ADD: if(ASTVERBOSE())printf("OP_ADD\n"); break;
		case OP_SUB: if(ASTVERBOSE())printf("OP_SUB\n"); break;
	}
	struct expr_expression *temp = malloc(sizeof(struct expr_expression));
	temp->op = op;
	temp->expression1 = currentexpression1head->data;
	currentexpression1head->data = temp;
	temp->expression2 = currentexpression2head->data;
	currentexpression2head = currentexpression2head->next;
	ast_display_exprll_status();
}
void ast_add_expr2_op_expr3(enum expr_expression2_operation op){
	if(ASTVERBOSE())printf(":AST: expression3 shifted to expression2 with op ");
	switch(op){
		case OP_MUL: if(ASTVERBOSE())printf("OP_MUL\n"); break;
		case OP_DIV: if(ASTVERBOSE())printf("OP_DIV\n"); break;
		case OP_TRUNCDIV: if(ASTVERBOSE())printf("OP_TRUNCDIV\n"); break;
	}
	struct expr_expression2 *temp = malloc(sizeof(struct expr_expression2));
	temp->op = op;
	temp->expression2 = currentexpression2head->data;
	currentexpression2head->data = temp;
	temp->expression3 = currentexpression3head->data;
	currentexpression3head = currentexpression3head->next;
	ast_display_exprll_status();
}
void ast_add_expr3_unprecexprsucc(){
	if(ASTVERBOSE())printf(":AST: Combining for expression 3 using format UNPREC EXPR SUCC\n");
	struct expr_expression3ll *temp = malloc(sizeof(struct expr_expression3ll));
	temp->next = currentexpression3head;
	currentexpression3head = temp;
	temp->data = malloc(sizeof(struct expr_expression3));
	temp->data->disc_term = NULL;
	temp->data->expr = currentexpression1head->data;
	currentexpression1head = currentexpression1head->next;
	temp->data->unprec = currentexpression3unaryprecederhead->data;
	currentexpression3unaryprecederhead = currentexpression3unaryprecederhead->next;
	temp->data->succ = currentexpression3successorhead==NULL?NULL:currentexpression3successorhead->data;
	if(currentexpression3successorhead)
		currentexpression3successorhead = currentexpression3successorhead->next;
	temp->data->type = UNPREC_EXPR_SUCC;
	ast_display_exprll_status();
}
void ast_add_varassignmenttype(char *varname, enum var_assignmenttype type){
	if(ASTVERBOSE())printf(":AST: Variable assignment type for {%s} : ", varname);
	switch (type) {
		case ASSIGN_PREINCR: if(ASTVERBOSE())printf(" PRE-INCREMENT\n"); break;
		case ASSIGN_POSTINCR: if(ASTVERBOSE())printf(" POST-INCREMENT\n"); break;
		case ASSIGN_PREDECR: if(ASTVERBOSE())printf(" PRE-DECREMENT\n"); break;
		case ASSIGN_POSTDECR: if(ASTVERBOSE())printf(" POST-DECREMENT\n"); break;
	}
	struct var_assignments *temp = malloc(sizeof(struct var_assignments));
	currentvarassignmentshead = temp;
	temp->type = type;
	temp->varname = malloc(sizeof(char)*(strlen(varname)+1));
	strcpy(temp->varname, varname);
	temp->expression = NULL;
	temp->keyvalpairs = NULL;
	ast_display_exprll_status();
}
void ast_add_varassignment_keyvalpairs(char *varname){
	if(ASTVERBOSE())printf(":AST: Variable assignment type for {%s} : ASSIGN_KEYVALPAIRS\n", varname);
	struct var_assignments *temp = malloc(sizeof(struct var_assignments));
	currentvarassignmentshead = temp;
	temp->keyvalpairs = currentkeyvalpairshead;
	currentkeyvalpairshead = NULL;
	temp->type = ASSIGN_KEYVALPAIRS;
	temp->expression = NULL;
	temp->varname = malloc(sizeof(char)*(strlen(varname)+1));
	strcpy(temp->varname, varname);
}
void ast_add_varassignment_expr(char *varname){
	if(ASTVERBOSE())printf(":AST: Variable assignment type for {%s} : ASSIGN_EXPRESSION\n", varname);
	struct var_assignments *temp = malloc(sizeof(struct var_assignments));
	currentvarassignmentshead = temp;
	temp->type = ASSIGN_EXPRESSION;
	temp->expression = currentexpression1head->data;
	currentexpression1head = currentexpression1head->next;
	temp->varname = malloc(sizeof(char)*(strlen(varname)+1));
	strcpy(temp->varname, varname);
	ast_display_exprll_status();
}
void ast_add_seq_varassignment(){
	struct ast_sequential_varassignment *temp = malloc(sizeof(struct ast_sequential_varassignment));
	temp->assignments = currentvarassignmentshead;
	currentvarassignmentshead = NULL;
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_ASSIGNMENTS;
	currentsequentialhead->child.assignments = temp;
	if(ASTVERBOSE())printf(":AST: Added variable assignment statement\n");
}
