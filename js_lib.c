#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#ifdef __APPLE__
#include <sys/syslimits.h>
#endif

#ifdef __linux__
#include <linux/limits.h>
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

value_t js_setOption(uint32_t args, environment_t *env) {
	string_t *str;
	value_t v, s;

	if (debug) fprintf(stderr, "funcall : setOption\n");

	s.bits = vt_status;

	v = eval_arg(&args, env);
	str = js_addr(v);

	if(v.type != vt_string) {
		fprintf(stderr, "Error: setOption => expecting string => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	if (!memcmp(str->val, "Debug", 6))
		debug = true;

	if (!memcmp(str->val, "Math", 5))
		mathNums = true;

	abandonValue(v);
	return s.status = OK, s;
}

value_t js_isNaN(uint32_t args, environment_t *env) {
	value_t v, val;

	if (debug) fprintf(stderr, "funcall : isNaN\n");

	v = eval_arg(&args, env);
	val.bits = vt_bool;
	val.boolean = v.type == vt_nan;
	abandonValue(v);
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
	value_t v, t, r;
	int type;

	t = eval_arg(&args, env);
	v = eval_arg(&args, env);

	type = conv2Int(t, true).nval;

	switch (type) {
	case 1:
		r = conv2Str(v, false, false);
		break;
	case 2:
		r = jsonParse(v);
		break;
	default:
		r.bits = vt_status;
		r.status = ERROR_script_unrecognized_function;
		break;
	}

	abandonValue(v);
	return r;
}

value_t js_print(uint32_t args, environment_t *env) {
	value_t s, v;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : Print\n");

	if (args) for(;;) {
		string_t *str;
		v = eval_arg(&args, env);

		if (v.type == vt_endlist)
			break;

		v = conv2Str(v, true, false);
		str = js_addr(v);
		fwrite(str->val, str->len, 1, stdout);
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
	s.bits = vt_status;
	string_t *progstr;
	string_t *namestr;
	symtab_t symbols;
	parseData pd[1];
	uint32_t first;
	firstNode *fn;
	char *string;

	if (debug) fprintf(stderr, "funcall : parseEval\n");

	memset(&symbols, 0, sizeof(symbols));
	symbols.parent = env->closure->symbols;
	symbols.depth = env->closure->symbols->depth + 1;

	memset(pd, 0, sizeof(pd));
	yylex_init(&pd->scanInfo);

	name = eval_arg(&args, env);
	namestr = js_addr(name);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: parseEval => expecting Script Name:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	pd->script = (char *)namestr->val;
	pd->lineNo = 1;

	first = newNode(pd, node_first, sizeof(firstNode) + namestr->len + 1, false);

	fn = (firstNode *)(pd->table + first);
	fn->hdr->aux = strlen(pd->script);
	memcpy (fn->script, namestr->val, namestr->len);
	fn->script[namestr->len] = 0;

	program = eval_arg(&args, env);
	progstr = js_addr(program);

	if (vt_string != program.type) {
		fprintf(stderr, "Error: parseEval => expecting program name string => %s\n", strtype(program.type));
		return s.status = ERROR_script_internal, s;
	}

	string = js_alloc(progstr->len + 2, false);
	memcpy (string, progstr->val, progstr->len);
	string[progstr->len] = 0;
	string[progstr->len + 1] = 0;

	abandonValue(name);
	abandonValue(program);

	buffer = yy_scan_buffer(string, progstr->len + 2, pd->scanInfo);

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

	execScripts(pd->table, pd->tableNext, arguments, &symbols, env);
	abandonValue(arguments);
	return s.status = OK, s;
}

//	load and run a saved script package
//	js_loadScript(scrName, argumentVector);

value_t js_loadScript(uint32_t args, environment_t *env) {
	value_t name, argVector, s;
	string_t *namestr;
	char errmsg[1024];
#ifdef _WIN32
	char fname[MAX_PATH];
#else
	char fname[PATH_MAX];
#endif
	symtab_t symbols;
	FILE *script;
	Node *table;
	long fsize;
	int err;

	memset (&symbols, 0, sizeof(symbols));
	symbols.parent = env->closure->symbols;
	symbols.depth = env->closure->symbols->depth + 1;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : loadScript\n");

	name = eval_arg(&args, env);
	namestr = js_addr(name);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: loadScript => expecting ScriptName:string => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	if (namestr->len > 1023) {
		fprintf(stderr, "Error: loadScript => filename too long (%d > 1023)\n", namestr->len);
		return s.status = ERROR_script_internal, s;
	}

	memcpy(fname, namestr->val, namestr->len);
	fname[namestr->len] = 0;

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

	execScripts(table, fsize / sizeof(Node), argVector, &symbols, env);

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
	value_t arglist, s, result, op;
	array_t *aval;

	arglist = eval_arg(&args, env);
	aval = js_addr(arglist);
	s.bits = vt_status;

	if (arglist.type != vt_array) {
		fprintf(stderr, "Error: miscop => expecting argument array => %s\n", strtype(arglist.type));
		abandonValue(arglist);
		return s.status = ERROR_script_internal, s;
	}

	op = conv2Int(eval_arg(&args, env), true);

	switch (op.nval) {
	case misc_fromCharCode: {
		result = newString(NULL, vec_cnt(aval->valuePtr));
		string_t *str = result.addr;

		for (int idx = 0; idx < str->len; idx++)
			str->val[idx] = conv2Int(aval->valuePtr[idx], false).nval;

		abandonValue(arglist);
		return result;
	}
	case misc_newDate:
		abandonValue(arglist);
		return newDate(aval->valuePtr);
	}

	abandonValue(arglist);
	s.status = ERROR_script_internal;
	return s;
}

value_t js_listFiles(uint32_t args, environment_t *env) {
	value_t s, path, result = newArray(array_value);
	array_t *aval = result.addr;
	string_t *pathstr;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : findFtw\n");

	path = eval_arg(&args, env);
	pathstr = js_addr(path);

	if (vt_string != path.type) {
		fprintf(stderr, "Error: listFiles => expecting path:string => %s\n", strtype(path.type));
		return s.status = ERROR_script_internal, s;
	}

#ifdef _WIN32
	char pattern[MAX_PATH];
	WIN32_FIND_DATA fd[1];
	HANDLE hndl;

	memcpy (pattern, pathstr->val, pathstr->len > MAX_PATH - 2 ? MAX_PATH - 2 : pathstr->len);
	pattern[pathstr->len] = '/';
	pattern[pathstr->len + 1] = '*';
	pattern[pathstr->len + 2] = 0;
	abandonValue(path);

	hndl = FindFirstFile(pattern, fd);

	if (hndl == INVALID_HANDLE_VALUE)
		return s.status = ERROR_endoffile, s;

	do {
		if (strcmp (fd->cFileName, ".") && strcmp (fd->cFileName, "..") )
		  if (~fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			value_t name = newString(fd->cFileName, -1);
			vec_push (aval->valuePtr, name);
			incrRefCnt(name);
		  }
	} while (FindNextFile(hndl, fd));

	FindClose(hndl);
#else
	DIR *dir = opendir ((char *)pathstr->val);
	struct dirent *ent;

	abandonValue(path);

	if (!dir)
		return s.status = ERROR_doesnot_exist, s;

	while ((ent = readdir(dir)))
	  if (ent->d_ino) {
		value_t name = newString(ent->d_name, -1);
		vec_push (aval->valuePtr, name);
		incrRefCnt(name);
	  }

	closedir(dir);
#endif
	return result;
}

