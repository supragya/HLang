/* HLang variable management service
 * created by Supragya Raj
 */

#include <string.h>
#include "buildtime_hlang-parser.h"
#include "hlang-parser.h"
#include "variable_mgmt.h"
unsigned long locations_available = TOTAL_SLOTS;

variable_t storage[TOTAL_SLOTS];

variable_ptr_t var_new(char *new_varname, unsigned int scope){
	/* Testing whether the combination already exists or not */
	if(1){
		/* Find integer position where the variable will be stored */
		unsigned long position = 0;
		if(locations_available !=0)
			/* We have locations available for variable storage */
			position = find_valid_location(new_varname);
		else
			/* Ran out of memory */
			yyerror("Not enough memory to save variables");

		/* Populating the variable data */
		storage[position].name 		= new_varname;
		storage[position].value		= NULL;
		storage[position].scope		= scope;
		storage[position].occupation	= OCCUPIED;

		/* Decrement the number of locations available by one */
		locations_available--;
		
		return position;
	}
	else{
		yyerror("Variable declared twice");
		return TOTAL_SLOTS;
	}
}


unsigned long find_valid_location(char *varname){
	/* Find hashed location in the bin corresponding to varname */
	unsigned long hashval = calchash(varname);
	
	/* If the given location is available, return position, else find location with least positive offset */
	while(storage[hashval].occupation == OCCUPIED)
		hashval = (hashval+1)%TOTAL_SLOTS;
	return hashval;
}

unsigned long calchash(char *varname){
	int len = strlen(varname);
	unsigned int i = 0, temp;
	unsigned int pos = 0;	

	for(i = 0; i<len; i++){
		/* ASCII character val */
		temp = varname[i];

		/* Horner's rule for computation */
		pos = (H_ALPHA*pos + temp) % TOTAL_SLOTS;
	}
	return pos;
}

void var_assign(variable_ptr_t var, char *str){
	if (var >= TOTAL_SLOTS)
		yyerror("Undefined variable_ptr_t value");
	else if(!storage[var].occupation == OCCUPIED)
		yyerror("Assignment to undeclared variable!");
	strcpy(storage[var].value, str);
}

void var_assign_to_varname(char *varname, unsigned int scope, char *str){
	var_assign(var_lookup(varname, scope), str);
}

variable_ptr_t var_lookup(char* varname, unsigned int scope){
	/* Find hash of the varname */
	unsigned long hashval = calchash(varname);

	/* Find variable in storage[], positive offsets */
	unsigned long ctr = TOTAL_SLOTS;
	while(!(storage[hashval].occupation == OCCUPIED && !strcmp(storage[hashval].name, varname) && storage[hashval].scope == scope) && ctr)
		ctr--;
	
	/* Check whether we have the required variable */
	return (storage[hashval].occupation == OCCUPIED && !strcmp(storage[hashval].name, varname) && storage[hashval].scope == scope) ? hashval : TOTAL_SLOTS;
}
