#include "jsdb.h"

static bool debug = false;
Status bson_read(FILE *file, int len, int *amt, value_t *result);
Status bson_response (FILE *file, uint32_t request, uint32_t response, uint32_t flags, uint64_t cursorId, uint32_t opcode, uint32_t start, array_t *docs);

value_t jsdb_open(uint32_t args, environment_t *env) {
	value_t v, name, *slot, s;
	char fname[1024];
	FILE *file;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : Open\n");

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: openFile => expecting file:Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	slot = v.ref;

	name = eval_arg(&args, env);

	if (vt_string != name.type) {
		fprintf(stderr, "Error: openFile => expecting fname:String => %s\n", strtype(name.type));
		return s.status = ERROR_script_internal, s;
	}

	if (name.aux > 1023) {
		fprintf(stderr, "Error: openFile => filename too long (%d > 1023)\n", name.aux);
		return s.status = ERROR_script_internal, s;
	}

	strncpy(fname, (char *)name.str, name.aux);
	fname[name.aux] = 0;
	abandonValue(v);

	file = fopen(fname, "rb");

	if (NULL==file) {
		fprintf(stderr, "Error: openFile => fopen failed on '%s' with %s\n", fname, strerror(errno));
		return s.status = ERROR_script_internal, s;
	}

	v.bits = vt_file;
	v.file = file;
	replaceSlotValue(slot, v);
	return s.status = OK, s;
}

value_t jsdb_close(uint32_t args, environment_t *env) {
	value_t v, s;
	FILE *file;

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

value_t jsdb_readInt32(uint32_t args, environment_t *env) {
	value_t v, *dest, s;
	uint32_t val;
	FILE *file;
	int cmd;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : ReadInt32\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: readInt32 => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	while( (v = eval_arg(&args, env), v.type != vt_endlist)) {
		if (vt_ref != v.type) {
			fprintf(stderr, "Error: readInt32 => expecting Symbol => %s\n", strtype(v.type));
			return s.status = ERROR_script_internal, s;
		}

		dest = v.ref;
		val = 0;

		if (fread (&val, sizeof(uint32_t), 1, file) < 1)
			return s.status = ERROR_endoffile, s;

		v.bits = vt_int;
		v.nval = val;

		replaceSlotValue(dest, v);
	}

	return s.status = OK, s;
}

// ReadInt64(FILE, dest, ...)

value_t jsdb_readInt64(uint32_t args, environment_t *env) {
	value_t v, *dest, s;
	uint64_t val;
	FILE *file;
	int cmd;

	s.bits = vt_status;

	if (debug) fprintf(stderr, "funcall : ReadInt64\n");

	v = eval_arg(&args, env);

	if (vt_file != v.type) {
		fprintf(stderr, "Error: readInt64 => expecting file:FILE => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	file = v.file;

	while( (v = eval_arg(&args, env), v.type != vt_endlist)) {
		if (vt_ref != v.type) {
			fprintf(stderr, "Error: readInt64 => expecting Symbol => %s\n", strtype(v.type));
			return s.status = ERROR_script_internal, s;
		}

		dest = v.ref;
		val = 0;

		if (fread (&val, sizeof(uint64_t), 1, file) < 1)
			return s.status = ERROR_endoffile, s;

		v.bits = vt_int;
		v.nval = val;

		replaceSlotValue(dest, v);
	}

	return s.status = OK, s;
}

// ReadString(FILE, dest)

value_t jsdb_readString(uint32_t args, environment_t *env) {
	value_t v, *dest, s;
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

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: readString => expecting Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	dest = v.ref;
	size = 0;

	while ((ch = fgetc(file)) > 0 )
		if (size < sizeof(val) )
			val[size++] = ch;

	v = newString(val, size);
	replaceSlotValue(dest, v);
	return s.status = OK, s;
}

//  read BSON document

value_t jsdb_readBSON(uint32_t args, environment_t *env) {
	int size, ch, max, len, total = 0;
	value_t v, *dest, *dest2, array;
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

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: readBSON => expecting Symbol => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	dest = v.ref;

	v = eval_arg(&args, env);
	max = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);

	if (vt_ref != v.type) {
		fprintf(stderr, "Error: readBSON => expecting size:ref => %s\n", strtype(v.type));
		return s.status = ERROR_script_internal, s;
	}

	dest2 = v.ref;
	array = newArray();
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
		vec_push (array.aval->array, v);
	  }

	  total += size;
	} while ((max -= size));

	replaceSlotValue(dest, array);

	v.bits = vt_int;
	v.nval = total;
	replaceSlotValue(dest2, v);

	return s.status = OK, s;
}


//  respond with BSON documents
//  bson_response(file, request, response, flags, cursorId, opcode, start, docs)

value_t jsdb_response(uint32_t args, environment_t *env) {
	uint32_t request, result, response, flags, start;
	uint64_t cursorId;
	uint32_t opcode;
	value_t v, array;
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
	request = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);
	response = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);
	flags = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);
	cursorId = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);
	opcode = conv2Int(v).nval;
	abandonValue(v);

	v = eval_arg(&args, env);
	start = conv2Int(v).nval;
	abandonValue(v);

	array = eval_arg(&args, env);

	if (vt_array != array.type) {
		fprintf(stderr, "Error: docs => expecting array => %s\n", strtype(array.type));
		return s.status = ERROR_script_internal, s;
	}

	stat = bson_response(file, request, response, flags, cursorId, opcode, start, array.aval);
	abandonValue(array);

	return s.status = stat, s;
}
