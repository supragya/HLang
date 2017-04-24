/* Shell talker
 * created by Supragya Raj
 */
#include <stdio.h>
#include "shell.h"

int shellexecute(char *shellstr){
	printf(":SHELL: [HLang dummy executor] Shell execution of {%s} returns: ", shellstr);
	int retval;
	scanf("%d",&retval);
	return retval;
}
