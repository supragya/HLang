/* HLang Scope management subystem
 * created by Supragya Raj
 */

#include <malloc.h>
#include <string.h>
#include "scope_mgmt.h"
#include "variable_mgmt.h"
#include "verbose.h"


int sms_init(){
	nodehead = NULL;
	return 0;
}
unsigned int sms_get_current_scope(){
	return nodehead->scope;
}
int sms_child_scope_begin(enum bordertype bor, char* description){
	struct scope_node_t *tempptr;
	tempptr = malloc(sizeof(struct scope_node_t));
	if(!tempptr){
		if(SMSVERBOSE())printf(":SMS: [Error] Cannot create a child scope. Low on memory.\n");
		return 1;
	}
	tempptr->next = nodehead;
	tempptr->scope = (nodehead == NULL)?0:(nodehead->scope+1);
	nodehead = tempptr;
	if(SMSVERBOSE())printf(":SMS: Beginning new scope %d.\n",tempptr->scope);
	tempptr->description = malloc(sizeof(char)*(strlen(description)+1));
	strcpy(tempptr->description, description);
	tempptr->is_border = bor;
	sms_display_status();
	return 0;
}
int sms_child_scope_end(){
	if(nodehead == NULL){
		if(SMSVERBOSE())printf(":SMS: [Error] Cannot go up the scope DLL: current node is root node.\n");
		return 1;
	}
	vms_decommission_scope(nodehead->scope);
	struct scope_node_t *currentnode = nodehead;
	nodehead = nodehead->next;
	free(currentnode);
	sms_display_status();
	return 0;
}
void sms_display_status(){
	if(SMSVERBOSE())printf(":SMS: Current scope status: ");
	struct scope_node_t *temp = nodehead;
	while(temp != NULL){
		if(temp->is_border == BORDER){
			if(SMSVERBOSE())printf(" %d(%s)* ", temp->scope, temp->description);
		}
		else{
			if(SMSVERBOSE())printf(" %d(%s) ", temp->scope, temp->description);
		}
		temp = temp->next;
	}
	if(SMSVERBOSE())printf("\n");
}
