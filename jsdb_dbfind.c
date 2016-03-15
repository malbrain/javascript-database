#include "jsdb.h"
#include "jsdb_eval.h"

bool debug = false;
bool init = true;

typedef bool (*queryFcn)(value_t r, value_t tst);

bool op_lt (value_t left, value_t right);
bool op_le (value_t left, value_t right);
bool op_eq (value_t left, value_t right);
bool op_ne (value_t left, value_t right);
bool op_ge (value_t left, value_t right);
bool op_gt (value_t left, value_t right);

queryFcn queryit1[43];
queryFcn queryit2[11];

uint64_t query_hash(uint8_t *str, int len) {
    uint64_t hash = 0;
    uint64_t mask;

    if (!len)
        len = strlen(str);

    while (len>=8) {
        len -= 8;
        hash += 7 * (*((uint64_t *) &str[len]));
        hash *= 5;
    }

    mask = 1ULL << len * 8;
    return hash += 7 * (--mask & (*((uint64_t *) &str[0])));
}

bool op_in (value_t slot, value_t e) {
    int idx;

    for (idx = 0; idx < vec_count(e.aval->array); idx++)
        if (op_eq (slot, (e.aval->array)[idx]))
            return true;

    return false;
}

bool op_exists (value_t slot, value_t e) {
    return slot.type != vt_uninitialized;
}

bool op_nin (value_t slot, value_t e) {
    int idx;

    for (idx = 0; idx < vec_count(e.aval->array); idx++)
        if (op_eq (slot, (e.aval->array)[idx]))
            return false;

    return true;
}

bool query_expr (value_t slot, value_t e) {
    bool accept = true;
    int i;

    if (e.type != vt_object)
        return op_eq(slot, e);

    for (i = 0; accept && i < vec_count(e.oval->names); i++) {
        uint8_t hash = query_hash(e.oval->names[i].str, e.oval->names[i].aux) % 43;
        accept = (queryit1[hash])(slot, e.oval->values[i]);
    }

    return accept;
}
 
value_t query_lookup(value_t obj, value_t field) {
    value_t name, *result = NULL, v;
    int idx, prev = 0;

    name.bits = vt_string;
    v.bits = vt_uninitialized;

    for (idx = 0; idx < field.aux; idx++) {
        if (field.str[idx] == '.') {
            name.aux = idx - prev;
            name.str = field.str + prev;
            if (obj.type == vt_object)
              if ((result = lookup (obj, name, false)))
                obj = *result;
              else
                return v;

            if (obj.type == vt_document) {
              obj = lookupDoc (obj, name);
              if (obj.type == vt_uninitialized)
                return obj;
            }
            prev = idx + 1;
        }
    }

    if (prev < idx) {
        name.aux = idx - prev;
        name.str = field.str + prev;
        if (obj.type == vt_object)
            if(( result = lookup (obj, name, false)))
                return *result;
            else
                return v;

        if (obj.type == vt_document)
            return lookupDoc(obj, name);
    }

    return obj;
}

bool query_eval (value_t q, value_t r) {
    bool accept = true;
    int idx, i;

    if (q.type == vt_document)
      for (idx = 0; accept && idx < q.document->count; idx++) {
        value_t e = q.document->names[idx + q.document->count];
        value_t f = q.document->names[idx];

        // logical operators

        if (f.str[0] == '$') {
           uint8_t hash = query_hash(f.str, f.aux) % 11;
           accept = (queryit2[hash])(r, e);
           continue;
        }

        value_t slot = query_lookup (r, f);
        accept = query_expr(slot, e);
    }

    if (q.type == vt_object)
      for (idx = 0; accept && idx < vec_count (q.oval->names); idx++) {
        value_t e = q.oval->values[idx];
        value_t f = q.oval->names[idx];

        // logical operators

        if (f.str[0] == '$') {
           uint8_t hash = query_hash(f.str, f.aux) % 11;
           accept = (queryit2[hash])(r, e);
           continue;
        }

        value_t slot = query_lookup (r, f);
        accept = query_expr(slot, e);
    }

    return accept;
}

bool op_not (value_t slot, value_t e) {
    return !query_expr (slot, e);
}

bool op_err1 ( value_t r, value_t a) {
    return true;
}

bool op_err2 ( value_t r, value_t a) {
    return true;
}

bool op_and (value_t r, value_t a) {
    int idx;

    for (idx = 0; idx < vec_count(a.aval->array); idx++)
        if (!query_eval ((a.aval->array)[idx], r))
            return false;

    return true;
}

bool op_or (value_t r, value_t a) {
    int idx;

    for (idx = 0; idx < vec_count(a.aval->array); idx++)
        if (query_eval ((a.aval->array)[idx], r))
            return true;

    return false;
}

bool op_nor (value_t r, value_t a) {
    int idx;

    for (idx = 0; idx < vec_count(a.aval->array); idx++)
        if (query_eval ((a.aval->array)[idx], r))
            return false;

    return true;
}

typedef struct {
    queryFcn fcn;
    char *op;
} queryTable1;

typedef struct {
    queryFcn fcn;
    char *op;
} queryTable2;

queryTable1 qryTab1[] = {
{ op_eq, "$eq" }, 
{ op_ne, "$ne" }, 
{ op_gt, "$gt" }, 
{ op_ge, "$gte" }, 
{ op_lt, "$lt" }, 
{ op_le, "$lte" }, 
{ op_in, "$in" }, 
{ op_nin, "$nin" }, 
{ op_not, "$not" }, 
{ op_exists, "$exists" }
};

queryTable2 qryTab2[] = {
{ op_and, "$and" },
{ op_nor, "$nor" },
{ op_or, "$or" } 
};

void query_install ()
{
    int idx;

    for (idx = 0; idx < 43; idx++)
        queryit1[idx] = op_err1;

    for (idx = 0; idx < 11; idx++)
        queryit2[idx] = op_err2;

    for (idx = 0; idx < sizeof(qryTab1) / sizeof(queryTable1); idx++) {
        uint8_t hash = query_hash(qryTab1[idx].op, 0) % 43;
        queryit1[hash] = qryTab1[idx].fcn;
    }

    for (idx = 0; idx < sizeof(qryTab2) / sizeof(queryTable2); idx++) {
        uint8_t hash = query_hash(qryTab2[idx].op, 0) % 11;
        queryit2[hash] = qryTab2[idx].fcn;
    }
}

// find (query, record, result)
// result: record satisfies query

Status jsdb_findDocs(uint32_t args, environment_t *env) {
    value_t q, r, v, *result;
    bool accept = true;
    int idx, i;

    if (init) {
        init = 0;
        query_install();
    }

    if (debug) fprintf(stderr, "funcall : find\n");

    // first arg is the query document (object)

	q = eval_arg(&args, env);

    if (vt_object != q.type && vt_document != q.type) {
        fprintf(stderr, "Error: find => expecting query object => %s  Line: %d\n", strtype(q.type), __LINE__);
        return ERROR_script_internal;
    }

    // second arg is the database document (object)

	r = eval_arg(&args, env);

    if (vt_object != r.type && vt_document != r.type) {
        fprintf(stderr, "Error: expecting database object => %s  Line: %d\n", strtype(r.type), __LINE__);
        return ERROR_script_internal;
    }

    // third arg is the result value

	v = eval_arg(&args, env);

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: expecting result:reference => %s  Line: %d\n", strtype(v.type), __LINE__);
        return ERROR_script_internal;
    }

    result = v.ref;

    v.bits = vt_bool;
    v.boolean = query_eval(q,r);
    replaceSlotValue(result, &v);
    return OK;
}

