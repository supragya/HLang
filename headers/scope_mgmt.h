enum bordertype{
	BORDER,
	NON_BORDER
};
struct scope_node_t{
	unsigned int scope;
	int is_border;
	char *description;
	struct scope_node_t *next;
};
int sms_init();
unsigned int sms_get_current_scope();
int sms_child_scope_begin(enum bordertype bor, char *description);
int sms_child_scope_end();
void sms_display_status();
struct scope_node_t *nodehead;
