/* HLang Abstract Syntax Tree
 * created by Supragya Raj
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ast.h"
#include "verbose.h"

struct ast_construct *currentconstructhead;
struct ast_sequentialnode *currentsequentialhead;
struct keyvalpairs *currentkeyvalpairshead;
struct mapvarlist *currentmapvarlisthead;
struct vardecl_assignmentlist *currentvardeclassignmentlisthead;
struct functioncallargs *currentfunccallargshead;
struct returnval *currentreturnvalhead;

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

void ast_add_seq(char *name){
	struct ast_sequentialnode *newseqnode;
	newseqnode = malloc(sizeof(struct ast_sequentialnode));
	newseqnode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newseqnode->name, name);
	//newseqnode->next.ctype = NONE;
	if(currentsequentialhead != NULL){
		newseqnode->childtype = currentsequentialhead->childtype;
		switch(newseqnode->childtype){
			case AST_GENVARDECL: newseqnode->name[0] = 'g'; if(ASTVERBOSE())printf(":AST: Registering sequential construct SEQ----GENVARDECL\n");newseqnode->child.genvardecl = currentsequentialhead->child.genvardecl; break;
			case AST_MAPVARDECL: newseqnode->name[0] = 'm'; if(ASTVERBOSE())printf(":AST: Registering sequential construct SEQ----MAPVARDECL\n");newseqnode->child.mapvardecl = currentsequentialhead->child.mapvardecl; break;
			case AST_ASSIGNMENTS: newseqnode->name[0] = 'a'; if(ASTVERBOSE())printf(":AST: Registering sequential construct SEQ----ASSIGNMENTS\n");newseqnode->child.assignments = currentsequentialhead->child.assignments; break;
			case AST_RETURN: newseqnode->name[0] = 'r'; if(ASTVERBOSE())printf(":AST: Registering sequential construct SEQ----RETURN\n");newseqnode->child._return = currentsequentialhead->child._return; break;
			case AST_SHELLECHO: newseqnode->name[0] = 's'; if(ASTVERBOSE())printf(":AST: Registering sequential construct SEQ----SHELLECHO\n");newseqnode->child.shellecho = currentsequentialhead->child.shellecho; break;
			case AST_FUNCTIONCALL: newseqnode->name[0] = 'f'; if(ASTVERBOSE())printf(":AST: Registering sequential construct SEQ----FUNCTIONCALL\n");newseqnode->child.functioncall = currentsequentialhead->child.functioncall; break;
		}
		free(currentsequentialhead);
		currentsequentialhead = NULL;
		// if(ASTVERBOSE())printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
		// if(ASTVERBOSE())printf(":AST: newseqnode made with name %s and value %s\n", newseqnode->name, newseqnode->child.shellecho->value);
	}
	if(ASTVERBOSE())printf(":AST: newseqnode with name %s\n", newseqnode->name);
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
			case SEQUENTIAL: printf("  [SEQUENTIAL]   ");break;
			case SELECTIVE: printf("   [SELECTIVE]   "); break;
			case ITERATIVE: printf("   [ITERATIVE]   "); break;
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

void ast_set_return_val_varname(char *varname){
	//TODO
	currentreturnvalhead = malloc(sizeof(struct returnval));
	currentreturnvalhead->type = R_VARNAME;
	currentreturnvalhead->ret_str = malloc(sizeof(char)*(strlen(varname)+1));
	strcpy(currentreturnvalhead->ret_str, varname);
	if(ASTVERBOSE())printf(":AST: Set the return value to varname %s\n", varname);
}

void ast_add_seq_return(){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_RETURN;
	currentsequentialhead->child._return = malloc(sizeof(struct ast_sequential_return));
	currentsequentialhead->child._return->retdata = currentreturnvalhead;
	currentreturnvalhead = NULL;
}
