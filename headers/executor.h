enum solvable{
	SOL_YES,
	SOL_NO
};
struct exec_expr_solution{
	enum solvable solvable;
	char *solution;
};
int exec_begin_execution();
int exec_constructs(struct ast_construct *entry);
struct ast_construct* exec_provide_constructlist_for_function(char *functionname);
int exec_sequential_construct(struct ast_sequentialnode *seq);
int exec_selective_construct(struct ast_selectivenode *seq);
int exec_iterative_construct(struct ast_iterativenode *seq);
int exec_genvardecl(struct ast_sequential_genvardecl *node);
int exec_mapvardecl(struct ast_sequential_mapvardecl *node);
int exec_add_keyval_pairs(char *mapname, struct keyvalpairs *pairs);
int exec_shellecho(struct ast_sequential_shellecho *node);
int exec_varassignment(struct ast_sequential_varassignment *node);
int exec_varassignment_aux1(struct var_assignments *node);
struct exec_expr_solution solve_expression(struct expr_expression *expr);
struct exec_expr_solution solve_expression2(struct expr_expression2 *expr2);
struct exec_expr_solution solve_expression3(struct expr_expression3 *expr3);
struct exec_expr_solution expr_addterms(struct exec_expr_solution t1, struct exec_expr_solution t2);
struct exec_expr_solution expr_subterms(struct exec_expr_solution t1, struct exec_expr_solution t2);
struct exec_expr_solution expr_multerms(struct exec_expr_solution t1, struct exec_expr_solution t2);
struct exec_expr_solution expr_divterms(struct exec_expr_solution t1, struct exec_expr_solution t2);
struct exec_expr_solution expr_truncdivterms(struct exec_expr_solution t1, struct exec_expr_solution t2);
struct exec_expr_solution expr_solve_unprec_expr_succ(struct expr_expression3 *expr3);
struct exec_expr_solution expr_solve_unprec_discr_succ(struct expr_expression3 *expr3);
char *longtostring(long val);
int exec_if(struct ast_ifsel *ifblock);
int exec_elif(struct ast_elifsel *elifsel);
int exec_else(struct ast_elsesel *elsesel);
int solve_condition(struct condition1 *cond);
int solve_condition2(struct condition2 *cond);
int solve_condition3(struct condition3 *cond);
int exec_for_condition(struct conditioncomponent *comp);
