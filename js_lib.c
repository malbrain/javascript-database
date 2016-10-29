#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#ifdef __APPLE__
#include <sys/syslimits.h>
#endif

#include "js.h"

static bool debug = false;

value_t js_isNaN(uint32_t args, environment_t *env) {
	value_t v, val;

	if (debug) fprintf(stderr, "funcall : parseInt\n");

	v = eval_arg(&args, env);
	val.bits = vt_bool;
	val.boolean = v.type == vt_nan;
	return val;
}

value_t js_parseInt(uint32_t args, environment_t *env) {
	value_t v, val;

	if (debug) fprintf(stderr, "funcall : parseInt\n");

	v = eval_arg(&args, env);
	val = conv2Int(v, true);
	return val;
}

value_t js_parseFlt(uint32_t args, environment_t *env) {
	value_t v, val;

	if (debug) fprintf(stderr, "funcall : parseFloat\n");

	v = eval_arg(&args, env);
	val = conv2Dbl(v, true);
	return val;
}

value_t js_print(uint32_t args, environment_t *env) {
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

value_t js_quit(uint32_t args, environment_t *env) {
	if (debug) fprintf(stderr, "funcall : Exit\n");
	exit(0);
}

value_t js_makeWeakRef(uint32_t args, environment_t *env) {
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

value_t js_loadScript(uint32_t args, environment_t *env) {
	value_t v, name, slot, s;
#ifdef _WIN32
	char fname[MAX_PATH];
#else
	char fname[PATH_MAX];
#endif
	fcnDeclNode *fcn;
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

	memcpy(fname, name.str, name.aux);
	fname[name.aux] = 0;

	slot = eval_arg(&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: loadScript => expecting ref => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	script = fopen(fname, "rb");
	fseek(script, 0, SEEK_END);
	fsize = ftell(script);

	fseek(script, 0, SEEK_SET);
	table = js_alloc(fsize / sizeof(Node), false);
	fread(table, sizeof(Node), fsize / sizeof(Node), script);

	memset(env, 0, sizeof(environment_t));
	fcn = (fcnDeclNode *)(table);

	env->table = table;

	v = dispatch(fcn->body, env);

	replaceValue(slot, v);
	abandonValue(name);
	return s.status = OK, s;
}

value_t js_eval(uint32_t args, environment_t *env) {
	value_t script, s;

	if (debug) fprintf(stderr, "funcall : loadScript\n");

	script = eval_arg(&args, env);

	if (vt_string != script.type) {
		fprintf(stderr, "Error: eval => expecting Script:string => %s\n", strtype(script.type));
		return s.status = ERROR_script_internal, s;
	}

	return s.status = OK, s;
}

enum MiscEnum {
	misc_fromCharCode,
	misc_newDate
};

extern value_t newDate(value_t *args);

value_t js_miscop (uint32_t args, environment_t *env) {
	value_t arglist, s, result;

	arglist = eval_arg(&args, env);
	s.bits = vt_status;

	if (arglist.type != vt_array) {
		fprintf(stderr, "Error: miscop => expecting argument array => %s\n", strtype(arglist.type));
		return s.status = ERROR_script_internal, s;
	}

	switch (eval_arg(&args, env).nval) {
	case misc_fromCharCode: {
		result.bits = vt_string;
		result.aux = vec_count(arglist.aval->values);
		result.str = js_alloc(result.aux + 1, false);
		result.str[result.aux] = 0;
		result.refcount = 1;

		for (int idx = 0; idx < result.aux; idx++)
			result.str[idx] = conv2Int(arglist.aval->values[idx], false).nval;

		return result;
	}
	case misc_newDate:
		return newDate(arglist.aval->values);
	}

	s.status = ERROR_script_internal;
	return s;
}

value_t js_listFiles(uint32_t args, environment_t *env) {
	value_t s, path, result = newArray(array_value);

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : findFtw\n");

	path = eval_arg(&args, env);

	if (vt_string != path.type) {
		fprintf(stderr, "Error: listFiles => expecting path:string => %s\n", strtype(path.type));
		return s.status = ERROR_script_internal, s;
	}

#ifdef _WIN32
	char pattern[MAX_PATH];
	WIN32_FIND_DATA fd[1];
	HANDLE hndl;

	memcpy (pattern, path.str, path.aux > MAX_PATH - 2 ? MAX_PATH - 2 : path.aux);
	pattern[path.aux] = '/';
	pattern[path.aux+1] = '*';
	pattern[path.aux+2] = 0;

	hndl = FindFirstFile(pattern, fd);

	if (hndl == INVALID_HANDLE_VALUE)
		return s.status = ERROR_endoffile, s;

	do {
		if (strcmp (fd->cFileName, ".") && strcmp (fd->cFileName, "..") )
		  if (~fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			value_t name = newString(fd->cFileName, strlen(fd->cFileName));
			vec_push (result.aval->values, name);
			incrRefCnt(name);
		  }
	} while (FindNextFile(hndl, fd));

	FindClose(hndl);
#else
	DIR *dir = opendir ((char *)path.str);
	struct dirent *ent;

	if (!dir)
		return s.status = ERROR_doesnot_exist, s;

	while ((ent = readdir(dir)))
	  if (ent->d_ino) {
		value_t name = newString(ent->d_name, strlen(ent->d_name));
		vec_push (result.aval->values, name);
		incrRefCnt(name);
	  }

	closedir(dir);
#endif
	return result;
}