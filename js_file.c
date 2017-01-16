#define _GNU_SOURCE

#include "js.h"
#include <errno.h>

#ifndef _WIN32
#define fopen_s(file, path, mode) ((*file = fopen(path, mode)) ? 0 : errno)
#define freopen_s(dummy, path, mode, file) (((*dummy) = freopen(path, mode,file)) ? 0 : errno)
#define strerror_s(buf,siz,err) (strerror_r(err,buf,siz))
#endif

Status bson_read(FILE *file, int len, int *amt, value_t *result);
Status bson_response (FILE *file, uint32_t request, uint32_t response, uint32_t flags, uint64_t cursorId, uint32_t opcode, uint32_t start, array_t *docs);

value_t js_open(uint32_t args, environment_t *env) {
	value_t v, name, slot, s;
	string_t *namestr;
	char errmsg[1024];
	char fname[1024];
	FILE *file;
	int err;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : Open\n");

	slot = eval_arg(&args, env);

	if (vt_lval != slot.type) {
		fprintf(stderr, "Error: openFile => expecting file:Symbol => %s\n", strtype(slot.type));
		return s.status = ERROR_script_internal, s;
	}

	name = eval_arg(&args, env);
	namestr = js_addr(name);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: openFile => expecting fname:String => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	if (namestr->len > 1023) {
		fprintf(stderr, "Error: openFile => filename too long (%d > 1023)\n", namestr->len);
		return s.status = ERROR_script_internal, s;
	}

	memcpy(fname, namestr->val, namestr->len);
	fname[namestr->len] = 0;

	if((err = fopen_s(&file, fname, "rb"))) {
		strerror_s(errmsg, sizeof(errmsg), err);
		fprintf(stderr, "Error: openFile => fopen failed on '%s' with %s\n", fname, errmsg);
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_file;
	v.file = file;
	replaceValue(slot, v);
	return s.status = OK, s;
}

value_t js_close(uint32_t args, environment_t *env) {
	value_t v, s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : Close\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: closeFile => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	fclose(v.file);
	return s.status = OK, s;
}

// ReadInt32(FILE, dest, ...)

value_t js_readInt32(uint32_t args, environment_t *env) {
	value_t v, dest, s;
	uint32_t val;
	FILE *file;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : ReadInt32\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: readInt32 => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	while( (dest = eval_arg(&args, env), dest.type != vt_endlist)) {
		if (vt_lval != dest.type) {
			fprintf(stderr, "Error: readInt32 => expecting Symbol => %s\n", strtype(dest.type));
			return s.status = ERROR_script_internal, s;
		}

		val = 0;

		if (fread (&val, sizeof(uint32_t), 1, file) < 1)
			return s.status = ERROR_endoffile, s;

		v.bits = vt_int;
		v.nval = val;

		replaceValue(dest, v);
	}

	return s.status = OK, s;
}

// ReadInt64(FILE, dest, ...)

value_t js_readInt64(uint32_t args, environment_t *env) {
	value_t v, dest, s;
	uint64_t val;
	FILE *file;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : ReadInt64\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: readInt64 => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	while( (dest = eval_arg(&args, env), dest.type != vt_endlist)) {
		if (vt_lval != dest.type) {
			fprintf(stderr, "Error: readInt64 => expecting Symbol => %s\n", strtype(dest.type));
			return s.status = ERROR_script_internal, s;
		}

		val = 0;

		if (fread (&val, sizeof(uint64_t), 1, file) < 1)
			return s.status = ERROR_endoffile, s;

		v.bits = vt_int;
		v.nval = val;

		replaceValue(dest, v);
	}

	return s.status = OK, s;
}

// ReadString(FILE, dest)

value_t js_readString(uint32_t args, environment_t *env) {
	value_t v, dest, s;
	char val[4096];
	int size, ch;
	FILE *file;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : ReadString\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: readString => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	dest = eval_arg(&args, env);

	if (vt_lval != dest.type) {
		fprintf(stderr, "Error: readString => expecting Symbol => %s\n", strtype(dest.type));
		return s.status = ERROR_script_internal, s;
	}

	size = 0;

	while ((ch = fgetc(file)) > 0 )
		if (size < sizeof(val) )
			val[size++] = ch;

	v = newString(val, size);
	replaceValue(dest, v);
	return s.status = OK, s;
}

//  read BSON document

value_t js_readBSON(uint32_t args, environment_t *env) {
	int size, max, len, total = 0;
	value_t v, dest, dest2, array;
	array_t *aval;
	Status stat;
	FILE *file;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : ReadBSON\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: readBSON => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	dest = eval_arg(&args, env);

	if (vt_lval != dest.type) {
		fprintf(stderr, "Error: readBSON => expecting Symbol => %s\n", strtype(dest.type));
		return s.status = ERROR_script_internal, s;
	}

	v = eval_arg(&args, env);
	max = conv2Int(v, true).nval;

	dest2 = eval_arg(&args, env);

	if (vt_lval != dest2.type) {
		fprintf(stderr, "Error: readBSON => expecting size:ref => %s\n", strtype(dest2.type));
		return s.status = ERROR_script_internal, s;
	}

	array = newArray(array_value);
	aval = array.addr;
	total = 0;

	do {
	  if (fread (&len, sizeof(uint32_t), 1, file) < 1)
		return s.status = ERROR_endoffile, s;

	  size = sizeof(uint32_t);

	  if (len > max)
		return s.status = ERROR_endoffile, s;

	  if ((stat = bson_read (file, len - sizeof(uint32_t), &size, &v)))
		return s.status = stat, s;

	  if (size > 5) {
		incrRefCnt(v);
		vec_push (aval->valuePtr, v);
	  }

	  total += size;
	} while ((max -= size));

	replaceValue(dest, array);

	v.bits = vt_int;
	v.nval = total;
	replaceValue(dest2, v);

	return s.status = OK, s;
}


//  respond with BSON documents
//  bson_response(file, request, response, flags, cursorId, opcode, start, docs)

value_t js_response(uint32_t args, environment_t *env) {
	uint32_t request, response, flags, start;
	uint64_t cursorId;
	uint32_t opcode;
	value_t v, array;
	array_t *aval;
	Status stat;
	FILE *file;
	value_t s;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : response\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: response => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	v = eval_arg(&args, env);
	request = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	response = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	flags = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	cursorId = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	opcode = conv2Int(v, true).nval;

	v = eval_arg(&args, env);
	start = conv2Int(v, true).nval;

	array = eval_arg(&args, env);
	aval = js_addr(array);

	if (vt_array != array.type) {
		fprintf(stderr, "Error: docs => expecting array => %s\n", strtype(array.type));
		return s.status = ERROR_script_internal, s;
	}

	stat = bson_response(file, request, response, flags, cursorId, opcode, start, aval);
	abandonValue(array);

	return s.status = stat, s;
}
