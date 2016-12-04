#include "js.h"

value_t js_strtod(value_t val);

typedef enum  {
	jsonElement,	// starting an Object/Array element
	jsonLiteral,	// assembling literal token
	jsonNumber,		// assembling a number
	jsonString,		// assembling a string
	jsonEnd			// end numeric element
} jsonState;

//	process next element with top-of-stack

bool processElement(array_t *stack, array_t *name, value_t *next) {
	value_t err, *slot;
	int type;

	err.bits = vt_undef;

	if (vec_count(stack->values) && next->type != vt_undef)
	 switch ((type = vec_last(stack->values).type)) {
	  case vt_object:
		if (vec_size(name->values))
		  slot = lookup(vec_last(stack->values).oval, vec_pop(name->values, err), true);
		*slot = *next;
		return true;

	  case vt_array:
		vec_push(vec_last(stack->values).aval->values, *next);
		return true;

	  default:
		break;
	 }

	fprintf(stderr, "Error: processElement => invalid JSON entry type=> %s\n", strtype(type));
	return false;
}

//	parse JSON string to value

value_t jsonParse(value_t v) {
	jsonState state = jsonElement;
	value_t next[1], val, err, s;
	array_t stack[1], name[1];
	bool minus = false;
	bool quot = false;
	char buff[64];
	int off = 0;
	int ch, nxt;

	next->bits = vt_undef;
	val.bits = vt_string;
	val.str = buff;

	memset (stack, 0, sizeof(array_t));
	memset (name, 0, sizeof(array_t));

	err.bits = vt_undef;
	s.bits = vt_status;

	while (true) {
		if (state != jsonEnd) {
		 if (v.type == vt_string) {
		  if (off < v.aux)
			ch = v.str[off++];
		  else
			ch = EOF;
		 } else if (v.type == vt_file) {
			ch = getc(v.file);
		 } else {
			goto jsonErr;
		 }
		} else
		 state = jsonElement;

		switch (state) {
		  case jsonLiteral:
			if (ch == EOF)
				goto jsonErr;

			if (val.aux < sizeof(buff)) {
				buff[val.aux++] = ch;
				continue;
			}

			goto jsonErr;

		  case jsonElement:
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

			  case '}': {
				value_t obj = vec_pop(stack->values, err);

				if (obj.type != vt_object)
					goto jsonErr;

				if (next->type != vt_undef)
					*lookup(obj.oval, vec_pop(name->values, err), true) = *next;

				*next = obj;
				continue;
			  }

			  case ']': {
				value_t array = vec_pop(stack->values, err);

				if (array.type != vt_array)
					goto jsonErr;

				if (next->type != vt_undef)
					vec_push(array.aval->values, *next);

				*next = array;
				continue;
			  }

			  case ',':
				if (!processElement(stack, name, next))
					goto jsonErr;

				next->bits = vt_undef;
				continue;
			}

			state = jsonLiteral;
			buff[0] = ch;
			val.aux = 1;
			continue;

		  case jsonString:
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
	vec_free(stack->values);
	return s.status = ERROR_json_parse, s;
}
