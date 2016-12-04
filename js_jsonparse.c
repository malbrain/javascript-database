#include "js.h"

value_t js_strtod(value_t val);

typedef enum  {
	jsonElement,	// starting an Object/Array element
	jsonLiteral,	// assembling literal token
	jsonNumber,		// assembling a number
	jsonString,		// assembling a string
	jsonEnd			// end numeric element
} jsonState;

//	append next element to the top-of-stack object/array

char *appendElement(value_t value, array_t *name, value_t *next) {
	value_t err, *slot;
	int type;

	err.bits = vt_undef;

	switch (value.type) {
	  case vt_object:
		if (vec_size(name->values))
		  *lookup(value.oval, vec_pop(name->values, err), true) = *next;
		else
		  break;

		return NULL;

	  case vt_array:
		vec_push(value.aval->values, *next);
		return NULL;

	  default:
		break;
	}

	return "not an object/array type";
}

//	parse JSON string to value

value_t jsonParse(value_t v) {
	value_t next[1], val, err, object;
	jsonState state = jsonElement;
	array_t stack[1], name[1];
	bool minus = false;
	bool quot = false;
	char buff[64];
	int off = 0;
	int ch, nxt;
	char *msg;

	next->bits = vt_undef;
	val.bits = vt_string;
	val.str = buff;

	memset (stack, 0, sizeof(array_t));
	memset (name, 0, sizeof(array_t));

	err.bits = vt_undef;

	while (true) {
		if (state != jsonEnd) {
		 if (v.type == vt_string) {
		  if (off < v.aux)
			ch = v.str[off++];
		  else
			ch = EOF;
		 } else if (v.type == vt_file) {
			ch = getc(v.file);
			off++;
		 } else {
			msg = "not file or string";
			goto jsonErr;
		 }
		} else
		 state = jsonElement;

		switch (state) {
		  case jsonLiteral:
			if (ch == EOF) {
				msg = "end of file or string";
				goto jsonErr;
			}

			if (isalpha(ch)) {
			  if (val.aux < sizeof(buff)) {
				buff[val.aux++] = ch;
				continue;
			  }

			  msg = "sizeof jsonLiteral > 64";
			  goto jsonErr;
			}

			if (val.aux == 4 && !memcmp(buff, "null", 4))
			  next->bits = vt_null;
			else if (val.aux == 4 && !memcmp(buff, "true", 4))
			  next->bits = vt_bool, next->boolean = true;
			else if (val.aux == 5 && !memcmp(buff, "false", 5))
			  next->bits = vt_bool, next->boolean = false;
			else {
			  msg = "invalid Literal value";
			  goto jsonErr;
			}

		 	state = jsonEnd;
			continue;

		  case jsonElement:
			msg = "invalid Element";

			switch (ch) {
			  case ' ':
			  case 0x09:
			  case 0x0a:
			  case 0x0d:
				continue;

			  case '0':
			  case '1':
			  case '2':
			  case '3':
			  case '4':
			  case '5':
			  case '6':
			  case '7':
			  case '8':
			  case '9':
			  case '-':
				if (next->type != vt_undef)
					goto jsonErr;

				state = jsonNumber;
				buff[0] = ch;
				val.aux = 1;
				continue;

			  case '"':
				if (next->type != vt_undef)
					goto jsonErr;

				state = jsonString;
				next->bits = vt_string;
				val.aux = 0;
				continue;

			  case '[':
				if (next->type != vt_undef)
					goto jsonErr;

				state = jsonElement;
				vec_push(stack->values, newArray(array_value));
				next->bits = vt_undef;
				continue;

			  case '{':
				if (next->type != vt_undef)
					goto jsonErr;

				state = jsonElement;
				vec_push(stack->values, newObject());
				next->bits = vt_undef;
				continue;

			  case ':':
				if (!stack->values || !vec_count(stack->values) || vec_last(stack->values).type != vt_object)
					goto jsonErr;

				if (next->type != vt_string)
					goto jsonErr;

				vec_push(name->values, newString(next->str, next->aux));
				next->bits = vt_undef;
				continue;

			  case EOF:
				if (vec_count(stack->values))
					goto jsonErr;

				return *next;

			  case '}':
			  case ']':
				object = vec_pop(stack->values, err);

				if (next->type != vt_undef)
				  if ((msg = appendElement(object, name, next)))
					goto jsonErr;

				*next = object;
				continue;

			  case ',':
				if (next->type == vt_undef) {
					msg = "invalid empty element";
					goto jsonErr;
				}

				object = vec_last(stack->values);

				if ((msg = appendElement(object, name, next)))
					goto jsonErr;

				next->bits = vt_undef;
				continue;
			}

			state = jsonLiteral;
			buff[0] = ch;
			val.aux = 1;
			continue;

		  case jsonString:
			msg = "Invalid String value";
			nxt = ch;

			if (quot) {
			  switch (ch) {
				case '"': ch = 0x100; break;
				case '/': break;
				case '\\': ch = 0x100; break;
				case 'b': ch = 0x08; break;
				case 'f': ch = 0x0c; break;
				case 'n': ch = 0x0a; break;
				case 'r': ch = 0x0d; break;
				case 't': ch = 0x08; break;
				default:
					msg = "invalid Quoted character";
					goto jsonErr;
			  }
			  quot = false;
			}

			if (ch == EOF)
				goto jsonErr;

			if (ch == '\\') {
				quot = true;
				continue;
			}

			if (ch != '"') {
			  if (val.aux == sizeof(buff)) {
				valueCat(next, val);
				val.aux = 0;
			  }

			  buff[val.aux++] = nxt;
			  continue;
			}

			valueCat(next, val);
			state = jsonElement;
			continue;

		  case jsonNumber:
			msg = "Invalid Number character";

			switch (ch) {
			  case ':':
			  case '{':
			  case '[':
				goto jsonErr;
				
			  case ',':
			  case '}':
			  case ']':
			  case EOF:
				*next = js_strtod(val);
				state = jsonEnd;
				continue;

			  default:
				if (val.aux < sizeof(buff)) {
				  buff[val.aux++] = ch;
				  continue;
				}

				goto jsonErr;
			}
		}
	}

jsonErr:
	fprintf(stderr, "Error: jsonparse => syntax error: %s pos: %d\n", msg, off);

	next->bits = vt_status;
	next->status = ERROR_json_parse;

jsonXit:
	vec_free(stack->values);
	vec_free(name->values);
	return *next;
}
