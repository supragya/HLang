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
	F_VARNAME,
	F_FUNCCALL,
	F_SHELLECHO
};

enum returntype{
	R_STRING,
	R_VARNAME,
	R_FUNCTIONCALL,
	R_EXPRESSION
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
	struct functioncallargs *nextarg;
	struct ast_sequential_shellecho *shellecho;
};
struct functioncallargsll{
	struct functioncallargs *data;
	struct functioncallargsll *next;
};

struct ast_sequential_genvardecl{
	struct vardecl_assignmentlist *list;
};
struct vardecl_assignmentlist{
	char *varname;
	char *value;
	struct expr_expression *expr;
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
	struct expr_expression *expr;
};
struct ast_sequential_varassignment{
	struct var_assignments *assignments;
};
enum var_assignmenttype{
	ASSIGN_EXPRESSION,
	ASSIGN_KEYVALPAIRS,
	ASSIGN_PREINCR,
	ASSIGN_POSTINCR,
	ASSIGN_PREDECR,
	ASSIGN_POSTDECR
};
struct var_assignments{
	char *varname;
	enum var_assignmenttype type;
	struct keyvalpairs *keyvalpairs;
	struct expr_expression *expression;

};

struct ast_construct{
	enum ast_constructtype_t ctype;
	union ast_nextconstruct_ptr ptr;
	struct ast_construct *next;
};

struct ast_sequentialnode{
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
	struct expr_expression *expr;
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
	enum expr_unary_preceder_type precedertype;
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
	struct expr_expression *data;
	struct expr_expression1ll *next;
};
struct expr_expression2ll{
	struct expr_expression2 *data;
	struct expr_expression2ll *next;

};
struct expr_expression3ll{
	struct expr_expression3 *data;
	struct expr_expression3ll *next;

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
enum condition1type{
	COND1_NONE,
	COND1_LOR
};
enum condition2type{
	COND2_NONE,
	COND2_LAND
};
enum condition3type{
	COND3_COND,
	COND3_COMP,
	COND3_COMP_REL_COMP
};
enum conditioncomponenttype{
	COMP_FUNC,
	COMP_SHELLECHO,
	COMP_VARNAME,
	COMP_STR
};
enum negation{
	NEG_YES,
	NEG_NO
};
enum relopr{
	REL_EQ,
	REL_NQ,
	REL_LT,
	REL_GT,
	REL_LE,
	REL_GE
};
struct condition1{
	enum condition1type type;
	struct condition1 *cond1;
	struct condition2 *cond2;
};
struct condition2{
	enum condition2type type;
	struct condition2 *cond2;
	struct condition3 *cond3;
};
struct condition3{
	enum negation neg;
	enum condition3type type;
	struct condition1 *cond1;
	struct conditioncomponent *component1, *component2;
	enum relopr rel;
};
struct conditioncomponent{
	struct ast_sequential_functioncall *func;
	struct ast_sequential_shellecho *shellecho;
	char *name;
	enum conditioncomponenttype type;
};

struct condition1ll{
	struct condition1 *data;
	struct condition1ll *next;
};
struct condition2ll{
	struct condition2 *data;
	struct condition2ll *next;
};
struct condition3ll{
	struct condition3 *data;
	struct condition3ll *next;
};
struct conditioncomponentll{
	struct conditioncomponent *data;
	struct conditioncomponentll *next;
};
struct reloprll{
	enum relopr rel;
	struct reloprll *next;
};
struct conditionnegationll{
	enum negation neg;
	struct conditionnegationll *next;
};

struct ast_root_node *rootnode;
struct ast_construct *currentconstructhead;
struct ast_sequentialnode *currentsequentialhead;
struct keyvalpairs *currentkeyvalpairshead;
struct mapvarlist *currentmapvarlisthead;
struct vardecl_assignmentlist *currentvardeclassignmentlisthead;
struct functioncallargsll *currentfunccallargshead;
struct functioncallargs *currentfunccallargsdiscretehead;
struct returnval *currentreturnvalhead;
struct expr_expression1ll *currentexpression1head;
struct expr_expression2ll *currentexpression2head;
struct expr_expression3ll *currentexpression3head;
struct expr_discrete_termll *currentexpression3discretermhead;
struct expr_unary_precederll *currentexpression3unaryprecederhead;
struct expr_successorll *currentexpression3successorhead;
struct var_assignments *currentvarassignmentshead;
struct condition1ll *currentcondition1head;
struct condition2ll *currentcondition2head;
struct condition3ll *currentcondition3head;
struct conditioncomponentll *currentconditioncomponenthead;
struct reloprll *currentreloprhead;
struct conditionnegationll *currentconditionnegationhead;

int ast_init();
void ast_add_function(char *functionname);
void ast_add_seq();
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
void ast_add_arguments_shellecho(char *echo);
void ast_add_arguments_functioncall(char *functionname);
void ast_add_seq_functioncall(char *functionname);
void ast_set_returnval_expression();
void ast_add_seq_return();
void ast_add_expr3_discrete_term_string(char *str);
void ast_add_expr3_discrete_term_variable(char *varname);
void ast_add_expr3_discrete_term_functioncall();
void ast_add_expr3_discrete_term_shellecho(char *shellecho);
void ast_display_expr3_discrete_termll_status();
void ast_add_expr3_unprecdiscrsucc();
void ast_add_expr3_unprec(enum expr_unary_preceder_type type);
void ast_display_expr3_unary_precll_status();
void ast_display_exprll_status();
void ast_add_expr_expr2();
void ast_add_expr_op_expr2(enum expr_expression1_operation op);
void ast_add_expr2_expr3();
void ast_add_expr2_op_expr3(enum expr_expression2_operation op);
void ast_add_expr3_unprecexprsucc();
void ast_add_varassignment_expr(char *varname);
void ast_add_varassignment_keyvalpairs(char *varname);
void ast_add_varassignmenttype(char *varname, enum var_assignmenttype type);
void ast_add_seq_varassignment();
void ast_add_condition_relopr(enum relopr rel);
void ast_add_condition_unary(enum negation neg);
void ast_add_condition_component_string(char *str);
void ast_add_condition_component_varname(char *varname);
void ast_add_condition_component_functioncall(char *funcname);
void ast_add_condition_component_shellecho(char *echo);
void ast_add_discrete_condition_comp();
void ast_add_discrete_condition_comp_rel_comp();
void ast_add_discrete_condition_unarycondition();
void ast_add_condition1_condition2();
void ast_add_condition2_condition3();
void ast_add_condition1_lor_condition2();
void ast_add_condition2_land_condition3();
void ast_display_condll_stauts();
void ast_add_expr3_discrete_term_functioncall(char *functionname);
void ast_add_expr3_discrete_term_shellecho(char *echo);
void ast_add_argument_to_llnode();
void ast_add_argument_to_llhead();
void ast_display_funcll_status();
