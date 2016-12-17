#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#ifdef __APPLE__
#include <sys/syslimits.h>
#endif

#include <errno.h>
#include "js.h"
#include "js.tab.h"
#include "js.lex.h"

#ifndef _WIN32
#define fopen_s(file, path, mode) ((*file = fopen(path, mode)) ? 0 : errno)
#define freopen_s(dummy, path, mode, file) (((*dummy) = freopen(path, mode,file)) ? 0 : errno)
#define strerror_s(buf,siz,err) (strerror_r(err,buf,siz))
#endif

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

//	json operation 1=stringify, 2=parse

extern value_t jsonParse(value_t v);

value_t js_json(uint32_t args, environment_t *env) {
	if (debug) fprintf(stderr, "funcall : json\n");
	value_t v, t, s;
	int type;

	t = eval_arg(&args, env);
	v = eval_arg(&args, env);

	type = conv2Int(t, true).nval;

	switch (type) {
	case 1:
		return conv2Str(v, true, true);
	case 2:
		return jsonParse(v);
	}

	s.bits = vt_status;
	s.status = ERROR_script_unrecognized_function;
	return s;
}

value_t js_print(uint32_t args, environment_t *env) {
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : Print\n");

	if (args) for(;;) {
		value_t v = eval_arg(&args, env);

		if (v.type == vt_endlist)
			break;

		v = conv2Str(v, true, true);
		fwrite(v.string, v.aux, 1, stdout);
		abandonValue(v);
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

//	parse and eval a javascript program string
//	js_parseEval(fragmentName, programString, argsVector);

value_t js_parseEval(uint32_t args, environment_t *env) {
	value_t program, s, name, arguments;
	YY_BUFFER_STATE buffer;
	symtab_t symbols[1];
	s.bits = vt_status;
	parseData pd[1];
	uint32_t first;
	firstNode *fn;
	char *string;

	if (debug) fprintf(stderr, "funcall : parseEval\n");

	memset(symbols, 0, sizeof(symbols));
	symbols->parent = env->closure->symbols;
	symbols->depth = env->closure->symbols->depth + 1;

	memset(pd, 0, sizeof(pd));
	yylex_init(&pd->scanInfo);

	name = eval_arg(&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: parseEval => expecting Script Name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	pd->script = name.str;
	pd->lineNo = 1;

	first = newNode(pd, node_first, sizeof(firstNode) + name.aux, false);

	fn = (firstNode *)(pd->table + first);
	fn->hdr->aux = strlen(pd->script);
	memcpy (fn->script, name.str, name.aux);
	fn->script[name.aux] = 0;

	program = eval_arg(&args, env);

	if (vt_string != program.type) {
		fprintf(stderr, "Error: parseEval => expecting javascript:string => %s\n", strtype(program.type));
		return s.status = ERROR_script_internal, s;
	}

	string = js_alloc(program.aux + 2, false);
	memcpy (string, program.str, program.aux);
	string[program.aux] = 0;
	string[program.aux + 1] = 0;

	buffer = yy_scan_buffer(string, program.aux + 2, pd->scanInfo);

	switch (yyparse(pd->scanInfo, pd)) {
	  case 1:
		return s.status = ERROR_script_parse, s;
	  case 2:
		return s.status = ERROR_outofmemory, s;
	}

	yy_delete_buffer(buffer, pd->scanInfo);
	yylex_destroy(pd->scanInfo);

	arguments = eval_arg(&args, env);

	fn = (firstNode *)(pd->table + first);
	fn->moduleSize = pd->tableNext - first;
	fn->begin = pd->beginning;

	execScripts(pd->table, pd->tableNext, arguments, symbols, env);
	return s.status = OK, s;
}

//	load and run a saved script package
//	js_loadScript(scrName, argumentVector);

value_t js_loadScript(uint32_t args, environment_t *env) {
	value_t name, argVector, s;
	symtab_t symbols[1];
	char errmsg[1024];
#ifdef _WIN32
	char fname[MAX_PATH];
#else
	char fname[PATH_MAX];
#endif
	FILE *script;
	Node *table;
	long fsize;
	int err;

	memset (symbols, 0, sizeof(symbols));
	symbols->parent = env->closure->symbols;
	symbols->depth = env->closure->symbols->depth + 1;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : loadScript\n");

	name = eval_arg(&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: loadScript => expecting ScriptName:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	if (name.aux > 1023) {
		fprintf(stderr, "Error: loadScript => filename too long (%d > 1023)\n", name.aux);
		return s.status = ERROR_script_internal, s;
	}

	memcpy(fname, name.str, name.aux);
	fname[name.aux] = 0;

	if((err = fopen_s(&script, fname, "rb"))) {
		strerror_s(errmsg, sizeof(errmsg), err);
		fprintf(stderr, "Error: loadScript => fopen failed on '%s' with %s\n", fname, errmsg);
		return s.status = ERROR_script_internal, s;
	}

	fseek(script, 0, SEEK_END);
	fsize = ftell(script);

	fseek(script, 0, SEEK_SET);
	table = js_alloc(fsize, false);
	fread(table, sizeof(Node), fsize / sizeof(Node), script);

	argVector = eval_arg(&args, env);

	execScripts(table, fsize / sizeof(Node), argVector, symbols, env);

	abandonValue(name);
	abandonValue(argVector);
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

