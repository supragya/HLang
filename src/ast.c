/* HLang Abstract Syntax Tree
 * created by Supragya Raj
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ast.h"
struct ast_root_node *rootnode;
struct ast_construct *currentconstructhead;
struct ast_sequentialnode *currentsequentialhead;
struct keyvalpairs *currentkeyvalpairshead;

int ast_init(){
	rootnode = malloc(sizeof(struct ast_root_node));
	currentconstructhead = malloc(sizeof(struct ast_construct));
	if(!rootnode)
		return 1;
	rootnode->functions = NULL;
	currentconstructhead->ctype = NONE;
	currentconstructhead->ptr.selective = NULL;
	currentsequentialhead = NULL;
	currentkeyvalpairshead = NULL;
	printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	return 0;
}

void ast_add_function(char *functionname){
	printf(":AST: Adding functionname: %s\n", functionname);
	struct ast_function_node *temp_function = malloc(sizeof(struct ast_function_node));
	struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));

	temp_function->executionlist = malloc(sizeof(struct ast_construct));
	if(!temp_function || !temp_function->executionlist)
		printf(":AST: [Error] Cannot allocate memory for function node (%s)\n", functionname);
	temp_function->functionname = malloc(sizeof(char)*(strlen(functionname)+1));

	strcpy(temp_function->functionname, functionname);
	temp_function->next = rootnode->functions;
	rootnode->functions = temp_function;

	// printf("dbg1\n");
	temp_function->executionlist->ctype = currentconstructhead->ctype;
	temp_function->executionlist->ptr = currentconstructhead->ptr;
	// printf("dbg2\n" );
	// printf("current execlist starts at %d\n", temp_function->executionlist->ctype);

	currentconstructhead->ctype = NONE;
	currentconstructhead->ptr.selective = NULL;
	// printf("dbg3\n");
	// printf("camehere1");
	temp_construct->ctype = temp_function->executionlist->ctype;
	temp_construct->ptr = temp_function->executionlist->ptr;
	// printf("camehere\n");
	// printf(":AST CONSTRUCTS:::   ");
	ast_walk_constructs(temp_construct);
	while(temp_function != NULL){
		printf("{%s}", temp_function->functionname);
		temp_function = temp_function->next;
	}
	free(temp_construct);
	printf("\n---------------------------------\n");
}

void ast_add_seq(char *name){
	struct ast_sequentialnode *newseqnode;
	newseqnode = malloc(sizeof(struct ast_sequentialnode));
	newseqnode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newseqnode->name, name);
	newseqnode->next.ctype = NONE;
	// if(currentsequentialhead != NULL){
	// 	newseqnode->childtype = currentsequentialhead->childtype;
	// 	switch(newseqnode->childtype){
	// 		case AST_GENVARDECL: newseqnode->child.genvardecl = currentsequentialhead->child.genvardecl; break;
	// 		case AST_MAPVARDECL: newseqnode->child.mapvardecl = currentsequentialhead->child.mapvardecl; break;
	// 		case AST_ASSIGNMENTS: newseqnode->child.assignments = currentsequentialhead->child.assignments; break;
	// 		case AST_RETURN: newseqnode->child._return = currentsequentialhead->child._return; break;
	// 		case AST_SHELLECHO: newseqnode->child.shellecho = currentsequentialhead->child.shellecho; break;
	// 		case AST_FUNCTIONCALL: newseqnode->child.functioncall = currentsequentialhead->child.functioncall; break;
	// 	}
	// 	//NOTE: free(currentsequentialhead); gives traversal error
	// 	currentsequentialhead = NULL;
	// 	// printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	// 	// printf(":AST: newseqnode made with name %s and value %s\n", newseqnode->name, newseqnode->child.shellecho->value);
	// }
	printf(":AST: newseqnode with name %s\n", newseqnode->name);
	if(currentconstructhead->ctype == NONE){
		currentconstructhead->ptr.sequential = newseqnode;
		currentconstructhead->ctype = SEQUENTIAL;
		printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	}
	else{
		// printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));
		temp_construct->ctype = currentconstructhead->ctype;
		if(currentconstructhead->ctype == SEQUENTIAL)
			temp_construct->ptr.sequential = currentconstructhead->ptr.sequential;
		if(currentconstructhead->ctype == SELECTIVE)
			temp_construct->ptr.selective = currentconstructhead->ptr.selective;
		if(currentconstructhead->ctype == ITERATIVE)
			temp_construct->ptr.iterative = currentconstructhead->ptr.iterative;
		// printf("temp_construct is ctype: %d\n", temp_construct->ctype);
		unsigned int flag = 1;
		while(flag){
			switch (temp_construct->ctype) {
				case SEQUENTIAL: //printf("at1\n");
						 ast_advanceto_next_sequential_construct(temp_construct, &flag);
						 break;
				case SELECTIVE:  //printf("at2\n");
						 ast_advanceto_next_selective_construct(temp_construct, &flag);
						 break;
				case ITERATIVE:  //printf("at3\n");
						 ast_advanceto_next_iterative_construct(temp_construct, &flag);
						 break;
			}
		}
		/* Now we have temp_construct as the last element */
		switch(temp_construct->ctype){
			case SEQUENTIAL:temp_construct->ptr.sequential->next.ctype = SEQUENTIAL;
					temp_construct->ptr.sequential->next.ptr.sequential = newseqnode;
					break;
			case SELECTIVE:temp_construct->ptr.selective->next.ctype = SEQUENTIAL;
					temp_construct->ptr.selective->next.ptr.sequential = newseqnode;
					break;
			case ITERATIVE:temp_construct->ptr.iterative->next.ctype = SEQUENTIAL;
					temp_construct->ptr.iterative->next.ptr.sequential = newseqnode;
					break;
		}
	}
	ast_walk_constructs(currentconstructhead);
}

void ast_add_sel(char *name){
	struct ast_selectivenode *newselnode;
	newselnode = malloc(sizeof(struct ast_selectivenode));
	newselnode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newselnode->name, name);
	newselnode->next.ctype = NONE;
	// printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	printf(":AST: newselnode made with name %s\n", newselnode->name);
	if(currentconstructhead->ctype == NONE){
		currentconstructhead->ptr.selective = newselnode;
		currentconstructhead->ctype = SELECTIVE	;
		// printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	}
	else{
		// printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));
		temp_construct->ctype = currentconstructhead->ctype;
		if(currentconstructhead->ctype == SEQUENTIAL)
			temp_construct->ptr.sequential = currentconstructhead->ptr.sequential;
		if(currentconstructhead->ctype == SELECTIVE)
			temp_construct->ptr.selective = currentconstructhead->ptr.selective;
		if(currentconstructhead->ctype == ITERATIVE)
			temp_construct->ptr.iterative = currentconstructhead->ptr.iterative;
		// printf("temp_construct is ctype: %d\n", temp_construct->ctype);
		unsigned int flag = 1;
		while(flag){
			switch (temp_construct->ctype) {
				case SEQUENTIAL: //printf("at1\n");
						 ast_advanceto_next_sequential_construct(temp_construct, &flag);
						 break;
				case SELECTIVE:  //printf("at2\n");
						 ast_advanceto_next_selective_construct(temp_construct, &flag);
						 break;
				case ITERATIVE:  //printf("at3\n");
						 ast_advanceto_next_iterative_construct(temp_construct, &flag);
						 break;
			}
			//printf("here temp_construct->ctype is %d\n", temp_construct->ctype);
		}
		//printf("we are out \n");
		/* Now we have temp_construct as the last element */
		switch(temp_construct->ctype){
			case SEQUENTIAL:temp_construct->ptr.sequential->next.ctype = SELECTIVE;
					temp_construct->ptr.sequential->next.ptr.selective = newselnode;
					break;
			case SELECTIVE:temp_construct->ptr.selective->next.ctype = SELECTIVE;
					temp_construct->ptr.selective->next.ptr.selective = newselnode;
					break;
			case ITERATIVE:temp_construct->ptr.iterative->next.ctype = SELECTIVE;
					temp_construct->ptr.iterative->next.ptr.selective = newselnode;
					break;
		}
	}
	ast_walk_constructs(currentconstructhead);
}

void ast_add_iter(char *name){
	struct ast_iterativenode *newiternode;
	newiternode = malloc(sizeof(struct ast_iterativenode));
	newiternode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newiternode->name, name);
	newiternode->next.ctype = NONE;
	// printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	printf(":AST: newiternode made with name %s\n", newiternode->name);
	if(currentconstructhead->ctype == NONE){
		currentconstructhead->ptr.iterative = newiternode;
		currentconstructhead->ctype = ITERATIVE	;
		// printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	}
	else{
		// printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));
		temp_construct->ctype = currentconstructhead->ctype;
		if(currentconstructhead->ctype == SEQUENTIAL)
			temp_construct->ptr.sequential = currentconstructhead->ptr.sequential;
		if(currentconstructhead->ctype == SELECTIVE)
			temp_construct->ptr.selective = currentconstructhead->ptr.selective;
		if(currentconstructhead->ctype == ITERATIVE)
			temp_construct->ptr.iterative = currentconstructhead->ptr.iterative;
		// printf("temp_construct is ctype: %d\n", temp_construct->ctype);
		unsigned int flag = 1;
		while(flag){
			switch (temp_construct->ctype) {
				case SEQUENTIAL: //printf("at1\n");
						 ast_advanceto_next_sequential_construct(temp_construct, &flag);
						 break;
				case SELECTIVE:  //printf("at2\n");
						 ast_advanceto_next_selective_construct(temp_construct, &flag);
						 break;
				case ITERATIVE:  //printf("at3\n");
						 ast_advanceto_next_iterative_construct(temp_construct, &flag);
						 break;
			}
			//printf("here temp_construct->ctype is %d\n", temp_construct->ctype);
		}
		//printf("we are out \n");
		/* Now we have temp_construct as the last element */
		switch(temp_construct->ctype){
			case SEQUENTIAL:temp_construct->ptr.sequential->next.ctype = ITERATIVE;
					temp_construct->ptr.sequential->next.ptr.iterative = newiternode;
					break;
			case SELECTIVE:temp_construct->ptr.selective->next.ctype = ITERATIVE;
					temp_construct->ptr.selective->next.ptr.iterative = newiternode;
					break;
			case ITERATIVE:temp_construct->ptr.iterative->next.ctype = ITERATIVE;
					temp_construct->ptr.iterative->next.ptr.iterative = newiternode;
					break;
		}
	}
	ast_walk_constructs(currentconstructhead);
}

void ast_walk_constructs(struct ast_construct *head){
	printf(">>>>>>>>>>>>> :AST: CONSTRUCT WALK::::  ");
	struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));
	temp_construct->ctype = head->ctype;
	if(head->ctype == SEQUENTIAL)
		temp_construct->ptr.sequential = head->ptr.sequential;
	if(head->ctype == SELECTIVE)
		temp_construct->ptr.selective = head->ptr.selective;
	if(head->ctype == ITERATIVE)
		temp_construct->ptr.selective = head->ptr.selective;
	unsigned int flag = 1;
	while(flag && temp_construct->ctype != NONE){
		switch (temp_construct->ctype) {
			case SEQUENTIAL: printf("[AST-SEQ]{%s}  ", temp_construct->ptr.sequential->name);
					 ast_advanceto_next_sequential_construct(temp_construct, &flag);
					 break;
			case SELECTIVE:  printf("[AST-SEL]{%s}  ", temp_construct->ptr.selective->name);
					 ast_advanceto_next_selective_construct(temp_construct, &flag);
					 break;
			case ITERATIVE:  printf("[AST-ITR]{%s}  ", temp_construct->ptr.iterative->name);
					 ast_advanceto_next_iterative_construct(temp_construct, &flag);
					 break;
		}
	}
	printf("\n" );
}

void ast_add_seq_shellecho(char *echo){
	currentsequentialhead = malloc(sizeof(struct ast_sequentialnode));
	currentsequentialhead->childtype = AST_SHELLECHO;
	char *temp = malloc(sizeof(char)*(strlen(echo)+1));
	strcpy(temp,echo);
	currentsequentialhead->child.shellecho = malloc(sizeof(struct ast_sequential_shellecho));
	currentsequentialhead->child.shellecho->value = temp;
}

void ast_make_keyvalpair(char *key, char *value){
	struct keyvalpairs *temp = malloc(sizeof(struct keyvalpairs));
	temp->key = malloc(sizeof(char)*(strlen(key)+1));
	temp->value = malloc(sizeof(char)*(strlen(value)+1));
	strcpy(temp->key, key);
	strcpy(temp->value, value);
	temp->next = currentkeyvalpairshead;
	currentkeyvalpairshead = temp;

	printf(":AST: Current keyvalpairs: ");
	temp = currentkeyvalpairshead;
	while(temp!=NULL){
		printf("  {%s|%s}  ", temp->key, temp->value);
		temp = temp->next;
	}
	printf("\n");
}

void ast_advanceto_next_sequential_construct(struct ast_construct *temp_construct, unsigned int *flag){
	switch(temp_construct->ptr.sequential->next.ctype){
		case NONE:	*flag = 0; break;
		case SEQUENTIAL:temp_construct->ctype = temp_construct->ptr.sequential->next.ctype;
				temp_construct->ptr.sequential = temp_construct->ptr.sequential->next.ptr.sequential; break;
		case SELECTIVE: temp_construct->ctype = temp_construct->ptr.sequential->next.ctype;
				temp_construct->ptr.selective = temp_construct->ptr.sequential->next.ptr.selective; break;
		case ITERATIVE: temp_construct->ctype = temp_construct->ptr.sequential->next.ctype;
				temp_construct->ptr.iterative = temp_construct->ptr.sequential->next.ptr.iterative; break;
	}
}
void ast_advanceto_next_selective_construct(struct ast_construct *temp_construct, unsigned int *flag){
	switch(temp_construct->ptr.selective->next.ctype){
		case NONE:	*flag = 0; break;
		case SEQUENTIAL: temp_construct->ctype = temp_construct->ptr.selective->next.ctype;
				temp_construct->ptr.sequential = temp_construct->ptr.selective->next.ptr.sequential; break;
		case SELECTIVE: temp_construct->ctype = temp_construct->ptr.selective->next.ctype;
				temp_construct->ptr.selective = temp_construct->ptr.selective->next.ptr.selective; break;
		case ITERATIVE: temp_construct->ctype = temp_construct->ptr.selective->next.ctype;
				temp_construct->ptr.iterative = temp_construct->ptr.selective->next.ptr.iterative; break;
	}
}
void ast_advanceto_next_iterative_construct(struct ast_construct *temp_construct, unsigned int *flag){
	switch(temp_construct->ptr.iterative->next.ctype){
		case NONE:	*flag = 0;break;
		case SEQUENTIAL: temp_construct->ctype = temp_construct->ptr.iterative->next.ctype;
				temp_construct->ptr.sequential = temp_construct->ptr.iterative->next.ptr.sequential; break;
		case SELECTIVE: temp_construct->ctype = temp_construct->ptr.iterative->next.ctype;
				temp_construct->ptr.selective = temp_construct->ptr.iterative->next.ptr.selective; break;
		case ITERATIVE: temp_construct->ctype = temp_construct->ptr.iterative->next.ctype;
				temp_construct->ptr.iterative = temp_construct->ptr.iterative->next.ptr.iterative; break;
	}
}
