value_t eval_endlist(Node *a, environment_t *env);
value_t eval_math(Node *a, environment_t *env);
value_t eval_neg(Node *a, environment_t *env);         // - expr
value_t eval_fcncall(Node *a, environment_t *env);     // func( exprlist )
value_t eval_fcnexpr(Node *a, environment_t *env);     // func( exprlist )
value_t eval_builtin(Node *a, environment_t *env);     // builtinfunc( exprlist )
value_t eval_assign(Node *a, environment_t *env);      // lval (+|-)= rval
value_t eval_fcnexpr(Node *a, environment_t *env);     // fcn ( paramlist )
value_t eval_fcndef(Node *a, environment_t *env);      // function f(.) { .. }
