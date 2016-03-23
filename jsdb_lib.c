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

Status jsdb_loadScript(uint32_t args, environment_t *env) {
	value_t v, name, *slot;
	fcnDeclNode *fcn;
    char fname[1024];
	FILE *script;
	Node *table;
	long fsize;

	if (debug) fprintf(stderr, "funcall : loadScript\n");

	name = eval_arg(&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: loadScript => expecting ScriptName:string => %s\n", strtype(name.type));
		return ERROR_script_internal;
	}

    if (name.aux > 1023) {
        fprintf(stderr, "Error: openFile => filename too long (%d > 1023)\n", name.aux);
        return ERROR_script_internal;
    }

    strncpy(fname, (char *)name.str, name.aux);

	v = eval_arg(&args, env);
	slot = v.ref;

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: loadScript => expecting ref => %s\n", strtype(v.type));
		return ERROR_script_internal;
	}

	script = fopen(fname, "rb");
	fseek(script, 0, SEEK_END);
	fsize = ftell(script);

	fseek(script, 0, SEEK_SET);
	table = malloc(fsize / sizeof(Node));
	fread(table, sizeof(Node), fsize / sizeof(Node), script);

	memset(env, 0, sizeof(environment_t));
	fcn = (fcnDeclNode *)(table);

	env->framev = NULL;
	env->table = table;

	v = dispatch(fcn->body, env);

	replaceSlotValue(slot, &v);
	return OK;
}

