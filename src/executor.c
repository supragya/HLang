/* HLang Interpreter execution subsystem
 * created by Supragya Raj
 */
#include <stdio.h>
#include <string.h>
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
					break;
		case AST_MAPVARDECL:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_MAPVARDECL\n");
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
