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
	currentconstructhead->data.selective = NULL;
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
		currentconstructhead->data.sequential = newseqnode;
		currentconstructhead->ctype = SEQUENTIAL;
		printf("currentconstructhead is ctype: %d\n", currentconstructhead->ctype);
	}
	else{
		printf("[AST]currentconstructhead was FULL\n");
		struct ast_construct *tempptr = malloc(sizeof(struct ast_construct));
		tempptr->ctype = currentconstructhead->ctype;
		if(currentconstructhead->ctype == SEQUENTIAL)
			tempptr->data.sequential = currentconstructhead->data.sequential;
		if(currentconstructhead->ctype == SELECTIVE)
			tempptr->data.selective = currentconstructhead->data.selective;
		if(currentconstructhead->ctype == ITERATIVE)
			tempptr->data.selective = currentconstructhead->data.selective;
		printf("tempptr is ctype: %d\n", tempptr->ctype);
		while(tempptr->ctype != NONE){
			switch (tempptr->ctype) {
				case SEQUENTIAL: printf("at1\n");
						 tempptr->ctype = tempptr->data.sequential->next.ctype;
						 printf("tempptr->data.sequential->next.ctype is %d\n",tempptr->data.sequential->next.ctype);
						 switch(tempptr->data.sequential->next.ctype){
							 case NONE:	break;
							 case SEQUENTIAL: tempptr->data.sequential = tempptr->data.sequential->next.data.sequential; break;
							 case SELECTIVE: tempptr->data.selective = tempptr->data.sequential->next.data.selective; break;
							 case ITERATIVE: tempptr->data.iterative = tempptr->data.sequential->next.data.iterative; break;
						 }
						 break;
				case SELECTIVE:  printf("at2\n");tempptr->ctype = tempptr->data.selective->next.ctype;
						 switch(tempptr->data.selective->next.ctype){
							 case NONE:	break;
							 case SEQUENTIAL: tempptr->data.sequential = tempptr->data.selective->next.data.sequential; break;
							 case SELECTIVE: tempptr->data.selective = tempptr->data.selective->next.data.selective; break;
							 case ITERATIVE: tempptr->data.iterative = tempptr->data.selective->next.data.iterative; break;
						 }
						 break;
				case ITERATIVE:  printf("at3\n");tempptr->ctype = tempptr->data.iterative->next.ctype;
						 switch(tempptr->data.iterative->next.ctype){
							 case NONE:	break;
							 case SEQUENTIAL: tempptr->data.sequential = tempptr->data.iterative->next.data.sequential; break;
							 case SELECTIVE: tempptr->data.selective = tempptr->data.iterative->next.data.selective; break;
							 case ITERATIVE: tempptr->data.iterative = tempptr->data.iterative->next.data.iterative; break;
						 }
						 break;
			}
		}
		/* Now we have tempptr as the last element */
		tempptr->data.sequential->next.ctype = SEQUENTIAL;
		tempptr->data.sequential->next.data.sequential = newseqnode;
	}
	ast_walk_constructs();
	return (void *)newseqnode;
}

void ast_walk_constructs(){
	struct ast_construct *tempptr = malloc(sizeof(struct ast_construct));
	tempptr->ctype = currentconstructhead->ctype;
	if(currentconstructhead->ctype == SEQUENTIAL)
		tempptr->data.sequential = currentconstructhead->data.sequential;
	if(currentconstructhead->ctype == SELECTIVE)
		tempptr->data.selective = currentconstructhead->data.selective;
	if(currentconstructhead->ctype == ITERATIVE)
		tempptr->data.selective = currentconstructhead->data.selective;
	while(tempptr->ctype != NONE){
		switch (tempptr->ctype) {
			case SEQUENTIAL: printf("[AST-SEQ]{%s}  ", tempptr->data.sequential->name);
					 tempptr->ctype = tempptr->data.sequential->next.ctype;
					 switch(tempptr->data.sequential->next.ctype){
						 case NONE:	break;
						 case SEQUENTIAL: tempptr->data.sequential = tempptr->data.sequential->next.data.sequential; break;
						 case SELECTIVE: tempptr->data.selective = tempptr->data.sequential->next.data.selective; break;
						 case ITERATIVE: tempptr->data.iterative = tempptr->data.sequential->next.data.iterative; break;
					 }
					 break;
			case SELECTIVE:  printf("[AST-SEL]{%s}  ", tempptr->data.selective->name);
					 tempptr->ctype = tempptr->data.selective->next.ctype;
					 switch(tempptr->data.selective->next.ctype){
						 case NONE:	break;
						 case SEQUENTIAL: tempptr->data.sequential = tempptr->data.selective->next.data.sequential; break;
						 case SELECTIVE: tempptr->data.selective = tempptr->data.selective->next.data.selective; break;
						 case ITERATIVE: tempptr->data.iterative = tempptr->data.selective->next.data.iterative; break;
					 }
					 break;
			case ITERATIVE:  printf("[AST-ITR]{%s}  ", tempptr->data.iterative->name);
					 tempptr->ctype = tempptr->data.iterative->next.ctype;
					 switch(tempptr->data.iterative->next.ctype){
						 case NONE:	break;
						 case SEQUENTIAL: tempptr->data.sequential = tempptr->data.iterative->next.data.sequential; break;
						 case SELECTIVE: tempptr->data.selective = tempptr->data.iterative->next.data.selective; break;
						 case ITERATIVE: tempptr->data.iterative = tempptr->data.iterative->next.data.iterative; break;
					 }
					 break;
		}
	}
	printf("\n" );
}
