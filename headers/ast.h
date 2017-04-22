struct ast_function_node{
	char *functionname;
	struct ast_function_node *next;
	struct ast_construct *executionlist;
};

struct ast_root_node{
	struct ast_function_node *functions;
};

enum ast_constructtype_t{
	NONE,
	SEQUENTIAL,
	SELECTIVE,
	ITERATIVE
};

enum ast_selectiveconstructtype{
	AST_GENVARDECL,
	AST_MAPVARDECL,
	AST_ASSIGNMENTS,
	AST_RETURN,
	AST_SHELLECHO,
	AST_FUNCTIONCALL
};

enum ast_iterativeconstructtype{
	AST_FORLOOP,
	AST_WHILELOOP
};

enum functioncallargtype{
	F_STRING,
	F_VARNAME
};

enum returntype{
	R_STRING,
	R_VARNAME,
	R_FUNCTIONCALL
};

union ast_nextconstruct_ptr{
	struct ast_sequentialnode *sequential;
	struct ast_selectivenode *selective;
	struct ast_iterativenode *iterative;
};

union ast_sequential_code_ptr{
	struct ast_sequential_shellecho *shellecho;
	struct ast_sequential_functioncall *functioncall;
	struct ast_sequential_genvardecl *genvardecl;
	struct ast_sequential_mapvardecl *mapvardecl;
	struct ast_sequential_return *_return;
	struct ast_sequential_varassignment *assignments;
};

struct ast_sequential_shellecho{
	char *value;
};
struct ast_sequential_functioncall{
	char *functionname;
	struct functioncallargs *args;
};
struct functioncallargs{
	enum functioncallargtype argtype;
	char *argument_str;
	struct ast_sequential_functioncall *fcall;
	struct functioncallargs *next;
};
struct ast_sequential_genvardecl{
	struct vardecl_assignmentlist *list;
};
struct vardecl_assignmentlist{
	char *varname;
	char *value;
	struct vardecl_assignmentlist *next;
};
struct ast_sequential_mapvardecl{
	struct mapvarlist *mapvarlist;
};
struct mapvarlist{
	char *mapname;
	struct keyvalpairs *keyvalpairs;
	struct mapvarlist *next;
};
struct keyvalpairs{
	char *key;
	char *value;
	struct keyvalpairs *next;
};
struct ast_sequential_return{
	struct returnval *retdata;
};
struct returnval{
	enum returntype type;
	char *ret_str;
};
struct ast_sequential_varassignment{
	struct var_assignments *assignments;
};

struct ast_construct{
	enum ast_constructtype_t ctype;
	union ast_nextconstruct_ptr ptr;
	struct ast_construct *next;
};

struct ast_sequentialnode{
	char *name;
	enum ast_selectiveconstructtype childtype;
	union ast_sequential_code_ptr child;
};

struct ast_selectivenode{
	char *name;
	enum ast_constructtype_t nextconstructtype;
};

struct ast_iterativenode{
	char *name;
	enum ast_iterativeconstructtype childtype;
	void *child;
};


struct ast_root_node *rootnode;


int ast_init();
void ast_add_function(char *functionname);
void ast_add_seq(char *name);
void ast_add_sel(char *name);
void ast_add_iter(char *name);
void ast_walk_constructs(struct ast_construct *head);
void ast_advanceto_next_sequential_construct(struct ast_construct *temp_construct, unsigned int *flag);
void ast_advanceto_next_selective_construct(struct ast_construct *temp_construct, unsigned int *flag);
void ast_advanceto_next_iterative_construct(struct ast_construct *temp_construct, unsigned int *flag);
void ast_add_seq_shellecho(char *echo);
void ast_make_keyvalpair(char *key, char *value);
void ast_add_mapdeclnode(char *mapname);
void ast_add_seq_mapdecl();
void ast_add_seq_vardecl();
void ast_make_vardecl_assignment(char *varname, char *value);
void ast_make_vardecl_assignment_defaultval(char *varname);
void ast_add_arguments_string(char *argstr);
void ast_add_arguments_varname(char *argstr);
void ast_add_seq_functioncall(char *functionname);
void ast_set_return_val_varname(char *varname);
void ast_add_seq_return();
