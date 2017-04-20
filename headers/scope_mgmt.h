struct scope_node_t{
	unsigned int scope;
	struct scope_node_t *parent, *child;
};

int sms_init();
unsigned int sms_get_current_scope();
int sms_child_scope_begin();
int sms_child_scope_end();
void sms_display_status();
