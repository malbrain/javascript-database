#include "js.h"

#ifdef _WIN32
#define fread_unlocked  _fread_nolock
#define fwrite_unlocked _fwrite_nolock
#define fgetc_unlocked  _fgetc_nolock
#endif

#ifdef __APPLE__
#define fread_unlocked  fread
#define fwrite_unlocked fwrite
#define fgetc_unlocked  getc_unlocked
#endif

Status bson_read (FILE *file, int len, int *amt, value_t *result) {
	value_t namestr[1024];
	value_t v, val[1024];
	int doclen[1024];
	char name[4096];
	int code, size;
	int depth = 0;
	int ch;

	val[0] = newObject();
	doclen[0] = len;

	while (doclen[depth]-- > 0) {
		code = fgetc_unlocked(file);
		(*amt)++;

		if (code < 0 )
			return ERROR_endoffile;

		if (code == 0) {	 // end of document
			if (doclen[depth])
				return ERROR_endoffile;
 
			if (!depth--) {
				*result = val[0];
				return OK;
			}

			incrRefCnt(val[depth+1]);

			if (val[depth].type == vt_object) {
				replaceSlot(lookup(val[depth].oval, namestr[depth], true), val[depth+1]);
			} else {
				vec_push(val[depth].aval->values, val[depth+1]);
			}

			continue;
		}

		size = 0;

		while (((*amt)++, ch = fgetc_unlocked(file)) > 0 ) {
			if (doclen[depth]-- > 0) {
				if (size < sizeof(name) )
					name[size++] = ch;
				else
					return ERROR_bsonformat;
			} else
				return ERROR_bsonformat;
		}
 
		doclen[depth]--;
		namestr[depth] = newString(name, size);

		switch (code) {
		case 0x1: {
			v.bits = vt_dbl;

			if (fread_unlocked (&v.dbl, sizeof(double), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(double);
			(*amt) += sizeof(double);
			break;
		}
		case 0x2: {
			uint32_t strLen;

			if (fread_unlocked (&strLen, sizeof(uint32_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(uint32_t);
			(*amt) += sizeof(uint32_t);
			v.bits = vt_string;
			v.str = js_alloc(strLen, false);
			v.refcount = true;
			v.aux = strLen - 1;

			if (fread_unlocked (v.str, strLen, 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= strLen;
			(*amt) += strLen;
			break;
		}
		case 0x3: {
			uint32_t objLen;

			if (fread_unlocked (&objLen, sizeof(uint32_t), 1, file) < 1)
				return ERROR_endoffile;

			(*amt) += sizeof(uint32_t);
			doclen[depth] -= objLen;

			doclen[++depth] = objLen - sizeof(uint32_t);
			val[depth] = newObject();
			continue;
		}
		case 0x4: {
			uint32_t arrayLen;

			if (fread_unlocked (&arrayLen, sizeof(uint32_t), 1, file) < 1)
				return ERROR_endoffile;

			(*amt) += sizeof(uint32_t);
			doclen[depth] -= arrayLen;

			doclen[++depth] = arrayLen - sizeof(uint32_t);
			val[depth] = newArray(array_value);
			continue;
		}
		case 0x5: {
			uint32_t miscLen;
			uint8_t type;

			if (fread_unlocked (&miscLen, sizeof(uint32_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(uint32_t);
			(*amt) += sizeof(uint32_t);

			if (fread_unlocked (&type, sizeof(uint8_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= 1;
			(*amt) += 1;

			switch (type) {
			case 0:	v.bits = vt_binary;		break;
			case 1:	v.bits = vt_function;	break;
			case 4:	v.bits = vt_uuid;	 	break;
			case 5:	v.bits = vt_md5;	 	break;
			case 0x80: v.bits = vt_user; 	break;
			}

			v.str = js_alloc(miscLen + 1, false);
			v.refcount = true;
			v.aux = miscLen;

			if (fread_unlocked (v.str, miscLen, 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= miscLen;
			(*amt) += miscLen;
			break;
		}
		case 0x7: {
			v.bits = vt_objId;
			v.str = js_alloc(12, false);
			v.refcount = true;
			v.aux = 12;

			if (fread_unlocked (v.str, 12, 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= 12;
			(*amt) += 12;
			break;
		}
		case 0x8: {
			uint32_t num = 0;
			v.bits = vt_bool;

			if (fread_unlocked (&num, 1, 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= 1;
			(*amt) += 1;
			v.nval = num;
			break;
		}
		case 0x9: {
			uint64_t num;

			if (fread_unlocked (&num, sizeof(uint64_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(uint64_t);
			(*amt) += sizeof(uint64_t);
			v.bits = vt_date;
			v.nval = num;
			break;
		}
		case 0x10: {
			uint32_t num;

			if (fread_unlocked (&num, sizeof(uint32_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(uint32_t);
			(*amt) += sizeof(uint32_t);
			v.bits = vt_int;
			v.nval = num;
			break;
		}
		case 0x11: {
			uint64_t num;

			if (fread_unlocked (&num, sizeof(uint64_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(uint64_t);
			(*amt) += sizeof(uint64_t);
			v.bits = vt_int;
			v.nval = num;
			break;
		}
		case 0x12: {
			uint64_t num;

			if (fread_unlocked (&num, sizeof(uint64_t), 1, file) < 1)
				return ERROR_endoffile;

			doclen[depth] -= sizeof(uint64_t);
			(*amt) += sizeof(uint64_t);
			v.bits = vt_int;
			v.nval = num;
			break;
		}
		}

		incrRefCnt(v);

		if (val[depth].type == vt_object)
			replaceSlot(lookup(val[depth].oval, namestr[depth], true), v);
		else
			vec_push(val[depth].aval->values, v);
	}

	return ERROR_endoffile;
}

typedef struct Builder {
	struct Builder *next;
	uint32_t length;
	char body[500];
} build_t;

void build_append(uint32_t *size, build_t **document, build_t **doclast, void *str, uint32_t len) {

	if (!*doclast) {
		(*doclast) = js_alloc(sizeof(build_t), false);
		(*document) = *doclast;
		(*doclast)->next = NULL;
		(*doclast)->length = 0;
	}

	if (500 - (*doclast)->length < len) {
		(*doclast) = js_alloc(sizeof(build_t), false);
		(*doclast)->next = NULL;
		(*doclast)->length = 0;
	}

	memcpy ((*doclast)->body + (*doclast)->length, str, len);
	(*doclast)->length += len;
	*size += len;
}

void build_move(uint8_t type, build_t **document, build_t **doclast, uint32_t *doclen, value_t name) {
	build_t *curr = document[1];
	uint8_t zero = 0;
	uint32_t len;

	build_append(doclen, document, doclast, &type, 1);

	if (name.type == vt_string)
		build_append(doclen, document, doclast, name.str, name.aux);

	build_append(doclen, document, doclast, &zero, 1);

	len = doclen[1] + 4 + 1;
	build_append(doclen, document, doclast, &len, sizeof(uint32_t));

	// splice the document chain

	if (curr) {
	  if (doclast[0])
		doclast[0]->next = curr;

	  doclast[0] = doclast[1];
	  doclen[0] += doclen[1];
	}

	build_append(doclen, document, doclast, &zero, 1);
}

//  respond to a query request with an array of documents/values

Status bson_response (FILE *file, uint32_t request, uint32_t response, uint32_t flags, uint64_t cursorId, uint32_t opcode, uint32_t start, array_t *docs) {
	uint32_t count = vec_count(docs->values);
	uint32_t *length = NULL, size = 0;
	build_t **result = NULL;
	build_t *document[1024];
	build_t *doclast[1024];
	uint32_t doclen[1024];
	value_t name[1024];
	value_t obj[1024];
	uint8_t doctype;
	int idx[1024];
	int i, depth;
	char zero[1];
	
	zero[0] = 0;

	for (i = 0; i < count; i++) {
	  name[0].bits = vt_null;
	  obj[0] = docs->values[i];
	  document[0] = NULL;
	  doclast[0] = NULL;
	  doclen[0] = 0;
	  idx[0] = 0;
	  depth = 1;

	  document[depth] = NULL;
	  doclast[depth] = 0;
	  doclen[depth] = 0;
	  idx[depth] = 0;

	  while (depth > 0) {

		// find next value in parent object or array

		if (obj[depth - 1].type == vt_array) {
			int max = vec_count(obj[depth - 1].aval->values);

			if (idx[depth] < max) {
				obj[depth] = obj[depth - 1].aval->values[idx[depth]++];
				name[depth].bits = vt_null;
			} else {
				if (--depth) {
					build_move (0x04, document + depth, doclast + depth, doclen + depth, name[depth]);
				}
				continue;
			}
		} else if (obj[depth - 1].type == vt_object) {
			struct Object *scan = obj[depth - 1].oval;

			if (idx[depth] < vec_count(scan->pairs)) {
				name[depth] = scan->pairs[idx[depth]].name;
				obj[depth] = scan->pairs[idx[depth]++].value;
			} else {
				if (--depth) {
					build_move (0x03, document + depth, doclast + depth, doclen + depth, name[depth]);
				}
				continue;
			}
		} else if (obj[depth - 1].type == vt_docarray) {
			docarray_t *scan = obj[depth - 1].docarray;

			if (idx[depth] < scan->count) {
				obj[depth] = getDocArray(obj[depth - 1].docarray, idx[depth]++);
				name[depth].bits = vt_null;
			} else {
				if (--depth) {
					build_move (0x04, document + depth, doclast + depth, doclen + depth, name[depth]);
				}
				continue;
			}
		} else if (obj[depth - 1].type == vt_document) {
			struct Document *scan = obj[depth - 1].document;

			if (idx[depth] < scan->count) {
				name[depth] = getDocName(scan, idx[depth]);
				obj[depth] = getDocValue(scan, idx[depth]++);
			} else {
				if (--depth) {
					build_move (0x03, document + depth, doclast + depth, doclen + depth, name[depth]);
				}
				continue;
			}
		} else {
			depth--;
			continue;
		}

		switch (obj[depth].type) {
		case vt_objId: {
			doctype = 0x07;

			build_append(doclen + depth, document + depth, doclast + depth, &doctype, 1);

			if (name[depth].type == vt_string) {
				build_append(doclen + depth, document + depth, doclast + depth, name[depth].str, name[depth].aux);
			}

			build_append(doclen + depth, document + depth, doclast + depth, zero, 1);
			build_append(doclen + depth, document + depth, doclast + depth, obj[depth].str, 12);
			break;
		}
		case vt_string: {
			uint32_t len = obj[depth].aux + 1;
			doctype = 0x02;

			build_append(doclen + depth, document + depth, doclast + depth, &doctype, 1);

			if (name[depth].type == vt_string) {
				build_append(doclen + depth, document + depth, doclast + depth, name[depth].str, name[depth].aux);
			}

			build_append(doclen + depth, document + depth, doclast + depth, zero, 1);
			build_append(doclen + depth, document + depth, doclast + depth, &len, sizeof(uint32_t));
			build_append(doclen + depth, document + depth, doclast + depth, obj[depth].str, obj[depth].aux);
			build_append(doclen + depth, document + depth, doclast + depth, zero, 1);
			break;
		}
		case vt_int: {
			int len = sizeof(uint64_t);
			doctype = 0x12;

			build_append(doclen + depth, document + depth, doclast + depth, &doctype, 1);

			if (depth && name[depth].type == vt_string) {
				build_append(doclen + depth, document + depth, doclast + depth, name[depth].str, name[depth].aux);
			}

			build_append(doclen + depth, document + depth, doclast + depth, zero, 1);
			build_append(doclen + depth, document + depth, doclast + depth, &obj[depth].nval, len);
			break;
		}
		case vt_dbl: {
			doctype = 0x01;

			build_append(doclen + depth, document + depth, doclast + depth, &doctype, 1);

			if (depth && name[depth].type == vt_string) {
				build_append(doclen + depth, document + depth, doclast + depth, name[depth].str, name[depth].aux);
			}

			build_append(doclen + depth, document + depth, doclast + depth, zero, 1);
			build_append(doclen + depth, document + depth, doclast + depth, &obj[depth].dbl, sizeof(double));
			break;
		}
		case vt_document:
		case vt_docarray:
		case vt_array:
		case vt_object: {
			document[++depth] = NULL;
			name[depth].bits = vt_null;
			doclast[depth] = 0;
			doclen[depth] = 0;
			idx[depth] = 0;
			break;
		}
		default:;
		}

	  }

	  // allow for zero terminator and size of length

	  vec_push (result, document[1]);
	  vec_push (length, doclen[1] + 1 + 4);
	  size += doclen[1] + 1 + 4;  
	}

	//  emit response

	size += 4 * sizeof(uint32_t);

	if (opcode == 1)
		size += 3 * sizeof(uint32_t) + sizeof(uint64_t);
	else
		size += 1 + sizeof(uint32_t);

	if (fwrite_unlocked (&size, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	if (fwrite_unlocked (&request, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	if (fwrite_unlocked (&response, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	if (fwrite_unlocked (&opcode, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	if (opcode == 1) {
	  if (fwrite_unlocked (&flags, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	  if (fwrite_unlocked (&cursorId, sizeof(uint64_t), 1, file) < 1)
		return ERROR_tcperror;

	  if (fwrite_unlocked (&start, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	  if (fwrite_unlocked (&count, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;
	}

	//  write the documents

	for (i = 0; i < count; i++) {
		uint32_t amt = length[i];
		build_t *next, *resp;
		int wire = 0;

		if (fwrite_unlocked (&amt, sizeof(uint32_t), 1, file) < 1)
			return ERROR_tcperror;

		wire += sizeof(uint32_t);

		if ((resp = result[i])) do {
			if (fwrite_unlocked (resp->body, resp->length, 1, file) < 1)
				return ERROR_tcperror;
			else
				next = resp->next;

			wire += resp->length;
			js_free(resp);
		} while ((resp = next));

		if (fwrite_unlocked (zero, 1, 1, file) < 1)
			return ERROR_tcperror;

		wire += 1;
	}

	vec_free(result);
	vec_free(length);

	if (opcode == 2011) {
	  size = sizeof(uint32_t) + 1;

	  if (fwrite_unlocked (&size, sizeof(uint32_t), 1, file) < 1)
		return ERROR_tcperror;

	  if (fwrite_unlocked (zero, 1, 1, file) < 1)
		return ERROR_tcperror;
	}
	  
	fflush(file);
	return OK;
}
