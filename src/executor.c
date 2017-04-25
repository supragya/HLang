/* HLang Interpreter execution subsystem
 * created by Supragya Raj
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "variable_mgmt.h"
#include "scope_mgmt.h"
#include "ast.h"
#include "verbose.h"
#include "executor.h"
#include "shell.h"

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
					return exec_varassignment(seq->child.assignments);
					break;
		case AST_RETURN:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_RETURN\n");
					break;
		case AST_SHELLECHO:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_SHELLECHO\n");
					return exec_shellecho(seq->child.shellecho);
					break;
		case AST_FUNCTIONCALL:	if(EXECVERBOSE())printf(":EXEC: Sequential execution AST_FUNCTIONCALL\n");
					break;
	}
	return 0;
}

int exec_selective_construct(struct ast_selectivenode *sel){
	int flag = 1;
	flag = exec_if(sel->ifblock);
	if(flag)
		flag = exec_elif(sel->elifblock);
	if(flag)
		flag = exec_else(sel->elseblock);
	return flag;
}
int exec_if(struct ast_ifsel *ifblock){
	if(EXECVERBOSE())printf(":EXEC: Trying to execute if statement\n");
	int truthvalue = solve_condition(ifblock->cond);
	if(truthvalue == 0){
		if(EXECVERBOSE())printf(":EXEC: If statement has a truth value of 0. Revert.\n");
		return 1;
	}
	else{
		if(EXECVERBOSE())printf(":EXEC: If statement has a truth value of 1. Continuing.\n");
		return exec_constructs(ifblock->constructs);
	}
}
int exec_elif(struct ast_elifsel *elifblock){
	int truthvalue, flag;
	while(elifblock != NULL){
		if(EXECVERBOSE())printf(":EXEC: Trying to execute elif statement\n");
		truthvalue = solve_condition(elifblock->cond);
		if(truthvalue == 0){
			if(EXECVERBOSE())printf(":EXEC: Elif statement has a truth value of 0. Revert.\n");
		}
		else{
			if(EXECVERBOSE())printf(":EXEC: Elif statement has a truth value of 1. Continuing.\n");
			return exec_constructs(elifblock->constructs);
		}
		elifblock = elifblock->next;
	}
	return 1;
}
int exec_else(struct ast_elsesel *elseblock){
	if(EXECVERBOSE())printf(":EXEC: Executing else statement\n");
	return exec_constructs(elseblock->constructs);
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
		if(list->type == VARDECL_VALUE)
			vms_assign_to_bin_location(currentbinlocation, list->value);
		else{
			struct exec_expr_solution sol = solve_expression(list->expr);
			if(sol.solvable == SOL_NO){
				if(EXECVERBOSE())printf(":EXEC: Assignment type is unsolvable arithmetically. Aborting execution\n");
				return 1;
			}
			vms_assign_to_bin_location(currentbinlocation, sol.solution);
		}
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
		if(list->keyvalpairs != NULL && exec_add_keyval_pairs(list->mapname, list->keyvalpairs)){
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
int exec_shellecho(struct ast_sequential_shellecho *node){
	return shellexecute(node->value);
}
int exec_varassignment(struct ast_sequential_varassignment *node){
	if(EXECVERBOSE())printf(":EXEC: Variable assignments\n");
	struct var_assignments *assignments = node->assignments;
	if(assignments->type != ASSIGN_KEYVALPAIRS && assignments->type != ASSIGN_EXPRESSION) return exec_varassignment_aux1(assignments);
	return 0;
}
int exec_varassignment_aux1(struct var_assignments *node){
	variable_ptr_t binlocation = vms_var_lookup(node->varname, 0);
	if(binlocation != TOTAL_SLOTS){
		if(EXECVERBOSE())printf(":EXEC: Variable for assignment has bin location: %ld\n", binlocation);
		char *value = vms_getvaluebylocation(binlocation);
		if(!strcmp(longtostring(atol(value)), value)){
			switch(node->type){
				case ASSIGN_POSTINCR:
				case ASSIGN_PREINCR: vms_assign_to_bin_location(binlocation, longtostring(atol(value)+1)); break;
				case ASSIGN_PREDECR:
				case ASSIGN_POSTDECR: vms_assign_to_bin_location(binlocation, longtostring(atol(value)-1)); break;
			}
			return 0;
		}
		else{
			if(ASTVERBOSE())printf(":EXEC: Found variable %s in variable table. However, it does not have integer values to deal with. Aborting execution.\n", node->varname);
		}

	}
	else{
		if(EXECVERBOSE())printf(":EXEC: Cannot find variable {%s} in the variable table. Aborting execution\n", node->varname);
		return 1;
	}
}
struct exec_expr_solution solve_expression(struct expr_expression *expr){
	struct exec_expr_solution unsolvable;
	unsolvable.solvable == SOL_NO;
	struct exec_expr_solution t1, t2;
	if(EXECVERBOSE())printf(":EXEC: Expression1 solver: ");
	switch(expr->op){
		case OP_NONE_EXPR1:	if(EXECVERBOSE())printf("OP_NONE_EXPR1\n");
					return solve_expression2(expr->expression2);
					break;
		case OP_ADD:		if(EXECVERBOSE())printf("OP_ADD\n");
					t1 = solve_expression(expr->expression1);
					t2 = solve_expression2(expr->expression2);
					if(t1.solvable == SOL_NO || t2.solvable == SOL_NO)
						return unsolvable;
					else
						return expr_addterms(t1, t2);
		case OP_SUB:		if(EXECVERBOSE())printf("OP_SUB\n");
					t1 = solve_expression(expr->expression1);
					t2 = solve_expression2(expr->expression2);
					if(t1.solvable == SOL_NO || t2.solvable == SOL_NO)
						return unsolvable;
					else
						return expr_subterms(t1, t2);
	}
}
struct exec_expr_solution solve_expression2(struct expr_expression2 *expr2){
	struct exec_expr_solution unsolvable;
	unsolvable.solvable == SOL_NO;
	struct exec_expr_solution t1, t2;
	if(EXECVERBOSE())printf(":EXEC: Expression2 solver: ");
	switch(expr2->op){
		case OP_NONE_EXPR2:	if(EXECVERBOSE())printf("OP_NONE_EXPR2\n");
					return solve_expression3(expr2->expression3);
					break;
		case OP_MUL:		if(EXECVERBOSE())printf("OP_ADD\n");
					t1 = solve_expression2(expr2->expression2);
					t2 = solve_expression3(expr2->expression3);
					if(t1.solvable == SOL_NO || t2.solvable == SOL_NO)
						return unsolvable;
					else
						return expr_multerms(t1, t2);
		case OP_DIV:		if(EXECVERBOSE())printf("OP_SUB\n");
					t1 = solve_expression2(expr2->expression2);
					t2 = solve_expression3(expr2->expression3);
					if(t1.solvable == SOL_NO || t2.solvable == SOL_NO)
						return unsolvable;
					else
						return expr_divterms(t1, t2);
		case OP_TRUNCDIV:	if(EXECVERBOSE())printf("OP_SUB\n");
					t1 = solve_expression2(expr2->expression2);
					t2 = solve_expression3(expr2->expression3);
					if(t1.solvable == SOL_NO || t2.solvable == SOL_NO)
						return unsolvable;
					else
						return expr_truncdivterms(t1, t2);
	}
}
struct exec_expr_solution solve_expression3(struct expr_expression3 *expr3){
	struct exec_expr_solution unsolvable;
	unsolvable.solvable == SOL_NO;
	if(EXECVERBOSE())printf(":EXEC: Expression3 solver: ");
	switch(expr3->type){
		case UNPREC_EXPR_SUCC:	if(EXECVERBOSE())printf("UNPREC_EXPR_SUCC\n");
					return expr_solve_unprec_expr_succ(expr3);
					break;

		case UNPREC_DISCR_SUCC:	if(EXECVERBOSE())printf("UNPREC_DISCR_SUCC\n");
					return expr_solve_unprec_discr_succ(expr3);
					break;
	}
}
struct exec_expr_solution expr_addterms(struct exec_expr_solution t1, struct exec_expr_solution t2){
	struct exec_expr_solution ret;
	ret.solvable = SOL_YES;
	long longans = atol(t1.solution) + atol(t2.solution);
	ret.solution = longtostring(longans);
	if(ASTVERBOSE())printf(":EXEC: ADD terms %s and %s to make %s\n", t1.solution, t2.solution, ret.solution);
	return ret;
}
struct exec_expr_solution expr_subterms(struct exec_expr_solution t1, struct exec_expr_solution t2){
	struct exec_expr_solution ret;
	ret.solvable = SOL_YES;
	long longans = atol(t1.solution) - atol(t2.solution);
	ret.solution = longtostring(longans);
	if(ASTVERBOSE())printf(":EXEC: SUB terms %s and %s to make %s\n", t1.solution, t2.solution, ret.solution);
	return ret;
}
struct exec_expr_solution expr_multerms(struct exec_expr_solution t1, struct exec_expr_solution t2){
	struct exec_expr_solution ret;
	ret.solvable = SOL_YES;
	long longans = atol(t1.solution) * atol(t2.solution);
	ret.solution = longtostring(longans);
	if(ASTVERBOSE())printf(":EXEC: MUL terms %s and %s to make %s\n", t1.solution, t2.solution, ret.solution);
	return ret;
}
struct exec_expr_solution expr_divterms(struct exec_expr_solution t1, struct exec_expr_solution t2){
	struct exec_expr_solution ret;
	ret.solvable = SOL_YES;
	long longans = atol(t1.solution) / atol(t2.solution);
	ret.solution = longtostring(longans);
	if(ASTVERBOSE())printf(":EXEC: DIV terms %s and %s to make %s\n", t1.solution, t2.solution, ret.solution);
	return ret;
}
struct exec_expr_solution expr_truncdivterms(struct exec_expr_solution t1, struct exec_expr_solution t2){
	struct exec_expr_solution ret;
	ret.solvable = SOL_YES;
	long longans = atol(t1.solution) + atol(t2.solution);
	ret.solution = longtostring(longans);
	if(ASTVERBOSE())printf(":EXEC: TRUNCDIV terms %s and %s to make %s\n", t1.solution, t2.solution, ret.solution);
	return ret;
}
struct exec_expr_solution expr_solve_unprec_expr_succ(struct expr_expression3 *expr3){
	struct exec_expr_solution expr = solve_expression(expr3->expr);
	// ADD succ here
	if(expr.solvable == SOL_NO){
		if(ASTVERBOSE())printf(":EXEC: Unsolvable expression at UNPREC EXPR SUCC\n");
	}
	else{
		if(ASTVERBOSE())printf(":EXEC: Solving expression at UNPREC EXPR SUCC ");
		if(expr3->unprec->precedertype == UNARY_NEG) {
			char *temp = malloc(sizeof(char)*(strlen(expr.solution)+2));
			strcpy(temp+1, expr.solution);
			free(expr.solution);
			expr.solution = temp;
			if(ASTVERBOSE())printf("with UNARY_NEG");
		}
		if(ASTVERBOSE())printf("\n");
	}
	return expr;
}
struct exec_expr_solution expr_solve_unprec_discr_succ(struct expr_expression3 *expr3){
	struct exec_expr_solution expr;
	variable_ptr_t binlocation;
	switch(expr3->disc_term->type){
		case DISCRETE_STRING: 	if(!strcmp(longtostring(atol(expr3->disc_term->termdata)),expr3->disc_term->termdata)){
						if(ASTVERBOSE())printf(":EXEC: Solving expression at UNPREC DISCR SUCC with DISCRETE_STRING, solvable\n");
						expr.solvable = SOL_YES;
						expr.solution = longtostring(atol(expr3->disc_term->termdata));
					}
					else{
						if(ASTVERBOSE())printf(":EXEC: Unsolvable expression at UNPREC DISCR SUCC with DISCRETE_STRING\n");
						expr.solvable = SOL_NO;
						expr.solution = NULL;
					}
					break;
		case DISCRETE_VARIABLE: binlocation= vms_var_lookup(expr3->disc_term->termdata, 0);
					if(binlocation != TOTAL_SLOTS){
						if(ASTVERBOSE())printf(":EXEC: Solving expression at UNPREC DISCR SUCC with DISCRETE_VARIABLE, solvable\n");
						expr.solvable = SOL_YES;
						expr.solution = vms_getvaluebylocation(binlocation);
					}
					else{
						if(ASTVERBOSE())printf(":EXEC: Unsolvable expression at UNPREC DISCR SUCC with DISCRETE_VARIABLE: %s\n", expr3->disc_term->termdata);
						expr.solvable = SOL_NO;
						expr.solution = NULL;
					}
					break;
		case DISCRETE_FUNCTIONCALL: if(ASTVERBOSE())printf(":AST: Expression calls function for value. Assigning 0 for time being\n");
						expr.solvable = SOL_YES;
						expr.solution = malloc(sizeof(char)*2);
						strcpy(expr.solution, "0");
						break;
		case DISCRETE_SHELLECHO: if(ASTVERBOSE())printf(":AST: Expression calls shellecho for value.\n");
						expr.solvable = SOL_YES;
						expr.solution = malloc(sizeof(char)*2);
						int retshell = shellexecute(expr3->disc_term->shellecho->value);
						retshell?strcpy(expr.solution, "1"):strcpy(expr.solution, "0");
						break;
	}
	if(expr3->unprec->precedertype == UNARY_NEG){
		if(ASTVERBOSE())printf(":AST: Negating usin UNARY_NEG\n");
	}
	return expr;
}
char *longtostring(long val){
	int len = 0;
	long tempval = val;
	while(tempval!=0){
		tempval /= 10;
		len++;
	}
	char *retstr = malloc(sizeof(char)*(len+1));
	retstr[len] = '\0';
	len--;
	while(len >= 0){
		retstr[len] = (char)((char)'0' + val%10);
		len--;
		val /= 10;
	}
	return retstr;
}
int solve_condition(struct condition1 *cond){
	int ret = 0;
	switch(cond->type){
		case COND1_NONE: if(ASTVERBOSE())printf(":EXEC: Condition1 solving with COND1_NONE\n"); ret = solve_condition2(cond->cond2); break;
		case COND1_LOR: if(ASTVERBOSE())printf(":EXEC: Condition1 solving with COND2_LOR\n"); ret = (solve_condition(cond->cond1) || solve_condition2(cond->cond2)); break;
	}
	if(ASTVERBOSE())printf(":EXEC: Condition1 returns: %d\n", ret);
	return ret;
}
int solve_condition2(struct condition2 *cond){
	int ret = 0;
	switch(cond->type){
		case COND2_NONE: if(ASTVERBOSE())printf(":EXEC: Condition2 solving with COND2_NONE\n"); ret = solve_condition3(cond->cond3); break;
		case COND2_LAND: if(ASTVERBOSE())printf(":EXEC: Condition2 solving with COND2_LAND\n"); ret = (solve_condition2(cond->cond2) && solve_condition3(cond->cond3)); break;
	}
	if(ASTVERBOSE())printf(":EXEC: Condition2 returns: %d\n", ret);
	return ret;
}
int solve_condition3(struct condition3 *cond){
	int ret = 0;
	switch(cond->type){
		case COND3_COND:if(ASTVERBOSE())printf(":EXEC: Condition3 solving with COND3_COND\n"); ret = solve_condition(cond->cond1); break;
		case COND3_COMP:if(ASTVERBOSE())printf(":EXEC: Condition3 solving with COND3_COMP\n"); ret  = exec_for_condition(cond->component1); break;
		case COND3_COMP_REL_COMP: if(ASTVERBOSE())printf(":EXEC: Condition3 solving with COND3_COMP_REL_COMP\n"); ret = exec_for_condition_comprelcomp(cond->component1, cond->rel, cond->component2); break;
	}
	if (cond->neg == NEG_YES){
		if(ASTVERBOSE())printf(":EXEC: Condition3 found NEG, changing %d to %d\n", ret, !ret);
		return !ret;
	}
	if(ASTVERBOSE())printf(":EXEC: Condition3 returns: %d\n", ret);
	return ret;
}
int exec_for_condition(struct conditioncomponent *comp){
	switch (comp->type) {
		case COMP_STR: return strcmp(comp->name, "0"); break;
		case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: Condition component needs variable. Looking up in vms\n");
				   variable_ptr_t binlocation = vms_var_lookup(comp->name, 0);
				   if(binlocation == TOTAL_SLOTS){
					if(ASTVERBOSE())printf(":EXEC: Looked up variable %s in vms, not found. Default returning 0\n",comp->name);
					return 0;
				   }
				   else{
					if(ASTVERBOSE())printf("Found in vms the variable %s, truth value is %d\n", comp->name, strcmp(vms_getvaluebylocation(binlocation), "0"));
				   	return strcmp(vms_getvaluebylocation(binlocation), "0");
				   }
				break;
		case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: Condition component needs shellecho. Sending to shell\n");
				return shellexecute(comp->name); break;
		case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: Condition component needs functioncall. Not implemented, meanwhile 0\n");
				return 0;
				break;
	}
}
int exec_for_condition_comprelcomp(struct conditioncomponent *component1, enum relopr rel, struct conditioncomponent *component2){
	int shellresult;
	char *variableval;
	variable_ptr_t binlocation;
	switch(rel){
		case REL_EQ:	switch(component1->type){
					case COMP_FUNC:
					switch(component2->type){
						default: if(ASTVERBOSE())printf(":EXEC: types COMP_FUNC EQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
					}
					break;

					case COMP_SHELLECHO:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(" :EXEC: types COMP_SHELLECHO EQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO EQ COMP_SHELLECHO\n");
								return (shellexecute(component1->name) == shellexecute(component2->name));
								break;
						case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO EQ COMP_VARNAME\n");
								shellresult = shellexecute(component1->name);
								binlocation = vms_var_lookup(component2->name, 0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return !strcmp(longtostring(shellresult),vms_getvaluebylocation(binlocation));
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO EQ COMP_STR\n");
								shellresult = shellexecute(component1->name);
								return !strcmp(longtostring(shellresult), component2->name);
								break;
					}
					break;

					case COMP_VARNAME:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME EQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME EQ COMP_SHELLECHO\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return !strcmp(vms_getvaluebylocation(binlocation), longtostring(shellexecute(component2->name)));
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME EQ COMP_VARNAME\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								variableval = vms_getvaluebylocation(binlocation);
								binlocation = vms_var_lookup(component2->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return !strcmp(variableval, vms_getvaluebylocation(binlocation));
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME EQ COMP_STR\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return !strcmp(component2->name, vms_getvaluebylocation(binlocation));
								break;
					}
					break;

					case COMP_STR:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_STR EQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_STR EQ COMP_SHELLECHO\n");
								return !strcmp(component1->name, longtostring(shellexecute(component2->name)));
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_STR EQ COMP_VARNAME\n");
								binlocation = vms_var_lookup(component2->name, 0);
								return !strcmp(component1->name, vms_getvaluebylocation(binlocation));
								break;
						case COMP_STR:	if(ASTVERBOSE())printf(":EXEC: types COMP_STR EQ COMP_STR\n");
								return !strcmp(component1->name, component2->name);
								break;
					}
					break;
				}
				break;


		case REL_NQ:	switch(component1->type){
					case COMP_FUNC:
					switch(component2->type){
						default: if(ASTVERBOSE())printf(":EXEC: types COMP_FUNC NQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
					}
					break;

					case COMP_SHELLECHO:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(" :EXEC: types COMP_SHELLECHO NQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO NQ COMP_SHELLECHO\n");
								return (shellexecute(component1->name) != shellexecute(component2->name));
								break;
						case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO NQ COMP_VARNAME\n");
								shellresult = shellexecute(component1->name);
								binlocation = vms_var_lookup(component2->name, 0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(longtostring(shellresult),vms_getvaluebylocation(binlocation));
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO NQ COMP_STR\n");
								shellresult = shellexecute(component1->name);
								return strcmp(longtostring(shellresult), component2->name);
								break;
					}
					break;

					case COMP_VARNAME:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME NQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME NQ COMP_SHELLECHO\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(vms_getvaluebylocation(binlocation), longtostring(shellexecute(component2->name)));
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME NQ COMP_VARNAME\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								variableval = vms_getvaluebylocation(binlocation);
								binlocation = vms_var_lookup(component2->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(variableval, vms_getvaluebylocation(binlocation));
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME NQ COMP_STR\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(component2->name, vms_getvaluebylocation(binlocation));
								break;
					}
					break;

					case COMP_STR:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_STR NQ COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_STR NQ COMP_SHELLECHO\n");
								return strcmp(component1->name, longtostring(shellexecute(component2->name)));
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_STR NQ COMP_VARNAME\n");
								binlocation = vms_var_lookup(component2->name, 0);
								return strcmp(component1->name, vms_getvaluebylocation(binlocation));
								break;
						case COMP_STR:	if(ASTVERBOSE())printf(":EXEC: types COMP_STR NQ COMP_STR\n");
								return strcmp(component1->name, component2->name);
								break;
					}
					break;
				}
				break;


		case REL_GT:	switch(component1->type){
					case COMP_FUNC:
					switch(component2->type){
						default: if(ASTVERBOSE())printf(":EXEC: types COMP_FUNC GT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
					}
					break;

					case COMP_SHELLECHO:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(" :EXEC: types COMP_SHELLECHO GT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO GT COMP_SHELLECHO\n");
								return (shellexecute(component1->name) > shellexecute(component2->name));
								break;
						case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO GT COMP_VARNAME\n");
								shellresult = shellexecute(component1->name);
								binlocation = vms_var_lookup(component2->name, 0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(longtostring(shellresult),vms_getvaluebylocation(binlocation)) > 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO GT COMP_STR\n");
								shellresult = shellexecute(component1->name);
								return strcmp(longtostring(shellresult), component2->name) > 0;
								break;
					}
					break;

					case COMP_VARNAME:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GT COMP_SHELLECHO\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(vms_getvaluebylocation(binlocation), longtostring(shellexecute(component2->name))) > 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GT COMP_VARNAME\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								variableval = vms_getvaluebylocation(binlocation);
								binlocation = vms_var_lookup(component2->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(variableval, vms_getvaluebylocation(binlocation)) > 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GT COMP_STR\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(component2->name, vms_getvaluebylocation(binlocation)) > 0;
								break;
					}
					break;

					case COMP_STR:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_STR GT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_STR GT COMP_SHELLECHO\n");
								return strcmp(component1->name, longtostring(shellexecute(component2->name))) > 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_STR GT COMP_VARNAME\n");
								binlocation = vms_var_lookup(component2->name, 0);
								return strcmp(component1->name, vms_getvaluebylocation(binlocation)) > 0;
								break;
						case COMP_STR:	if(ASTVERBOSE())printf(":EXEC: types COMP_STR GT COMP_STR\n");
								return strcmp(component1->name, component2->name) > 0;
								break;
					}
					break;
				}
				break;


		case REL_LT:		switch(component1->type){
					case COMP_FUNC:
					switch(component2->type){
						default: if(ASTVERBOSE())printf(":EXEC: types COMP_FUNC LT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
					}
					break;

					case COMP_SHELLECHO:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(" :EXEC: types COMP_SHELLECHO LT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO LT COMP_SHELLECHO\n");
								return (shellexecute(component1->name) < shellexecute(component2->name));
								break;
						case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO LT COMP_VARNAME\n");
								shellresult = shellexecute(component1->name);
								binlocation = vms_var_lookup(component2->name, 0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(longtostring(shellresult),vms_getvaluebylocation(binlocation)) < 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO LT COMP_STR\n");
								shellresult = shellexecute(component1->name);
								return strcmp(longtostring(shellresult), component2->name) < 0;
								break;
					}
					break;

					case COMP_VARNAME:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LT COMP_SHELLECHO\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(vms_getvaluebylocation(binlocation), longtostring(shellexecute(component2->name))) < 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LT COMP_VARNAME\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								variableval = vms_getvaluebylocation(binlocation);
								binlocation = vms_var_lookup(component2->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(variableval, vms_getvaluebylocation(binlocation)) < 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LT COMP_STR\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(component2->name, vms_getvaluebylocation(binlocation)) < 0;
								break;
					}
					break;

					case COMP_STR:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_STR LT COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_STR LT COMP_SHELLECHO\n");
								return strcmp(component1->name, longtostring(shellexecute(component2->name))) < 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_STR LT COMP_VARNAME\n");
								binlocation = vms_var_lookup(component2->name, 0);
								return strcmp(component1->name, vms_getvaluebylocation(binlocation)) < 0;
								break;
						case COMP_STR:	if(ASTVERBOSE())printf(":EXEC: types COMP_STR LT COMP_STR\n");
								return strcmp(component1->name, component2->name) < 0;
								break;
					}
					break;
				}
				break;



		case REL_GE:		switch(component1->type){
					case COMP_FUNC:
					switch(component2->type){
						default: if(ASTVERBOSE())printf(":EXEC: types COMP_FUNC GE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
					}
					break;

					case COMP_SHELLECHO:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(" :EXEC: types COMP_SHELLECHO GE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO GE COMP_SHELLECHO\n");
								return (shellexecute(component1->name) >= shellexecute(component2->name));
								break;
						case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO GE COMP_VARNAME\n");
								shellresult = shellexecute(component1->name);
								binlocation = vms_var_lookup(component2->name, 0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(longtostring(shellresult),vms_getvaluebylocation(binlocation)) >= 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO GE COMP_STR\n");
								shellresult = shellexecute(component1->name);
								return strcmp(longtostring(shellresult), component2->name) >= 0;
								break;
					}
					break;

					case COMP_VARNAME:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GE COMP_SHELLECHO\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(vms_getvaluebylocation(binlocation), longtostring(shellexecute(component2->name))) >= 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GE COMP_VARNAME\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								variableval = vms_getvaluebylocation(binlocation);
								binlocation = vms_var_lookup(component2->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(variableval, vms_getvaluebylocation(binlocation)) >= 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME GE COMP_STR\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(component2->name, vms_getvaluebylocation(binlocation)) >= 0;
								break;
					}
					break;

					case COMP_STR:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_STR GE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_STR GE COMP_SHELLECHO\n");
								return strcmp(component1->name, longtostring(shellexecute(component2->name))) >= 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_STR GE COMP_VARNAME\n");
								binlocation = vms_var_lookup(component2->name, 0);
								return strcmp(component1->name, vms_getvaluebylocation(binlocation)) >= 0;
								break;
						case COMP_STR:	if(ASTVERBOSE())printf(":EXEC: types COMP_STR GE COMP_STR\n");
								return strcmp(component1->name, component2->name) >= 0;
								break;
					}
					break;
				}
				break;

		case REL_LE:	switch(component1->type){
					case COMP_FUNC:
					switch(component2->type){
						default: if(ASTVERBOSE())printf(":EXEC: types COMP_FUNC LE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
					}
					break;

					case COMP_SHELLECHO:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(" :EXEC: types COMP_SHELLECHO LE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO LE COMP_SHELLECHO\n");
								return (shellexecute(component1->name) <= shellexecute(component2->name));
								break;
						case COMP_VARNAME: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO LE COMP_VARNAME\n");
								shellresult = shellexecute(component1->name);
								binlocation = vms_var_lookup(component2->name, 0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(longtostring(shellresult),vms_getvaluebylocation(binlocation)) <= 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_SHELLECHO LE COMP_STR\n");
								shellresult = shellexecute(component1->name);
								return strcmp(longtostring(shellresult), component2->name) <= 0;
								break;
					}
					break;

					case COMP_VARNAME:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LE COMP_SHELLECHO\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(vms_getvaluebylocation(binlocation), longtostring(shellexecute(component2->name))) <= 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LE COMP_VARNAME\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								variableval = vms_getvaluebylocation(binlocation);
								binlocation = vms_var_lookup(component2->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(variableval, vms_getvaluebylocation(binlocation)) <= 0;
								break;
						case COMP_STR: if(ASTVERBOSE())printf(":EXEC: types COMP_VARNAME LE COMP_STR\n");
								binlocation = vms_var_lookup(component1->name,0);
								if(binlocation == TOTAL_SLOTS)
									return 0;
								return strcmp(component2->name, vms_getvaluebylocation(binlocation)) <= 0;
								break;
					}
					break;

					case COMP_STR:
					switch(component2->type){
						case COMP_FUNC: if(ASTVERBOSE())printf(":EXEC: types COMP_STR LE COMP_FUNC\n");
								//TODO Functioncall here
								return 0;
								break;
						case COMP_SHELLECHO:if(ASTVERBOSE())printf(":EXEC: types COMP_STR LE COMP_SHELLECHO\n");
								return strcmp(component1->name, longtostring(shellexecute(component2->name))) <= 0;
								break;
						case COMP_VARNAME:if(ASTVERBOSE())printf(":EXEC: types COMP_STR LE COMP_VARNAME\n");
								binlocation = vms_var_lookup(component2->name, 0);
								return strcmp(component1->name, vms_getvaluebylocation(binlocation)) <= 0;
								break;
						case COMP_STR:	if(ASTVERBOSE())printf(":EXEC: types COMP_STR LE COMP_STR\n");
								return strcmp(component1->name, component2->name) <= 0;
								break;
					}
					break;
				}
				break;
	}
}
