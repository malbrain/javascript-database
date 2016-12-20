#include <ctype.h>

#include "js.h"
#include "js_props.h"

value_t propStrLength(value_t val, bool lVal) {
	value_t num;

	num.bits = vt_int;
	num.nval = val.aux;
	return num;
}

value_t fcnStrValueOf(value_t *args, value_t *thisVal) {
	if (thisVal->oval->base->type == vt_undef)
		return *thisVal;

	return *thisVal->oval->base;
}

value_t fcnStrSplit(value_t *args, value_t *thisVal) {
	int off, count, prev, max;
	value_t val = newArray(array_value);
	value_t limit, delim;
	value_t s = *thisVal;
	
	if (vec_count(args) > 0)
		delim = conv2Str(args[0], false, false);
	else {
		vec_push(val.aval->values, *thisVal);
		incrRefCnt(*thisVal);
		return val;
	}

	if (vec_count(args) > 1)
		limit = conv2Int(args[1], false);
	else {
		limit.bits = vt_int;
		limit.nval = 1024 * 1024;
	}

	if (limit.type != vt_int)
		return val;

	max = s.aux - delim.aux;
	count = 0;
	prev = 0;

	for (off = 0; count < limit.nval && off < max; off++) {
		if (!delim.aux || !memcmp(s.str+off, delim.str, delim.aux)) {
			value_t v = newString(s.str + prev, off - prev);
			vec_push(val.aval->values, v);
			incrRefCnt(v);
			off += delim.aux;
			prev = off;
			count++;
		}
	}

	if (count < limit.nval && prev < s.aux) {
		value_t v = newString(s.str + prev, s.aux - prev);
		vec_push(val.aval->values, v);
		incrRefCnt(v);
	}

	return val;
}

value_t fcnStrConcat(value_t *args, value_t *thisVal) {
	value_t val[1];

	if (vec_count(args))
		*val = args[0];
	else
		val->bits = vt_string;

	for (int idx = 1; idx < vec_count(args); idx++) {
		value_t v = conv2Str(args[idx], false, false);
		valueCat (val, v, false);
		abandonValueIfDiff(v, args[idx]);
	}

	return *val;
}

value_t fcnStrRepeat(value_t *args, value_t *thisVal) {
	value_t count, val[1];
	int idx = 0;

	if (vec_count(args) > 0)
		count = conv2Int(args[0], false);
	else
		count.nval = 0;

	val->bits = vt_string;

	while (idx < count.nval)
		valueCat(val, *thisVal, false);

	return *val;
}

value_t fcnStrLastIndexOf(value_t *args, value_t *thisVal) {
	value_t from, val, test;
	int start;

	val.bits = vt_int;
	val.nval = -1;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	if (vec_count(args) > 1)
		from = conv2Int(args[1], false);
	else
		from.bits = vt_undef;

	if (from.type == vt_int)
		start = from.nval;
	else
		start = thisVal->aux - test.aux;

	if (start > thisVal->aux)
		start = thisVal->aux;

	if (start < 0)
		start = 0;

	while (start >= 0)
		if (!memcmp(thisVal->str + start, test.str, test.aux))
			return val.nval = start, val;
		else
			start++;

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrReplaceAll(value_t *args, value_t *thisVal) {
	value_t next, test, repl, val[1];
	uint32_t *matches = NULL;
	int off = 0, idx, prev;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return *thisVal;

	if (vec_count(args) > 1)
		repl = conv2Str(args[1], false, false);
	else {
		repl.bits = vt_string;
		repl.string = "undefined";
		repl.aux = strlen(repl.string);
	}

	while (off <= thisVal->aux - test.aux)
		if (!memcmp(thisVal->str + off, test.str, test.aux)) {
			vec_push(matches, off);
			off += test.aux;
		} else
			off++;

	if (vec_count(matches) == 0)
		*val = *thisVal;
	else {
		val->bits = vt_string;
		prev = 0;

		for (idx = 0; idx < vec_count(matches); idx++) {
			next.bits = vt_string;
			next.aux = matches[idx] - prev;
			next.str = thisVal->str + prev;
			valueCat(val, next, false);
			valueCat(val, repl, false);
			prev = matches[idx] + test.aux;
		}

		next.bits = vt_string;
		next.aux = thisVal->aux - prev;
		next.str = thisVal->str + prev;
		valueCat(val, next, false);
	}

	abandonValueIfDiff(test, args[0]);
	abandonValueIfDiff(repl, args[1]);
	vec_free(matches);
	return *val;
}

value_t fcnStrSubstring(value_t *args, value_t *thisVal) {
	value_t off, end, val;

	if (vec_count(args) > 0)
		off = conv2Int(args[0], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		return val.bits = vt_undef, val;

	if (vec_count(args) > 1)
		end = conv2Int(args[1], false);
	else
		end.nval = thisVal->aux;

	if (end.type != vt_int)

	val.bits = vt_string;
	val.aux = 0;

	if (off.nval < 0)
		off.nval = 0;

	if (off.nval > thisVal->aux)
		off.nval = thisVal->aux;

	if (end.nval > thisVal->aux)
		end.nval = thisVal->aux;

	if (off.nval < 0)
		off.nval = 0;

	if (end.nval < 0)
		end.nval = 0;

	if (end.nval > off.nval)
		val = newString(thisVal->str + off.nval, end.nval - off.nval);
	else if (end.nval < off.nval)
		val = newString(thisVal->str + end.nval, off.nval - end.nval);
    else
		val.bits = vt_string;

	return val;
}

value_t fcnStrTrim(value_t *args, value_t *thisVal) {
	int len = thisVal->aux, start = 0;

	while (len)
		if (isspace(thisVal->str[len - 1]))
			len--;
		else
			break;
 
	while (start < len)
		if (isspace(thisVal->str[start]))
			start++;
		else
			break;
 
	return newString(thisVal->str + start, len - start);
}

value_t fcnStrToUpperCase(value_t *args, value_t *thisVal) {
	value_t val;
	int idx;

	val = newString(thisVal->str, thisVal->aux);

	for (idx = 0; idx < val.aux; idx++)
		val.str[idx] = toupper(val.str[idx]);

	return val;
}

value_t fcnStrToLowerCase(value_t *args, value_t *thisVal) {
	value_t val;
	int idx;

	val = newString(thisVal->str, thisVal->aux);

	for (idx = 0; idx < val.aux; idx++)
		val.str[idx] = tolower(val.str[idx]);

	return val;
}

value_t fcnStrSubstr(value_t *args, value_t *thisVal) {
	value_t off, count, val;

	val.bits = vt_string;
	val.aux = 0;

	if (vec_count(args) > 0)
		off = conv2Int(args[0], false);
	else
		return *thisVal;

	if (off.type != vt_int)
		return *thisVal;

	if (off.nval < 0)
		off.nval += thisVal->aux;

	if (vec_count(args) > 1)
		count = conv2Int(args[1], false);
	else
		count.bits = vt_undef;

	if (count.type != vt_int)
		count.nval = thisVal->aux - off.nval;

	if (count.nval > thisVal->aux - off.nval)
		count.nval = thisVal->aux - off.nval;

	if (count.nval > 0)
		val = newString(thisVal->str + off.nval, count.nval);

	return val;
}

value_t fcnStrSlice(value_t *args, value_t *thisVal) {
	value_t slice, end, val;
	int count, start;

	if (vec_count(args) > 0)
		slice = conv2Int(args[0], false);
	else {
		slice.bits = vt_int;
		slice.nval = 0;
	}

	if (vec_count(args) > 1)
		end = conv2Int(args[1], false);
	else {
		end.bits = vt_int;
		end.nval = thisVal->aux;
	}

	if (slice.type != vt_int)
		slice.nval = 0;

	if (end.nval < 0)
		end.nval += thisVal->aux;

	if (end.nval > thisVal->aux || end.nval == 0)
		end.nval = thisVal->aux;

	if (slice.nval < 0) {
		start = slice.nval + thisVal->aux;
		count = -slice.nval;
	} else {
		start = slice.nval;
		count = end.nval - start;
	}

	if (count > 0)
		val = newString(thisVal->str + start, count);
	else
		val.bits = vt_string, val.aux = 0;

	return val;
}

value_t fcnStrReplace(value_t *args, value_t *thisVal) {
	value_t next, pattern, repl, val[1];
	int off = 0;

	if (vec_count(args) > 0)
		pattern = conv2Str(args[0], false, false);
	else
		return *thisVal;

	if (vec_count(args) > 1)
		repl = conv2Str(args[1], false, false);
	else
		repl.bits = vt_string;

	val->bits = vt_string;

	while (off < thisVal->aux - pattern.aux) {
		if (!memcmp(thisVal->str + off, pattern.str, pattern.aux)) {
			next.bits = vt_string;
			next.aux = off;
			next.str = thisVal->str;
			valueCat(val, next, false);
			valueCat(val, repl, false);
			next.aux = off;
			next.str = thisVal->str + off + pattern.aux;
			valueCat(val, next, false);
			break;
		} else
			off++;
	}

	abandonValueIfDiff(pattern, args[0]);
	abandonValueIfDiff(repl, args[1]);
	return val->aux ? *val : *thisVal;
}

value_t fcnStrStartsWith(value_t *args, value_t *thisVal) {
	value_t test, off, val;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	if (vec_count(args) > 1)
		off = conv2Int(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.nval < 0)
		return val;

	if (off.nval >=  0)
	  if (off.nval < thisVal->aux - test.aux)
		val.boolean = !memcmp(thisVal->str + off.nval, test.str, test.aux);

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrIndexOf(value_t *args, value_t *thisVal) {
	value_t test, off, val;

	val.bits = vt_int;
	val.nval = -1;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	if (vec_count(args) > 1)
		off = conv2Int(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		return val;

	if (off.nval < 0)
		off.nval = 0;

	while (off.nval < thisVal->aux - test.aux)
		if (!memcmp(thisVal->str + off.nval, test.str, test.aux)) {
			val.nval = off.nval;
			break;
		} else
			off.nval++;

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrIncludes(value_t *args, value_t *thisVal) {
	value_t test, off, val;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	if (vec_count(args) > 1)
		off = conv2Int(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		off.nval = 0;

	while (off.nval < thisVal->aux - test.aux)
		if ((val.boolean = !memcmp(thisVal->str + off.nval, test.str, test.aux)))
			break;
		else
			off.nval++;

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrEndsWith(value_t *args, value_t *thisVal) {
	value_t test, len, val;
	int off;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	if (vec_count(args) > 1)
		len = conv2Int(args[1], false);
	else {
		len.bits = vt_int;
		len.nval = thisVal->aux;
	}

	off = len.nval - test.aux;

	if (off < 0)
		val.boolean = false;
	else
		val.boolean = !memcmp(thisVal->str + off, test.str, test.aux);

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrCharCodeAt(value_t *args, value_t *thisVal) {
	value_t idx, val;

	if (vec_count(args) > 0)
		idx = conv2Int(args[0], false);
	else
		idx.bits = vt_undef;

	if (idx.type != vt_int) {
		idx.bits = vt_int;
		idx.nval = 0;
	}

	if (idx.nval < 0 || idx.nval > thisVal->aux)
		return val.bits = vt_nan, val;

	val.bits = vt_int;
	val.nval = thisVal->str[idx.nval];
	return val;
}

value_t fcnStrCharAt(value_t *args, value_t *thisVal) {
	value_t idx, val;

	val.bits = vt_string;
	val.aux = 0;

	if (vec_count(args) > 0)
		idx = conv2Int(args[0], false);
	else
		idx.bits = vt_undef;

	if (idx.type == vt_undef) {
		idx.bits = vt_int;
		idx.nval = 0;
	}

	if (idx.type != vt_int)
		return val;

	if (idx.nval < thisVal->aux)
		val = newString(thisVal->str + idx.nval, 1);

	return val;
}

value_t fcnStrToString(value_t *args, value_t *thisVal) {
	value_t ans[1];

	ans->bits = vt_string;
	ans->string = "\"";
	ans->aux = 1;

	valueCat(ans, *thisVal, false);
	return *ans;
}

struct PropVal builtinStrProp[] = {
	{ propStrLength, "length"},
	{ NULL, NULL}
};

struct PropFcn builtinStrFcns[] = {
	{ fcnStrCharCodeAt, "charCodeAt" },
	{ fcnStrCharAt, "charAt" },
	{ fcnStrEndsWith, "endsWith" },
	{ fcnStrIncludes, "includes" },
	{ fcnStrIndexOf, "indexOf" },
	{ fcnStrLastIndexOf, "lastIndexOf" },
	{ fcnStrRepeat, "repeat" },
	{ fcnStrSlice, "slice" },
	{ fcnStrSubstr, "substr" },
	{ fcnStrSubstring, "substring" },
	{ fcnStrSplit, "split" },
	{ fcnStrStartsWith, "startsWith" },
	{ fcnStrReplace, "replace" },
	{ fcnStrReplaceAll, "replaceAll" },
	{ fcnStrToLowerCase, "toLowerCase" },
	{ fcnStrToUpperCase, "toUpperCase" },
	{ fcnStrTrim, "trim" },
	{ fcnStrConcat, "concat" },
	{ fcnStrValueOf, "valueOf" },
	{ fcnStrToString, "toString" },
	{ NULL, NULL}
};

