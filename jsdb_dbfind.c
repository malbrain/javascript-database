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
	for (int idx = 0; idx < vec_count(e.aval->values); idx++)
		if (op_eq (slot, (e.aval->values)[idx]))
			return true;

	return false;
}

bool op_exists (value_t slot, value_t e) {
	return !(slot.type == vt_null || slot.type == vt_undef);
}

bool op_nin (value_t slot, value_t e) {
	for (int idx = 0; idx < vec_count(e.aval->values); idx++)
		if (op_eq (slot, (e.aval->values)[idx]))
			return false;

	return true;
}

bool query_expr (value_t slot, value_t e) {
	bool accept = true;

	if (e.type != vt_object)
		return op_eq(slot, e);

	for (int i = 0; accept && i < vec_count(e.oval->pairs); i++) {
		uint8_t hash = query_hash(e.oval->pairs[i].name.str, e.oval->pairs[i].name.aux) % 43;
		accept = (queryit1[hash])(slot, e.oval->pairs[i].value);
	}

	return accept;
}
 
value_t query_lookup(value_t obj, value_t field) {
	value_t name, *result = NULL, v;
	int idx, prev = 0;

	name.bits = vt_string;
	v.bits = vt_undef;

	for (idx = 0; idx < field.aux; idx++) {
		if (field.str[idx] == '.') {
			name.aux = idx - prev;
			name.str = field.str + prev;
			if (obj.type == vt_object)
			  if ((result = lookup (obj.oval, name, false)))
				obj = *result;
			  else
				return v;

			if (obj.type == vt_document) {
			  obj = lookupDoc (obj.document, name);
			  if (obj.type == vt_null || obj.type == vt_undef)
				return obj;
			}
			prev = idx + 1;
		}
	}

	if (prev < idx) {
		name.aux = idx - prev;
		name.str = field.str + prev;
		if (obj.type == vt_object)
			if(( result = lookup (obj.oval, name, false)))
				return *result;
			else
				return v;

		if (obj.type == vt_document)
			return lookupDoc(obj.document, name);
	}

	return obj;
}

bool query_eval (value_t q, value_t r) {
	bool accept = true;
	int idx, i;

	if (q.type == vt_document)
	  for (idx = 0; accept && idx < q.document->count; idx++) {
		value_t e = q.document->pairs[idx].value;
		value_t f = q.document->pairs[idx].name;

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
	  for (idx = 0; accept && idx < vec_count (q.oval->pairs); idx++) {
		value_t e = q.oval->pairs[idx].value;
		value_t f = q.oval->pairs[idx].name;

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
	for (int idx = 0; idx < vec_count(a.aval->values); idx++)
		if (!query_eval ((a.aval->values)[idx], r))
			return false;

	return true;
}

bool op_or (value_t r, value_t a) {
	for (int idx = 0; idx < vec_count(a.aval->values); idx++)
		if (query_eval ((a.aval->values)[idx], r))
			return true;

	return false;
}

bool op_nor (value_t r, value_t a) {
	for (int idx = 0; idx < vec_count(a.aval->values); idx++)
		if (query_eval ((a.aval->values)[idx], r))
			return false;

	return true;
}

typedef struct {
	queryFcn fcn;
	char *op;
} queryTable;

queryTable qryTab1[] = {
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

queryTable qryTab2[] = {
{ op_and, "$and" },
{ op_nor, "$nor" },
{ op_or, "$or" } 
};

void query_install ()
{
	for (int idx = 0; idx < 43; idx++)
		queryit1[idx] = op_err1;

	for (int idx = 0; idx < 11; idx++)
		queryit2[idx] = op_err2;

	for (int idx = 0; idx < sizeof(qryTab1) / sizeof(queryTable); idx++) {
		uint8_t hash = query_hash(qryTab1[idx].op, 0) % 43;
		queryit1[hash] = qryTab1[idx].fcn;
	}

	for (int idx = 0; idx < sizeof(qryTab2) / sizeof(queryTable); idx++) {
		uint8_t hash = query_hash(qryTab2[idx].op, 0) % 11;
		queryit2[hash] = qryTab2[idx].fcn;
	}
}

// find (query, record)
// result: record satisfies query

value_t jsdb_findDocs(uint32_t args, environment_t *env) {
	value_t q, r, v, s;
	bool accept = true;
	int idx, i;

	s.bits = vt_status;

	if (init) {
		init = 0;
		query_install();
	}

	if (debug) fprintf(stderr, "funcall : find\n");

	// first arg is the query document (object)

	q = eval_arg(&args, env);

	if (vt_object != q.type && vt_document != q.type) {
		fprintf(stderr, "Error: find => expecting query object => %s  Line: %d\n", strtype(q.type), __LINE__);
		return s.status = ERROR_script_internal, s;
	}

	// second arg is the database document (object)

	r = eval_arg(&args, env);

	if (vt_object != r.type && vt_document != r.type) {
		fprintf(stderr, "Error: expecting database document => %s  File: %s\n", strtype(r.type), __FILE__);
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_bool;
	v.boolean = query_eval(q,r);
	abandonValue(q);
	abandonValue(r);
	return v;
}

