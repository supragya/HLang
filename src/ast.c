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
	struct ast_function_node *tempptr;
	tempptr = malloc(sizeof(struct ast_function_node));
	if(!tempptr)
		printf(":AST: [Error] Cannot allocate memory for function node (%s)\n", functionname);
	tempptr->functionname = malloc(sizeof(char)*(strlen(functionname)+1));
	if(!tempptr->functionname)
		printf(":AST: [Error] Cannot allocate memory for function node's string\n");
	strcpy(tempptr->functionname, functionname);
	tempptr->next = rootnode->functions;
	rootnode->functions = tempptr;

	tempptr = rootnode->functions;
	while(tempptr != NULL){
		printf("{%s}", tempptr->functionname);
		tempptr = tempptr->next;
	}
	printf("\n");
}

void* ast_add_seq(char *name){
	printf("called");
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
		struct ast_construct *tempptr = malloc(sizeof(struct ast_construct));
		tempptr->ctype = currentconstructhead->ctype;
		if(currentconstructhead->ctype == SEQUENTIAL)
			tempptr->ptr.sequential = currentconstructhead->ptr.sequential;
		if(currentconstructhead->ctype == SELECTIVE)
			tempptr->ptr.selective = currentconstructhead->ptr.selective;
		if(currentconstructhead->ctype == ITERATIVE)
			tempptr->ptr.selective = currentconstructhead->ptr.selective;
		printf("tempptr is ctype: %d\n", tempptr->ctype);
		while(tempptr->ctype != NONE){
			switch (tempptr->ctype) {
				case SEQUENTIAL: printf("at1\n");
						 tempptr->ctype = tempptr->ptr.sequential->next.ctype;
						 printf("tempptr->ptr.sequential->next.ctype is %d\n",tempptr->ptr.sequential->next.ctype);
						 switch(tempptr->ptr.sequential->next.ctype){
							 case NONE:	break;
							 case SEQUENTIAL: tempptr->ptr.sequential = tempptr->ptr.sequential->next.ptr.sequential; break;
							 case SELECTIVE: tempptr->ptr.selective = tempptr->ptr.sequential->next.ptr.selective; break;
							 case ITERATIVE: tempptr->ptr.iterative = tempptr->ptr.sequential->next.ptr.iterative; break;
						 }
						 break;
				case SELECTIVE:  printf("at2\n");tempptr->ctype = tempptr->ptr.selective->next.ctype;
						 switch(tempptr->ptr.selective->next.ctype){
							 case NONE:	break;
							 case SEQUENTIAL: tempptr->ptr.sequential = tempptr->ptr.selective->next.ptr.sequential; break;
							 case SELECTIVE: tempptr->ptr.selective = tempptr->ptr.selective->next.ptr.selective; break;
							 case ITERATIVE: tempptr->ptr.iterative = tempptr->ptr.selective->next.ptr.iterative; break;
						 }
						 break;
				case ITERATIVE:  printf("at3\n");tempptr->ctype = tempptr->ptr.iterative->next.ctype;
						 switch(tempptr->ptr.iterative->next.ctype){
							 case NONE:	break;
							 case SEQUENTIAL: tempptr->ptr.sequential = tempptr->ptr.iterative->next.ptr.sequential; break;
							 case SELECTIVE: tempptr->ptr.selective = tempptr->ptr.iterative->next.ptr.selective; break;
							 case ITERATIVE: tempptr->ptr.iterative = tempptr->ptr.iterative->next.ptr.iterative; break;
						 }
						 break;
			}
		}
		/* Now we have tempptr as the last element */
		tempptr->ptr.sequential->next.ctype = SEQUENTIAL;
		tempptr->ptr.sequential->next.ptr.sequential = newseqnode;
	}
	ast_walk_constructs();
	return (void *)newseqnode;
}

void ast_walk_constructs(){
	struct ast_construct *tempptr = malloc(sizeof(struct ast_construct));
	tempptr->ctype = currentconstructhead->ctype;
	if(currentconstructhead->ctype == SEQUENTIAL)
		tempptr->ptr.sequential = currentconstructhead->ptr.sequential;
	if(currentconstructhead->ctype == SELECTIVE)
		tempptr->ptr.selective = currentconstructhead->ptr.selective;
	if(currentconstructhead->ctype == ITERATIVE)
		tempptr->ptr.selective = currentconstructhead->ptr.selective;
	while(tempptr->ctype != NONE){
		switch (tempptr->ctype) {
			case SEQUENTIAL: printf("[AST-SEQ]{%s}  ", tempptr->ptr.sequential->name);
					 tempptr->ctype = tempptr->ptr.sequential->next.ctype;
					 switch(tempptr->ptr.sequential->next.ctype){
						 case NONE:	break;
						 case SEQUENTIAL: tempptr->ptr.sequential = tempptr->ptr.sequential->next.ptr.sequential; break;
						 case SELECTIVE: tempptr->ptr.selective = tempptr->ptr.sequential->next.ptr.selective; break;
						 case ITERATIVE: tempptr->ptr.iterative = tempptr->ptr.sequential->next.ptr.iterative; break;
					 }
					 break;
			case SELECTIVE:  printf("[AST-SEL]{%s}  ", tempptr->ptr.selective->name);
					 tempptr->ctype = tempptr->ptr.selective->next.ctype;
					 switch(tempptr->ptr.selective->next.ctype){
						 case NONE:	break;
						 case SEQUENTIAL: tempptr->ptr.sequential = tempptr->ptr.selective->next.ptr.sequential; break;
						 case SELECTIVE: tempptr->ptr.selective = tempptr->ptr.selective->next.ptr.selective; break;
						 case ITERATIVE: tempptr->ptr.iterative = tempptr->ptr.selective->next.ptr.iterative; break;
					 }
					 break;
			case ITERATIVE:  printf("[AST-ITR]{%s}  ", tempptr->ptr.iterative->name);
					 tempptr->ctype = tempptr->ptr.iterative->next.ctype;
					 switch(tempptr->ptr.iterative->next.ctype){
						 case NONE:	break;
						 case SEQUENTIAL: tempptr->ptr.sequential = tempptr->ptr.iterative->next.ptr.sequential; break;
						 case SELECTIVE: tempptr->ptr.selective = tempptr->ptr.iterative->next.ptr.selective; break;
						 case ITERATIVE: tempptr->ptr.iterative = tempptr->ptr.iterative->next.ptr.iterative; break;
					 }
					 break;
		}
	}
	printf("\n" );
}
