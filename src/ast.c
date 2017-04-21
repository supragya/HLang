/* HLang Abstract Syntax Tree
 * created by Supragya Raj
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ast.h"
struct ast_root_node *rootnode;
struct ast_construct *currentconstructhead;

int ast_init(){
	rootnode = malloc(sizeof(struct ast_root_node));
	currentconstructhead = malloc(sizeof(struct ast_construct));
	if(!rootnode)
		return 1;
	rootnode->functions = NULL;
	currentconstructhead->ctype = NONE;
	currentconstructhead->ptr.selective = NULL;
	printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	return 0;
}

void ast_add_function(char *functionname){
	printf(":AST: Adding functionname: %s\n", functionname);
	struct ast_function_node *temp_construct;
	temp_construct = malloc(sizeof(struct ast_function_node));
	if(!temp_construct)
		printf(":AST: [Error] Cannot allocate memory for function node (%s)\n", functionname);
	temp_construct->functionname = malloc(sizeof(char)*(strlen(functionname)+1));
	if(!temp_construct->functionname)
		printf(":AST: [Error] Cannot allocate memory for function node's string\n");
	strcpy(temp_construct->functionname, functionname);
	temp_construct->next = rootnode->functions;
	rootnode->functions = temp_construct;
	temp_construct.executionlist = malloc(si)
	currentconstructhead;

	while(temp_construct != NULL){
		printf("{%s}", temp_construct->functionname);
		temp_construct = temp_construct->next;
	}
	printf("\n");
}

void* ast_add_seq(char *name){
	struct ast_sequentialnode *newseqnode;
	newseqnode = malloc(sizeof(struct ast_sequentialnode));
	newseqnode->name = malloc(sizeof(char)*(strlen(name)+1));
	strcpy(newseqnode->name, name);
	newseqnode->next.ctype = NONE;
	printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	printf(":AST: newseqnode made with name %s\n", newseqnode->name);
	if(currentconstructhead->ctype == NONE){
		currentconstructhead->ptr.sequential = newseqnode;
		currentconstructhead->ctype = SEQUENTIAL;
		printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	}
	else{
		printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));
		temp_construct->ctype = currentconstructhead->ctype;
		if(currentconstructhead->ctype == SEQUENTIAL)
			temp_construct->ptr.sequential = currentconstructhead->ptr.sequential;
		if(currentconstructhead->ctype == SELECTIVE)
			temp_construct->ptr.selective = currentconstructhead->ptr.selective;
		if(currentconstructhead->ctype == ITERATIVE)
			temp_construct->ptr.selective = currentconstructhead->ptr.selective;
		printf("temp_construct is ctype: %d\n", temp_construct->ctype);
		while(temp_construct->ctype != NONE){
			switch (temp_construct->ctype) {
				case SEQUENTIAL: printf("at1\n");
						 ast_advanceto_next_sequential_construct(temp_construct);
						 break;
				case SELECTIVE:  printf("at2\n");
						 ast_advanceto_next_selective_construct(temp_construct);
						 break;
				case ITERATIVE:  printf("at3\n");
						 ast_advanceto_next_iterative_construct(temp_construct);
						 break;
			}
		}
		/* Now we have temp_construct as the last element */
		temp_construct->ptr.sequential->next.ctype = SEQUENTIAL;
		temp_construct->ptr.sequential->next.ptr.sequential = newseqnode;
	}
	ast_walk_constructs();
	return (void *)newseqnode;
}

void ast_walk_constructs(){
	struct ast_construct *temp_construct = malloc(sizeof(struct ast_construct));
	temp_construct->ctype = currentconstructhead->ctype;
	if(currentconstructhead->ctype == SEQUENTIAL)
		temp_construct->ptr.sequential = currentconstructhead->ptr.sequential;
	if(currentconstructhead->ctype == SELECTIVE)
		temp_construct->ptr.selective = currentconstructhead->ptr.selective;
	if(currentconstructhead->ctype == ITERATIVE)
		temp_construct->ptr.selective = currentconstructhead->ptr.selective;
	while(temp_construct->ctype != NONE){
		switch (temp_construct->ctype) {
			case SEQUENTIAL: printf("[AST-SEQ]{%s}  ", temp_construct->ptr.sequential->name);
					 ast_advanceto_next_sequential_construct(temp_construct);
					 break;
			case SELECTIVE:  printf("[AST-SEL]{%s}  ", temp_construct->ptr.selective->name);
					 ast_advanceto_next_selective_construct(temp_construct);
					 break;
			case ITERATIVE:  printf("[AST-ITR]{%s}  ", temp_construct->ptr.iterative->name);
					 ast_advanceto_next_iterative_construct(temp_construct);
					 break;
		}
	}
	printf("\n" );
}

void ast_advanceto_next_sequential_construct(struct ast_construct *temp_construct){
	temp_construct->ctype = temp_construct->ptr.sequential->next.ctype;
	switch(temp_construct->ptr.sequential->next.ctype){
		case NONE:	break;
		case SEQUENTIAL: temp_construct->ptr.sequential = temp_construct->ptr.sequential->next.ptr.sequential; break;
		case SELECTIVE: temp_construct->ptr.selective = temp_construct->ptr.sequential->next.ptr.selective; break;
		case ITERATIVE: temp_construct->ptr.iterative = temp_construct->ptr.sequential->next.ptr.iterative; break;
	}
}
void ast_advanceto_next_selective_construct(struct ast_construct *temp_construct){
	temp_construct->ctype = temp_construct->ptr.selective->next.ctype;
	switch(temp_construct->ptr.selective->next.ctype){
		case NONE:	break;
		case SEQUENTIAL: temp_construct->ptr.sequential = temp_construct->ptr.selective->next.ptr.sequential; break;
		case SELECTIVE: temp_construct->ptr.selective = temp_construct->ptr.selective->next.ptr.selective; break;
		case ITERATIVE: temp_construct->ptr.iterative = temp_construct->ptr.selective->next.ptr.iterative; break;
	}
}
void ast_advanceto_next_iterative_construct(struct ast_construct *temp_construct){
	temp_construct->ctype = temp_construct->ptr.iterative->next.ctype;
	switch(temp_construct->ptr.iterative->next.ctype){
		case NONE:	break;
		case SEQUENTIAL: temp_construct->ptr.sequential = temp_construct->ptr.iterative->next.ptr.sequential; break;
		case SELECTIVE: temp_construct->ptr.selective = temp_construct->ptr.iterative->next.ptr.selective; break;
		case ITERATIVE: temp_construct->ptr.iterative = temp_construct->ptr.iterative->next.ptr.iterative; break;
	}
}
