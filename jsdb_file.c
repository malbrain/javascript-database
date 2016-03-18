#include "jsdb.h"

static bool debug = false;
Status bson_read(FILE *file, int len, int *amt, value_t *result);
Status bson_response (FILE *file, uint32_t request, uint32_t response, uint32_t flags, uint64_t cursorId, uint32_t opcode, uint32_t start, array_t *docs);

Status jsdb_open(uint32_t args, environment_t *env) {
    uint32_t fname_len;
    value_t v, *slot;
    char fname[1024];
    FILE *file;

    if (debug) fprintf(stderr, "funcall : Open\n");

	v = eval_arg(&args, env);

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: openFile => expecting file:Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    slot = v.ref;

	v = eval_arg(&args, env);

    if (vt_string != v.type) {
        fprintf(stderr, "Error: openFile => expecting fname:String => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    fname_len = v.aux;

    if (fname_len > 1023) {
        fprintf(stderr, "Error: openFile => filename too long (%d > 1023)\n", fname_len);
        return ERROR_script_internal;
    }

    strncpy(fname, (char *)v.str, fname_len);
    fname[fname_len] = 0;

    file = fopen(fname, "rb");

    if (NULL==file) {
        fprintf(stderr, "Error: openFile => fopen failed on '%s' with %s\n", fname, strerror(errno));
        return ERROR_script_internal;
    }

    v.bits = vt_file;
    v.file = file;
    replaceSlotValue(slot, &v);
    return OK;
}

Status jsdb_close(uint32_t args, environment_t *env) {
    FILE *file;
    value_t v;

    if (debug) fprintf(stderr, "funcall : Close\n");

	v = eval_arg(&args, env);

    if (vt_file != v.type) {
        fprintf(stderr, "Error: closeFile => expecting file:FILE => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    fclose(v.file);
    return OK;
}

// ReadInt32(FILE, dest, ...)

Status jsdb_readInt32(uint32_t args, environment_t *env) {
    value_t v, *dest;
    uint32_t val;
    FILE *file;
    int cmd;

    if (debug) fprintf(stderr, "funcall : ReadInt32\n");

	v = eval_arg(&args, env);

    if (vt_file != v.type) {
        fprintf(stderr, "Error: readInt32 => expecting file:FILE => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    file = v.file;

    while( (v = eval_arg(&args, env), v.type != vt_endlist)) {
        if (vt_ref != v.type) {
            fprintf(stderr, "Error: readInt32 => expecting Symbol => %s\n", strtype(v.type));
            return ERROR_script_internal;
        }

        dest = v.ref;
        val = 0;

        if (fread (&val, sizeof(uint32_t), 1, file) < 1)
            return ERROR_endoffile;

        v.bits = vt_int;
        v.nval = val;

        replaceSlotValue(dest, &v);
    }

    return OK;
}

// ReadInt64(FILE, dest, ...)

Status jsdb_readInt64(uint32_t args, environment_t *env) {
    value_t v, *dest;
    uint64_t val;
    FILE *file;
    int cmd;

    if (debug) fprintf(stderr, "funcall : ReadInt64\n");

	v = eval_arg(&args, env);

    if (vt_file != v.type) {
        fprintf(stderr, "Error: readInt64 => expecting file:FILE => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    file = v.file;

    while( (v = eval_arg(&args, env), v.type != vt_endlist)) {
        if (vt_ref != v.type) {
            fprintf(stderr, "Error: readInt64 => expecting Symbol => %s\n", strtype(v.type));
            return ERROR_script_internal;
        }

        dest = v.ref;
        val = 0;

        if (fread (&val, sizeof(uint64_t), 1, file) < 1)
            return ERROR_endoffile;

        v.bits = vt_int;
        v.nval = val;

        replaceSlotValue(dest, &v);
    }

    return OK;
}

// ReadString(FILE, dest, [&len])

Status jsdb_readString(uint32_t args, environment_t *env) {
    value_t v, *dest;
    char val[4096];
    int size, ch;
    FILE *file;

    if (debug) fprintf(stderr, "funcall : ReadString\n");

	v = eval_arg(&args, env);

    if (vt_file != v.type) {
        fprintf(stderr, "Error: readString => expecting file:FILE => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    file = v.file;

	v = eval_arg(&args, env);

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: readString => expecting Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    dest = v.ref;
    size = 0;

    while ((ch = fgetc(file)) > 0 )
        if (size < sizeof(val) )
            val[size++] = ch;

    v.bits = vt_string;
    v.str = jsdb_alloc(size, false);
    memcpy (v.str, val, size);
    v.refcount = true;
    v.aux = size;

    replaceSlotValue(dest, &v);

	v = eval_arg(&args, env);

    if (!v.type)
		return OK;

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: readString => expecting Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    dest = v.ref;
    v.bits = vt_int;
    v.nval = size;

    replaceSlotValue(dest, &v);
    return OK;
}

//  read BSON document

Status jsdb_readBSON(uint32_t args, environment_t *env) {
    int size, ch, max, len, total = 0;
    value_t v, *dest, *dest2, array;
    Status stat;
    FILE *file;

    if (debug) fprintf(stderr, "funcall : ReadBSON\n");

	v = eval_arg(&args, env);

    if (vt_file != v.type) {
        fprintf(stderr, "Error: readBSON => expecting file:FILE => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    file = v.file;

	v = eval_arg(&args, env);

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: readBSON => expecting Symbol => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    dest = v.ref;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: readBSON => expecting length:num => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    max = v.nval;

	v = eval_arg(&args, env);

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: readBSON => expecting size:ref => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    dest2 = v.ref;
    array = newArray();
    total = 0;

    do {
      if (fread (&len, sizeof(uint32_t), 1, file) < 1)
        return ERROR_endoffile;

      size = sizeof(uint32_t);

      if (len > max)
        return ERROR_endoffile;

      if ((stat = bson_read (file, len - sizeof(uint32_t), &size, &v)))
       return stat;

      if (size > 5)
          vec_push (array.aval->array, v);

      total += size;
    } while ((max -= size));

    replaceSlotValue(dest, &array);

    v.bits = vt_int;
    v.nval = total;
    replaceSlotValue(dest2, &v);

	v = eval_arg(&args, env);

    if (vt_ref != v.type) {
        fprintf(stderr, "Error: readBSON => expecting count:ref => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    dest2 = v.ref;

    v.bits = vt_int;
    v.nval = vec_count(array.aval->array);
    replaceSlotValue(dest2, &v);

    return OK;
}


//  respond with BSON documents
//  bson_response(file, request, response, flags, cursorId, opcode, start, docs)

Status jsdb_response(uint32_t args, environment_t *env) {
    uint32_t request, result, response, flags, start;
    uint64_t cursorId;
    uint32_t opcode;
    FILE *file;
    value_t v;

    if (debug) fprintf(stderr, "funcall : response\n");

	v = eval_arg(&args, env);

    if (vt_file != v.type) {
        fprintf(stderr, "Error: response => expecting file:FILE => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    file = v.file;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: request => expecting id:num => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    request = v.nval;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: response => expecting response:num => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    response = v.nval;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: flags => expecting value:int => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    flags = v.nval;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: cursorId => expecting handle => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    cursorId = v.nval;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: opcode => expecting value:int => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    opcode = v.nval;

	v = eval_arg(&args, env);

    if (vt_int != v.type) {
        fprintf(stderr, "Error: start => expecting value:int => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    start = v.nval;

	v = eval_arg(&args, env);

    if (vt_array != v.type) {
        fprintf(stderr, "Error: docs => expecting array => %s\n", strtype(v.type));
        return ERROR_script_internal;
    }

    return bson_response(file, request, response, flags, cursorId, opcode, start, v.aval);
}
