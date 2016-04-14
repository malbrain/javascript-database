#include "jsdb.h"

static bool debug = false;

value_t jsdb_parseInt(uint32_t args, environment_t *env) {
	value_t v, val;

	if (debug) fprintf(stderr, "funcall : parseInt\n");

	v = eval_arg(&args, env);
	val = conv2Int(v, true);
	return val;
}

value_t jsdb_parseFlt(uint32_t args, environment_t *env) {
	value_t v, val;

	if (debug) fprintf(stderr, "funcall : parseFloat\n");

	v = eval_arg(&args, env);
	val = conv2Dbl(v, true);
	return val;
}

value_t jsdb_getObject(uint32_t args, environment_t *env) {
	value_t o, v, slot, s;
	int i;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : getObject\n");

	o = eval_arg(&args, env);

	if (vt_object != o.type) {
		fprintf(stderr, "Error: getObject => expecting object => %s\n", strtype(o.type));
		return s.status = ERROR_script_internal, s;
	}

	slot = eval_arg(&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: getObject => expecting ref => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	v = newArray(array_value);

	v.aval->values = vec_dup (vec_raw(o.oval->names));
	replaceValue(slot, v);

	slot = eval_arg(&args, env);

	if (!(slot.type))
		return s.status = OK, s;

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: getObject => expecting ref => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	v = newArray(array_value);

	v.aval->values = vec_dup (vec_raw(o.oval->values));
	replaceValue(slot, v);

	abandonValue(o);
	return s.status = OK, s;
}

value_t jsdb_print(uint32_t args, environment_t *env) {
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : Print\n");

	if (args) for(;;) {
		value_t v = eval_arg(&args, env);
		value_t *array = NULL;

		if (v.type == vt_endlist)
			break;

		value2Str(v, &array, 1);

		for (int idx = 0; idx < vec_count(array); idx++) {
			fwrite(array[idx].str, array[idx].aux, 1, stdout);
			abandonValue(array[idx]);
		}

		vec_free(array);
	}

	printf("\n");
	return s.status = OK, s;
}

value_t jsdb_quit(uint32_t args, environment_t *env) {
	if (debug) fprintf(stderr, "funcall : Exit\n");
	exit(0);
}

value_t jsdb_makeWeakRef(uint32_t args, environment_t *env) {
	value_t o, v, slot, s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : makeWeakRef\n");

	o = eval_arg(&args, env);

	if (vt_object != o.type && vt_array != o.type) {
		fprintf(stderr, "Error: makeWeakRef => expecting object => %s\n", strtype(o.type));
		return s.status = ERROR_script_internal, s;
	}

	slot = eval_arg(&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: makeWeakRef => expecting reference => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_weakref;
	v.weakcount = 1;
	v.raw = o.raw;

	incrRefCnt(v);
	replaceValue(slot, v);
	abandonValue(o);
	return s.status = OK, s;
}

value_t jsdb_loadScript(uint32_t args, environment_t *env) {
	value_t v, name, slot, s;
	fcnDeclNode *fcn;
	char fname[1024];
	FILE *script;
	Node *table;
	long fsize;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : loadScript\n");

	name = eval_arg(&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: loadScript => expecting ScriptName:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	if (name.aux > 1023) {
		fprintf(stderr, "Error: openFile => filename too long (%d > 1023)\n", name.aux);
		return s.status = ERROR_script_internal, s;
	}

	strncpy(fname, (char *)name.str, name.aux);

	slot = eval_arg(&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: loadScript => expecting ref => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
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

	replaceValue(slot, v);
	abandonValue(name);
	return s.status = OK, s;
}

value_t jsdb_eval(uint32_t args, environment_t *env) {
	value_t script, s;

	if (debug) fprintf(stderr, "funcall : loadScript\n");

	script = eval_arg(&args, env);

	if (vt_string != script.type) {
		fprintf(stderr, "Error: eval => expecting Script:string => %s\n", strtype(script.type));
		return s.status = ERROR_script_internal, s;
	}

	return s.status = OK, s;
}
