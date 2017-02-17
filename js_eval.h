value_t eval_endlist(Node *a, environment_t *env);
value_t eval_math(Node *a, environment_t *env);
value_t eval_neg(Node *a, environment_t *env);         // - expr
value_t eval_fcncall(Node *a, environment_t *env);     // func( exprlist )
value_t eval_fcnexpr(Node *a, environment_t *env);     // func( exprlist )
value_t eval_builtin(Node *a, environment_t *env);     // builtinfunc( exprlist )
value_t eval_assign(Node *a, environment_t *env);      // lval (+|-)= rval
value_t eval_fcnexpr(Node *a, environment_t *env);     // fcn ( paramlist )
value_t eval_fcndef(Node *a, environment_t *env);      // function f(.) { .. }

value_t eval_block (Node *a, environment_t *env);
value_t eval_arg(uint32_t *args, environment_t *env);
value_t eval_num (Node *a, environment_t *env);
value_t eval_badop (Node *a, environment_t *env);
value_t eval_noop (Node *a, environment_t *env);
value_t eval_access (Node *a, environment_t *env);
value_t eval_lookup (Node *a, environment_t *env);
value_t eval_array (Node *n, environment_t *env);
value_t eval_obj (Node *n, environment_t *env);
value_t eval_list(Node *n, environment_t *env);
value_t eval_var(Node *a, environment_t *env);
value_t eval_string(Node *a, environment_t *env);
value_t eval_while(Node *a, environment_t *env);
value_t eval_dowhile(Node *a, environment_t *env);
value_t eval_ifthen(Node *a, environment_t *env);
value_t eval_return(Node *a, environment_t *env);
value_t eval_typeof(Node *a, environment_t *env);
value_t eval_enum(Node *a, environment_t *env);
value_t eval_incr(Node *a, environment_t *env);
value_t eval_tern(Node *a, environment_t *env);
value_t eval_forin(Node *a, environment_t *env);
value_t eval_for(Node *a, environment_t *env);
value_t eval_lor(Node *a, environment_t *env);
value_t eval_land(Node *a, environment_t *env);
