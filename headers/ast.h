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

enum expr_expression1_operation{
	OP_NONE_EXPR1,
	OP_ADD,
	OP_SUB
};
struct expr_expression{
	enum expr_expression1_operation op;
	struct expr_expression *expression1;
	struct expr_expression2 *expression2;
	char *value;
};
enum expr_expression2_operation{
	OP_NONE_EXPR2,
	OP_MUL,
	OP_DIV,
	OP_TRUNCDIV
};
struct expr_expression2{
	enum expr_expression2_operation op;
	struct expr_expression2 *expression2;
	struct expr_expression3 *expression3;
	char *value;
};
enum expr_expression3_type{
	UNPREC_EXPR_SUCC,
	UNPREC_DISCR_SUCC
};
struct expr_expression3{
	enum expr_expression3_type type;
	struct expr_unary_preceder *unprec;
	struct expr_successor *succ;
	struct expr_expression1 *expr;
	struct expr_discrete_term *disc_term;
	char *value;
};
enum expr_discrete_term_type{
	DISCRETE_STRING,
	DISCRETE_VARIABLE,
	DISCRETE_FUNCTIONCALL,
	DISCRETE_SHELLECHO
};
struct expr_discrete_term{
	enum expr_discrete_term_type type;
	char *termdata;
	struct ast_sequential_functioncall *functioncalldata;
	struct ast_sequential_shellecho *shellecho;
};
enum expr_unary_preceder_type{
	UNARY_POS,
	UNARY_NEG
};
struct expr_unary_preceder{
	enum expr_unary_preceder_type precedortype;
};
enum expr_successor_type{
	FACTORIAL,
	EXP_DISCRETE_TERM,
	EXP_EXPRESSION
};
struct expr_successor{
	enum expr_successor_type type;
	struct expr_expression *expr;
	struct expr_discrete_term *disc_term;
};

struct expr_expression1ll{
	struct expr_expression1 *data;
	struct expr_expression1ll *next;
};
struct expr_expression2ll{
	struct expr_expression2 *data;
	struct expr_expression2ll *next;

};
struct expr_expression3ll{
	struct expr_expression2 *data;
	struct expr_expression2ll *next;

};
struct expr_discrete_termll{
	struct expr_discrete_term *data;
	struct expr_discrete_termll *next;
};
struct expr_unary_precederll{
	struct expr_unary_preceder *data;
	struct expr_unary_precederll *next;
};
struct expr_successorll{
	struct expr_successor *data;
	struct expr_successorll *next;
};

struct ast_root_node *rootnode;
struct ast_construct *currentconstructhead;
struct ast_sequentialnode *currentsequentialhead;
struct keyvalpairs *currentkeyvalpairshead;
struct mapvarlist *currentmapvarlisthead;
struct vardecl_assignmentlist *currentvardeclassignmentlisthead;
struct functioncallargs *currentfunccallargshead;
struct returnval *currentreturnvalhead;
struct expr_expression1ll *currentexpression1head;
struct expr_expression2ll *currentexpression2head;
struct expr_expression3ll *currentexpression3head;
struct expr_discrete_termll *currentexpression3discretermhead;
struct expr_unary_precederll *currentexpression3unaryprecederhead;
struct expr_successorll *currentexpression3successorhead;


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
void ast_add_expr3_discrete_term_string(char *str);
void ast_add_expr3_discrete_term_variable(char *varname);
void ast_add_expr3_discrete_term_functioncall();
void ast_add_expr3_discrete_term_shellecho(char *shellecho);
void ast_display_expr3_discrete_termll_status();
