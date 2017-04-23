/* HLang Interpreter execution subsystem
 * created by Supragya Raj
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "variable_mgmt.h"
#include "scope_mgmt.h"
#include "ast.h"
#include "verbose.h"
#include "executor.h"

int flagerror = 0;

int exec_begin_execution(){
	struct ast_construct *mainentry = exec_provide_constructlist_for_function("main");
	if(flagerror){
		return 1;
	}
	return exec_constructs(mainentry);
	return 0;
}

int exec_constructs(struct ast_construct *entry){
	int retval = 0;
	while(entry != NULL){
		switch (entry->ctype) {
			case SEQUENTIAL: if(EXECVERBOSE())printf(":EXEC: Undertaking a sequential construct execution\n");
					retval = exec_sequential_construct(entry->ptr.sequential); break;
			case SELECTIVE: if(EXECVERBOSE())printf(":EXEC: Undertaking a selective construct execution\n");
					retval = exec_selective_construct(entry->ptr.selective);  break;
			case ITERATIVE: if(EXECVERBOSE())printf(":EXEC: Undertaking an iterative construct execution\n");
					retval = exec_iterative_construct(entry->ptr.iterative); break;
		}
		if(retval == 0){
			if(EXECVERBOSE())printf(":EXEC: Construct execution returned with code %d. All okay\n", retval);
		}
		else{
			if(EXECVERBOSE())printf(":EXEC: Construct execution returned with code %d. Something went wrong\n", retval);
			return retval;
		}
		entry = entry->next;
	}
	return 0;
}

struct ast_construct* exec_provide_constructlist_for_function(char *functionname){
	struct ast_function_node *function = rootnode->functions;
	flagerror = 1;
	if(EXECVERBOSE())printf(":EXEC: Searching constructlist for function {%s}\n", functionname);
	while(function != NULL && strcmp(functionname, function->functionname))
		function = function->next;

	if(function == NULL){
		if(EXECVERBOSE())printf(":EXEC: Cannot find function {%s}. Aborting execution\n", functionname);
		return NULL;
	}
	else if(!strcmp(function->functionname, functionname)){
		flagerror = 0;
		if(EXECVERBOSE())printf(":EXEC: Found {%s}\n", function->functionname);
	}
	return function->executionlist;
}

int exec_sequential_construct(struct ast_sequentialnode *seq){
	switch(seq->childtype){
		case AST_GENVARDECL: 	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_GENVARDECL\n");
					return exec_genvardecl(seq->child.genvardecl);
					break;
		case AST_MAPVARDECL:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_MAPVARDECL\n");
					return exec_mapvardecl(seq->child.mapvardecl);
					break;
		case AST_ASSIGNMENTS:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_ASSIGNMENTS\n");
					break;
		case AST_RETURN:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_RETURN\n");
					break;
		case AST_SHELLECHO:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_SHELLECHO\n");
					break;
		case AST_FUNCTIONCALL:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_FUNCTIONCALL\n");
					break;
	}
	return 0;
}

int exec_selective_construct(struct ast_selectivenode *sel){
	return 0;
}

int exec_iterative_construct(struct ast_iterativenode *iter){
	return 0;
}

int exec_genvardecl(struct ast_sequential_genvardecl *node){
	if(EXECVERBOSE())printf(":EXEC: Declaring general variables in vms variable table\n");
	struct vardecl_assignmentlist *list = node->list;
	variable_ptr_t currentbinlocation = 0;
	while(list != NULL){
		currentbinlocation = vms_add_new_variable(list->varname, 0);
		if(currentbinlocation == TOTAL_SLOTS){
			if(EXECVERBOSE())printf(":EXEC: No more space in vms variable table. Aborting execution\n");
			return 1;
		}
		vms_assign_to_bin_location(currentbinlocation, list->value);
		list = list->next;
	}
	return 0;
}

int exec_mapvardecl(struct ast_sequential_mapvardecl *node){
	if(EXECVERBOSE())printf(":EXEC: Declaring map variables in vms map list\n");
	struct mapvarlist *list = node->mapvarlist;
	while(list != NULL){
		if(vms_add_new_map(list->mapname,0)){
			if(EXECVERBOSE())printf(":EXEC: Error declaring map variable. Aborting execution\n");
			return 1;
		}
		if(exec_add_keyval_pairs(list->mapname, list->keyvalpairs)){
			if(EXECVERBOSE())printf(":EXEC: Error assigning spaces for map keyval pairs. Aborting execution\n");
			return 1;
		}
		list = list->next;
	}
	return 0;
}

int exec_add_keyval_pairs(char *mapname, struct keyvalpairs *pairs){
	char *temp;
	int len = strlen(mapname);
	/* Prepare name */
	while(pairs != NULL){
		temp = malloc(sizeof(char)*(len + strlen(pairs->key) + 3));
		if(!temp){
			if(EXECVERBOSE())printf(":EXEC: Error allocating space for keyval pair {%s|%s}\n", pairs->key, pairs->value);
			return 1;
		}
		strcpy(temp, mapname);
		temp[len] = '[';
		strcpy(temp+len+1, pairs->key);
		temp[len+1+strlen(pairs->key)] = ']';
		temp[len+1+strlen(pairs->key)+1] = '\0';
		if(EXECVERBOSE())printf(":EXEC: Adding keyval pair {%s|%s}\n", temp, pairs->value);
		variable_ptr_t currentbinloc = vms_add_new_variable(temp, 0);
		if(currentbinloc == TOTAL_SLOTS){
			if(EXECVERBOSE())printf(":EXEC: Error allocating space for keyval pair {%s|%s}\n", pairs->key, pairs->value);
			return 1;
		}
		vms_assign_to_bin_location(currentbinloc, pairs->value);
		pairs = pairs->next;
	}
}
