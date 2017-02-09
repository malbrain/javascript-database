#include <ctype.h>

#include "js.h"
#include "js_props.h"

//  strings

#ifndef _WIN32
#define LBRACE {
#define RBRACE }
#else
#define LBRACE
#define RBRACE
#endif

string_t UndefinedStr = {9, LBRACE "undefined" RBRACE };
string_t EmptyStr = {0, LBRACE "" RBRACE };
string_t QuoteStr = {1, LBRACE "\"" RBRACE };
string_t ColonStr = {1, LBRACE ":" RBRACE };
string_t CommaStr = {1, LBRACE "," RBRACE };
string_t LeftBraceStr = {1, LBRACE "{" RBRACE };
string_t RightBraceStr = {1, LBRACE "}" RBRACE };
string_t LeftBrackStr = {1, LBRACE "[" RBRACE };
string_t RightBrackStr = {1, LBRACE "]" RBRACE };
string_t TrueStr = {4, LBRACE "true" RBRACE };
string_t FalseStr = {5, LBRACE "false" RBRACE };
string_t InfinityStr = {8, LBRACE "Infinity" RBRACE };
string_t MInfinityStr = {9, LBRACE "-Infinity" RBRACE };
string_t NullStr = {4, LBRACE "null" RBRACE };
string_t NaNStr = {3, LBRACE "NaN" RBRACE };
string_t ToStringStr = {8, LBRACE "toString" RBRACE };
string_t ValueOfStr = {7, LBRACE "valueOf" RBRACE };
string_t NotObjStr = {24, LBRACE "not an object/array type" RBRACE };

value_t newString(void *value, int len) {
	value_t v;

	if (len < 0)
		len = strlen(value);

	v.bits = vt_string;

	if (len) {
		string_t *str = js_alloc(len + 1 + sizeof(string_t), false);

		if (value)
			memcpy(str->val, value, len);

		str->val[len] = 0;
		str->len = len;

		v.refcount = 1;
		v.addr = str;
	} else
		v.addr = &EmptyStr;

	return v;
}

value_t propStrLength(value_t val, bool lVal) {
	string_t *valstr = js_addr(val);
	value_t num;

	num.bits = vt_int;
	num.nval = valstr->len;
	return num;
}

value_t fcnStrValueOf(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);

	if (oval->base->type == vt_undef)
		return *thisVal;

	return *oval->base;
}

value_t fcnStrSplit(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *delimstr;
	value_t val = newArray(array_value);
	int off, count, prev, max;
	array_t *aval = val.addr;
	value_t limit, delim;
	
	if (vec_cnt(args) > 0)
		delim = conv2Str(args[0], false, false);
	else {
		vec_push(aval->valuePtr, *thisVal);
		incrRefCnt(*thisVal);
		return val;
	}

	delimstr = js_addr(delim);

	if (vec_cnt(args) > 1)
		limit = conv2Int(args[1], false);
	else {
		limit.bits = vt_int;
		limit.nval = 1024 * 1024;
	}

	if (limit.type != vt_int)
		return val;

	max = thisstr->len - delimstr->len;
	count = 0;
	prev = 0;

	for (off = 0; count < limit.nval && off < max; off++) {
	  if (!delimstr->len || !memcmp(thisstr->val+off, delimstr->val, delimstr->len)) {
		value_t v = newString(thisstr->val + prev, off - prev);
		vec_push(aval->valuePtr, v);
		off += delimstr->len;
		incrRefCnt(v);
		prev = off;
		count++;
	  }
	}

	if (count < limit.nval && prev < thisstr->len) {
		value_t v = newString(thisstr->val + prev, thisstr->len - prev);
		vec_push(aval->valuePtr, v);
		incrRefCnt(v);
	}

	return val;
}

value_t fcnStrConcat(value_t *args, value_t *thisVal) {
	string_t *str = js_addr(*thisVal);
	value_t val[1];

	*val = newString(str->val, str->len);

	for (int idx = 0; idx < vec_cnt(args); idx++) {
		value_t v = conv2Str(args[idx], false, false);
		valueCat (val, v, false);
		abandonValueIfDiff(v, args[idx]);
	}

	return *val;
}

value_t fcnStrRepeat(value_t *args, value_t *thisVal) {
	value_t count, val[1];
	int idx = 0;

	if (vec_cnt(args) > 0)
		count = conv2Int(args[0], false);
	else
		count.nval = 0;

	val->bits = vt_string;
	val->addr = &EmptyStr;

	while (idx < count.nval)
		valueCat(val, *thisVal, false);

	return *val;
}

value_t fcnStrLastIndexOf(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *teststr;
	value_t from, val, test;
	int start;

	val.bits = vt_int;
	val.nval = -1;

	if (vec_cnt(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	teststr = js_addr(test);

	if (vec_cnt(args) > 1)
		from = conv2Int(args[1], false);
	else
		from.bits = vt_undef;

	if (from.type == vt_int)
		start = from.nval;
	else
		start = thisstr->len - teststr->len;

	if (start > thisstr->len)
		start = thisstr->len;

	if (start < 0)
		start = 0;

	while (start >= 0)
		if (!memcmp(thisstr->val + start, teststr->val, teststr->len))
			return val.nval = start, val;
		else
			start++;

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrReplaceAll(value_t *args, value_t *thisVal) {
	string_t *teststr, *thisstr = js_addr(*thisVal);
	value_t test, repl, val[1];
	uint32_t *matches = NULL;
	int off = 0, idx, prev;

	if (vec_cnt(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return *thisVal;

	teststr = js_addr(test);

	if (vec_cnt(args) > 1)
		repl = conv2Str(args[1], false, false);
	else {
		repl.bits = vt_string;
		repl.addr = &UndefinedStr;
	}

	while (off <= thisstr->len - teststr->len)
		if (!memcmp(thisstr->val + off, teststr->val, teststr->len)) {
			vec_push(matches, off);
			off += teststr->len;
		} else
			off++;

	if (vec_cnt(matches) == 0)
		*val = *thisVal;
	else {
		val->bits = vt_string;
		val->addr = &EmptyStr;
		prev = 0;

		for (idx = 0; idx < vec_cnt(matches); idx++) {
			valueCatStr(val, thisstr->val + prev, matches[idx] - prev);
			valueCat(val, repl, false);
			prev = matches[idx] + teststr->len;
		}

		valueCatStr(val, thisstr->val + prev, thisstr->len - prev);
	}

	abandonValueIfDiff(test, args[0]);
	abandonValueIfDiff(repl, args[1]);
	vec_free(matches);
	return *val;
}

value_t fcnStrSubstring(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal);
	value_t off, end, val;

	if (vec_cnt(args) > 0)
		off = conv2Int(args[0], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		return val.bits = vt_undef, val;

	if (vec_cnt(args) > 1)
		end = conv2Int(args[1], false);
	else
		end.nval = thisstr->len;

	if (end.type != vt_int)

	if (off.nval < 0)
		off.nval = 0;

	if (off.nval > thisstr->len)
		off.nval = thisstr->len;

	if (end.nval > thisstr->len)
		end.nval = thisstr->len;

	if (off.nval < 0)
		off.nval = 0;

	if (end.nval < 0)
		end.nval = 0;

	if (end.nval > off.nval)
		val = newString(thisstr->val + off.nval, end.nval - off.nval);
	else if (end.nval < off.nval)
		val = newString(thisstr->val + end.nval, off.nval - end.nval);
    else {
		val.bits = vt_string;
		val.addr = &EmptyStr;
	}

	return val;
}

value_t fcnStrTrim(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal);
	int len = thisstr->len, start = 0;

	while (len)
		if (isspace(thisstr->val[len - 1]))
			len--;
		else
			break;
 
	while (start < len)
		if (isspace(thisstr->val[start]))
			start++;
		else
			break;
 
	return newString(thisstr->val + start, len - start);
}

value_t fcnStrToUpperCase(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *valstr;
	value_t val;
	int idx;

	val = newString(thisstr->val, thisstr->len);
	valstr = val.addr;

	for (idx = 0; idx < valstr->len; idx++)
		valstr->val[idx] = toupper(valstr->val[idx]);

	return val;
}

value_t fcnStrToLowerCase(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal),  *valstr;
	value_t val;
	int idx;

	val = newString(thisstr->val, thisstr->len);
	valstr = val.addr;

	for (idx = 0; idx < valstr->len; idx++)
		valstr->val[idx] = tolower(valstr->val[idx]);

	return val;
}

value_t fcnStrSubstr(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal);
	value_t off, count, val;

	val.bits = vt_string;
	val.addr = &EmptyStr;

	if (vec_cnt(args) > 0)
		off = conv2Int(args[0], false);
	else
		return *thisVal;

	if (off.type != vt_int)
		return *thisVal;

	if (off.nval < 0)
		off.nval += thisstr->len;

	if (vec_cnt(args) > 1)
		count = conv2Int(args[1], false);
	else
		count.bits = vt_undef;

	if (count.type != vt_int)
		count.nval = thisstr->len - off.nval;

	if (count.nval > thisstr->len - off.nval)
		count.nval = thisstr->len - off.nval;

	if (count.nval > 0)
		val = newString(thisstr->val + off.nval, count.nval);

	return val;
}

value_t fcnStrSlice(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal);
	value_t slice, end, val;
	int count, start;

	if (vec_cnt(args) > 0)
		slice = conv2Int(args[0], false);
	else {
		slice.bits = vt_int;
		slice.nval = 0;
	}

	if (vec_cnt(args) > 1)
		end = conv2Int(args[1], false);
	else {
		end.bits = vt_int;
		end.nval = thisstr->len;
	}

	if (slice.type != vt_int)
		slice.nval = 0;

	if (end.nval < 0)
		end.nval += thisstr->len;

	if (end.nval > thisstr->len || end.nval == 0)
		end.nval = thisstr->len;

	if (slice.nval < 0) {
		start = slice.nval + thisstr->len;
		count = -slice.nval;
	} else {
		start = slice.nval;
		count = end.nval - start;
	}

	if (count > 0)
		val = newString(thisstr->val + start, count);
	else
		val.bits = vt_string, val.addr = &EmptyStr;

	return val;
}

value_t fcnStrReplace(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *pattstr, *valstr;
	value_t pattern, repl, val[1];
	int off = 0;

	if (vec_cnt(args) > 0)
		pattern = conv2Str(args[0], false, false);
	else
		return *thisVal;

	pattstr = js_addr(pattern);

	if (vec_cnt(args) > 1)
		repl = conv2Str(args[1], false, false);
	else {
		repl.bits = vt_string;
		repl.addr = &EmptyStr;
	}

	val->bits = vt_string;
	val->addr = &EmptyStr;

	while (off < thisstr->len - pattstr->len) {
		if (!memcmp(thisstr->val + off, pattstr->val, pattstr->len)) {
			valueCatStr(val, thisstr->val, off);
			valueCat(val, repl, false);
			valueCatStr(val, thisstr->val + off + pattstr->len, off);
			break;
		} else
			off++;
	}

	abandonValueIfDiff(pattern, args[0]);
	abandonValueIfDiff(repl, args[1]);

	valstr = js_addr(*val);
	return valstr->len ? *val : *thisVal;
}

value_t fcnStrStartsWith(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *teststr;
	value_t test, off, val;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_cnt(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	teststr = js_addr(test);

	if (vec_cnt(args) > 1)
		off = conv2Int(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.nval < 0)
		return val;

	if (off.nval >=  0)
	  if (off.nval < thisstr->len - teststr->len)
		val.boolean = !memcmp(thisstr + off.nval, teststr->val, teststr->len);

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrIndexOf(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *teststr;
	value_t test, off, val;

	val.bits = vt_int;
	val.nval = -1;

	if (vec_cnt(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	teststr = js_addr(test);

	if (vec_cnt(args) > 1)
		off = conv2Int(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		return val;

	if (off.nval < 0)
		off.nval = 0;

	while (off.nval < thisstr->len - teststr->len)
		if (!memcmp(thisstr->val + off.nval, teststr->val, teststr->len)) {
			val.nval = off.nval;
			break;
		} else
			off.nval++;

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrIncludes(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *teststr;
	value_t test, off, val;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_cnt(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	teststr = js_addr(test);

	if (vec_cnt(args) > 1)
		off = conv2Int(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		off.nval = 0;

	while (off.nval < thisstr->len - teststr->len)
	  if ((val.boolean = !memcmp(thisstr->val + off.nval, teststr->val, teststr->len)))
		break;
	  else
		off.nval++;

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrEndsWith(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal), *teststr;
	value_t test, len, val;
	int off;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_cnt(args) > 0)
		test = conv2Str(args[0], false, false);
	else
		return val;

	teststr = js_addr(test);

	if (vec_cnt(args) > 1)
		len = conv2Int(args[1], false);
	else {
		len.bits = vt_int;
		len.nval = thisstr->len;
	}

	off = len.nval - teststr->len;

	if (off < 0)
		val.boolean = false;
	else
		val.boolean = !memcmp(thisstr->val + off, teststr->val, teststr->len);

	abandonValueIfDiff(test, args[0]);
	return val;
}

value_t fcnStrCharCodeAt(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal);
	value_t idx, val;

	if (vec_cnt(args) > 0)
		idx = conv2Int(args[0], false);
	else
		idx.bits = vt_undef;

	if (idx.type != vt_int) {
		idx.bits = vt_int;
		idx.nval = 0;
	}

	if (idx.nval < 0 || idx.nval > thisstr->len)
		return val.bits = vt_nan, val;

	val.bits = vt_int;
	val.nval = thisstr->val[idx.nval];
	return val;
}

value_t fcnStrCharAt(value_t *args, value_t *thisVal) {
	string_t *thisstr = js_addr(*thisVal);
	value_t idx, val;

	val.bits = vt_string;
	val.addr = &EmptyStr;

	if (vec_cnt(args) > 0)
		idx = conv2Int(args[0], false);
	else
		idx.bits = vt_undef;

	if (idx.type == vt_undef) {
		idx.bits = vt_int;
		idx.nval = 0;
	}

	if (idx.type != vt_int)
		return val;

	if (idx.nval < thisstr->len)
		val = newString(thisstr->val + idx.nval, 1);

	return val;
}

value_t fcnStrToString(value_t *args, value_t *thisVal) {
	object_t *oval = js_addr(*thisVal);

	if (thisVal->type == vt_string)
		return *thisVal;

	if (oval->base->type > vt_undef)
		return conv2Str(*oval->base, false, false);

	return conv2Str(*thisVal, false, false);
}

PropVal builtinStrProp[] = {
	{ propStrLength, "length"},
	{ NULL, NULL}
};

PropFcn builtinStrFcns[] = {
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

