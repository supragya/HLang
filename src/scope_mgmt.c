/* HLang Scope management subystem
 * created by Supragya Raj
 */
#include <malloc.h>
#include "scope_mgmt.h"
#include "variable_mgmt.h"

struct scope_node_t *rootnode, *currentnode, *currentrootnode;
unsigned int currentcounter;

int sms_init(){
	currentcounter = 0;
	rootnode = malloc(sizeof(struct scope_node_t));
	if(!rootnode)
		return 1;
	rootnode->scope = currentcounter;
	rootnode->parent = rootnode->child = NULL;
	currentnode = rootnode;
	currentrootnode = rootnode;
	return 0;
}

unsigned int sms_get_current_scope(){
	return currentnode->scope;
}

int sms_child_scope_begin(){
	struct scope_node_t *tempptr;
	tempptr = malloc(sizeof(struct scope_node_t));
	if(!tempptr){
		printf(":SMS: [Error] Cannot create a child scope of %d. Low on memory.\n", currentcounter);
		return 1;
	}
	tempptr->parent = currentnode;
	tempptr->child = NULL;
	tempptr->scope = ++currentcounter;
	currentnode = tempptr;
	sms_display_status();
	return 0;
}

int sms_child_scope_end(){
	if(currentnode == currentrootnode){
		printf(":SMS: [Error] Cannot go up the scope DLL: current node is root node.\n");
		return 1;
	}
	vms_decommission_scope(currentnode->scope);
	currentnode = currentnode->parent;
	free(currentnode->child);
	currentnode->child = NULL;
	--currentcounter;
	sms_display_status();
	return 0;
}

void sms_display_status(){
	printf(":SMS: [Current Status] RUNNING SCOPE: %d, ROOT SCOPE: %d \n", currentnode->scope, currentrootnode->scope);
}
