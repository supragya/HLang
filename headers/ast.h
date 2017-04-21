struct ast_function_node{
	char *functionname;
	struct ast_function_node *next;
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

union ast_nextconstruct_ptr{
	struct ast_sequentialnode *sequential;
	struct ast_selectivenode *selective;
	struct ast_iterativenode *iterative;
};

struct ast_construct{
	enum ast_constructtype_t ctype;
	union ast_nextconstruct_ptr data;
};

struct ast_sequentialnode{
	char *name;
	enum ast_selectiveconstructtype childtype;
	void *child;
	struct ast_construct next;
};

struct ast_selectivenode{
	char *name;
	enum ast_constructtype_t nextconstructtype;
	struct ast_construct next;
};

struct ast_iterativenode{
	char *name;
	enum ast_iterativeconstructtype childtype;
	void *child;
	struct ast_construct next;
};





int ast_init();
void ast_add_function(char *functionname);
void* ast_add_seq(char *name);
void ast_walk_constructs();
