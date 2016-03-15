#include "jsdb.h"

static bool debug = false;

Status jsdb_getObject(uint32_t args, environment_t *env) {
	value_t o, v, *slot;
	int i;

	if (debug) fprintf(stderr, "funcall : getObject\n");

	o = eval_arg(&args, env);

	if (vt_object != o.type) {
		fprintf(stderr, "Error: getObject => expecting object => %s\n", strtype(o.type));
		return ERROR_script_internal;
	}

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: getObject => expecting ref => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	slot = v.ref;
	v = newArray();

	if (o.type == vt_object)
		v.aval->array = o.oval->names;
	else
		for (i = 0; i < vec_count(o.oval->names); i++)
			vec_push(v.aval->array, o.oval->names[i]);

	replaceSlotValue(slot, &v);

	v = eval_arg(&args, env);

	if (!(v.type))
		return OK;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: getObject => expecting ref => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	slot = v.ref;

	v = newArray();

	if (o.type == vt_object)
		v.aval->array = o.oval->values;
	else
	  for (i = 0; i < vec_count(o.oval->values); i++)
		vec_push(v.aval->array, o.oval->values[i]);

	replaceSlotValue(slot, &v);

	v = eval_arg(&args, env);
	if (!v.type)
		return OK;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: getObject => expecting ref => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	slot = v.ref;

	v.bits = vt_int;
	v.nval = vec_count(o.oval->names);

	replaceSlotValue(slot, &v);

	return OK;
}

Status jsdb_print(uint32_t args, environment_t *env) {
	value_t v;

	if (debug) fprintf(stderr, "funcall : Print\n");

	if (args) for(;;) {
		value_t v = eval_arg(&args, env);
		if (v.type == vt_endlist)
			break;
		printValue(v, 0);
	}

	printf("\n");
	return OK;
}

Status jsdb_exit(uint32_t args, environment_t *env) {
	if (debug) fprintf(stderr, "funcall : Exit\n");
	exit(0);
}

Status jsdb_makeWeakRef(uint32_t args, environment_t *env) {
	value_t o, v, *slot;

	if (debug) fprintf(stderr, "funcall : makeWeakRef\n");

	o = eval_arg(&args, env);

	if (vt_object != o.type && vt_array != o.type) {
		fprintf(stderr, "Error: makeWeakRef => expecting object => %s\n", strtype(o.type));
		return ERROR_script_internal;
	}

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: makeWeakRef => expecting reference => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	slot = v.ref;

	v.bits = vt_weakref;
	v.weakcount = 1;
	v.raw = o.raw;

	incr_ref_cnt(v);
	replaceSlotValue(slot, &v);
	return OK;
}

Status jsdb_getTokens(uint32_t args, environment_t *env) {
	value_t s, a, d, t, v, *slot;
	int idx, delim, prev = 0;

	if (debug) fprintf(stderr, "funcall : getTokens\n");

	s = eval_arg(&args, env);

	if (vt_string != s.type) {
		fprintf(stderr, "Error: getTokens => expecting string => %s\n", strtype(s.type));
		return ERROR_script_internal;
	}

	d = eval_arg(&args, env);

	if (vt_string != d.type) {
		fprintf(stderr, "Error: getObject => expecting Delimeter::string => %s\n", strtype(d.type));
		return ERROR_script_internal;
	}

	if (d.aux)
		delim = d.str[0];
	else
		return ERROR_script_internal;

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: getObject => expecting ref => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	slot = v.ref;

	a = newArray();

	for (idx = 0; idx < s.aux; idx++) {
		if (s.str[idx] == delim) {
			v = newString(s.str + prev, idx - prev);
			vec_push(a.aval->array, v);
			prev = idx + 1;
		}
	}

	if (prev < idx) {
		v = newString(s.str + prev, idx - prev);
		vec_push(a.aval->array, v);
	}

	replaceSlotValue(slot, &a);
	return OK;
}
