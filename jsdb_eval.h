#include "jsdb.h"

value_t eval_endlist(Node *a, environment_t *env);
value_t eval_math(Node *a, environment_t *env);
value_t eval_neg(Node *a, environment_t *env);         // - expr
value_t eval_num(Node *a, environment_t *env);         // number
value_t eval_int(Node *a, environment_t *env);         // integer
value_t eval_string(Node *a, environment_t *env);      // string
value_t eval_fcncall(Node *a, environment_t *env);     // func( exprlist )
value_t eval_fcnexpr(Node *a, environment_t *env);     // func( exprlist )
value_t eval_builtin(Node *a, environment_t *env);     // builtinfunc( exprlist )
value_t eval_var(Node *a, environment_t *env);         // symbol
value_t eval_ref(Node *a, environment_t *env);         // &symbol
value_t eval_assign(Node *a, environment_t *env);      // lval (+|-)= rval
value_t eval_return(Node *a, environment_t *env);      // return stmt
value_t eval_ifthen(Node *a, environment_t *env);      // if (c) {..} else {..}
value_t eval_while(Node *a, environment_t *env);       // while (c) do {..}
value_t eval_dowhile(Node *a, environment_t *env);     // do {..} while (c)
value_t eval_for(Node *a, environment_t *env);         // for (e; c; e) {..}
value_t eval_list(Node *a, environment_t *env);        // {expr,decl,arg}list
value_t eval_elemlist(Node *a, environment_t *env);    // elem, elem, ...
value_t eval_array(Node *a, environment_t *env);       // [ a, b, ... ]
value_t eval_obj(Node *a, environment_t *env);         // { elem, elem, ... }
value_t eval_fcnexpr(Node *a, environment_t *env);     // fcn ( paramlist )
value_t eval_fcndef(Node *a, environment_t *env);      // function f(.) { .. }
value_t eval_elem(Node *a, environment_t *env);        // name : value
value_t eval_lookup(Node *a, environment_t *env);      // x[1] or x["b"]
