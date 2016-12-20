#include <ctype.h>
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

char *appendElement(pair_t *pair, value_t *next) {
	switch (pair->value.type) {
	  case vt_object:
		if (pair->name.type == vt_string && pair->name.aux)
			replaceSlot(lookup(pair->value.oval, pair->name, true, false), *next);
		else
			break;

		pair->name.bits = vt_undef;
		return NULL;

	  case vt_array:
		vec_push(pair->value.aval->values, *next);
		return NULL;

	  default:
		break;
	}

	return "not an object/array type";
}

//	parse JSON string to value

value_t jsonParse(value_t v) {
	jsonState state = jsonElement;
	pair_t *stack = NULL, pair;
	value_t next[1], val;
	bool negative = false;
	bool quot = false;
	char *msg = NULL;
	int ch = 0, nxt;
	char buff[64];
	int off = 0;

	next->bits = vt_undef;
	val.bits = vt_string;
	val.str = buff;

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
		}

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
			else if (val.aux == 3 && !memcmp(buff, "NaN", 3))
			  next->bits = vt_nan;
			else if (val.aux == 8 && !memcmp(buff, "Infinity", 8))
			  next->bits = vt_infinite, next->negative = false;
			else if (val.aux == 9 && !memcmp(buff, "+Infinity", 9))
			  next->bits = vt_infinite, next->negative = false;
			else if (val.aux == 9 && !memcmp(buff, "-Infinity", 9))
			  next->bits = vt_infinite, next->negative = true;
			else if (val.aux == 5 && !memcmp(buff, "false", 5))
			  next->bits = vt_bool, next->boolean = false;
			else {
			  msg = "invalid Literal value";
			  goto jsonErr;
			}

		 	state = jsonEnd;
			continue;

		  case jsonEnd:
			state = jsonElement;

		  case jsonElement:
			msg = "invalid Element";

			switch (ch) {
			  case ' ':
			  case 0x09:
			  case 0x0a:
			  case 0x0d:
				continue;

			  case '-':
				if (negative)
					goto jsonErr;

				negative = true;
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
				if (next->type != vt_undef)
					goto jsonErr;

				state = jsonNumber;

				if (negative)
					buff[0] = '-';
				else
					buff[0] = '+';

				negative = false;
				buff[1] = ch;
				val.aux = 2;
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

				pair.name.bits = vt_undef;
				pair.value = newArray(array_value);
				vec_push(stack, pair);
				continue;

			  case '{':
				if (next->type != vt_undef)
					goto jsonErr;

				pair.name.bits = vt_undef;
				pair.value = newObject(vt_object);
				vec_push(stack, pair);
				continue;

			  case ':':
				if (!vec_count(stack) || vec_last(stack).value.type != vt_object)
					goto jsonErr;

				if (next->type == vt_string)
					vec_last(stack).name = *next;
				else
					goto jsonErr;

				next->bits = vt_undef;
				continue;

			  case EOF:
				if (vec_count(stack))
					goto jsonErr;

				return *next;

			  case '}':
			  case ']':
				if (vec_count(stack))
					pair = vec_pop(stack);
				else
					goto jsonErr;

				if (pair.value.type == vt_object && ch != '}')
					goto jsonErr;
				else if (pair.value.type == vt_array && ch != ']')
					goto jsonErr;

				if (next->type != vt_undef)
				  if ((msg = appendElement(&pair, next)))
					goto jsonErr;

				*next = pair.value;
				continue;

			  case ',':
				if (next->type == vt_undef) {
					msg = "invalid empty element";
					goto jsonErr;
				}

				if ((msg = appendElement(&vec_last(stack), next)))
					goto jsonErr;

				next->bits = vt_undef;
				continue;
			}

			state = jsonLiteral;

			if (negative) {
				buff[0] = '-';
				buff[1] = ch;
				val.aux = 2;
			} else {
				buff[0] = ch;
				val.aux = 1;
			}

			negative = false;
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
				valueCat(next, val, false);
				val.aux = 0;
			  }

			  buff[val.aux++] = nxt;
			  continue;
			}

			valueCat(next, val, false);
			state = jsonElement;
			continue;

		  case jsonNumber:
			msg = "Invalid Number character";

			if (val.aux == 2 && buff[1] == '0' && ch == '0')
				goto jsonErr;

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
			  case 'e':
			  case 'E':
			  case '+':
			  case '-':
			  case '.':
				if (val.aux < sizeof(buff)) {
				  buff[val.aux++] = ch;
				  continue;
				}
			  }
			  default:
				goto jsonErr;
			}
	}

jsonErr:
	fprintf(stderr, "Error: jsonparse => syntax error: %s pos: %d\n", msg, off);

	next->bits = vt_status;
	next->status = ERROR_json_parse;

	vec_free(stack);
	return *next;
}
