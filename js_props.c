#include <ctype.h>
#include "js.h"
#include "js_props.h"

value_t propStrLength(value_t val) {
	value_t num;

	num.bits = vt_int;
	num.nval = val.aux;
	return num;
}

value_t propArrayLength(value_t val) {
	value_t num;

	num.bits = vt_int;
	num.nval = vec_count(val.aval->values);
	return num;
}

value_t propBoolLength(value_t val) {
	value_t len;

	len.bits = vt_int;
	len.nval = 1;
	return len;
}

value_t propObjLength(value_t val) {
	value_t len;

	len.bits = vt_int;
	len.nval = 1;
	return len;
}

value_t propObjProto(value_t val) {
	value_t obj;

	obj.bits = vt_object;
	obj.oval = val.oval->proto;
	return obj;
}

value_t propFcnDisplayName(value_t val) {
	stringNode *sn;
	symNode *sym;
	value_t obj;

	if (!val.closure->fcn->name)
		return obj.bits = vt_undef, obj;

	sym = (symNode *)(val.closure->table + val.closure->fcn->name);
	sn = (stringNode *)(val.closure->table + sym->name);

	obj.bits = vt_string;
	obj.string = sn->string;
	obj.aux = sn->hdr->aux;
	return obj;
}

value_t propFcnProto(value_t val) {
	value_t obj;

	obj.bits = vt_object;
	obj.oval = val.closure->proto;
	return obj;
}

value_t fcnObjectSetBaseVal(value_t *args, value_t thisVal) {
	value_t undef;

	if (vec_count(args))
		undef = args[0];
	else
		undef.bits = vt_undef;

	return thisVal.oval->base = undef;
}

value_t fcnObjectValueOf(value_t *args, value_t thisVal) {
	return thisVal.oval->base;
}

value_t fcnObjectToString(value_t *args, value_t thisVal) {
	return conv2Str(thisVal, false);
}

/*
value_t fcnObjectLock(value_t *args, value_t thisVal) {
	value_t val, mode;

	if (vec_count(args) > 0)
		mode = conv2Int(args[0], false);
	else
		return val.bits = vt_undef, val;

	if (mode.type != vt_int)
		return val.bits = vt_undef, val;

	switch (mode.nval) {
	case 0:	readLock(thisVal.oval->lock); break;
	case 1:	writeLock(thisVal.oval->lock); break;
	}

	val.bits = vt_bool;
	val.boolean = true;
	return val;
}

value_t fcnObjectUnlock(value_t *args, value_t thisVal) {
	value_t val;

	rwUnlock(thisVal.oval->lock);
	val.bits = vt_bool;
	val.boolean = true;
	return val;
}
*/
value_t fcnArraySlice(value_t *args, value_t thisVal) {
	int idx, cnt = vec_count(thisVal.aval->values);
	value_t array = newArray(array_value);
	value_t slice, end;
	int start, count;

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
		end.nval = cnt;
	}

	if (slice.type != vt_int)
		slice.nval = 0;

	if (end.nval < 0)
		end.nval += cnt;

	if (end.nval > cnt || end.nval == 0)
		end.nval = cnt;

	if (slice.nval < 0) {
		start = slice.nval + cnt;
		count = -slice.nval;
	} else {
		start = slice.nval;
		count = end.nval - start;
	}

	for (idx = 0; idx < count; idx++) {
		value_t nxt = thisVal.aval->values[start + idx];
		vec_push(array.aval->values, nxt);
		incrRefCnt(nxt);
	}

	return array;
}

value_t fcnArrayConcat(value_t *args, value_t thisVal) {
	value_t array = newArray(array_value);
	int idx;

	for (idx = 0; idx < vec_count(thisVal.aval->values); idx++) {
		value_t nxt = thisVal.aval->values[idx];
		vec_push(array.aval->values, nxt);
		incrRefCnt(nxt);
	}

	for (idx = 0; idx < vec_count(args); idx++) {
	  if (args[idx].type == vt_array) {
		for (int j = 0; j < vec_count(args[idx].aval->values); j++) {
		  vec_push(array.aval->values, args[idx].aval->values[j]);
		  incrRefCnt(args[idx].aval->values[j]);
		}
	  } else if (args[idx].type == vt_object) {
		vec_push (array.aval->values, args[idx]);
		incrRefCnt(args[idx]);
	  } else {
		vec_push (array.aval->values, args[idx]);
		incrRefCnt(args[idx]);
	  }
	}

	return array;
}

value_t fcnArrayValueOf(value_t *args, value_t thisVal) {
	return thisVal.aval->obj->base;
}

value_t fcnArrayJoin(value_t *args, value_t thisVal) {
	uint32_t len = 0, off = 0;
	value_t delim, val, next;
	value_t *values = NULL;

	if (vec_count(args) > 0)
		delim = conv2Str(args[0], false);
	else {
		delim.bits = vt_string;
		delim.string = ",";
		delim.aux = 1;
	}

	for (int idx = 0; idx < vec_count(thisVal.aval->values); idx++) {
		value_t v = thisVal.aval->values[idx];

		switch (v.type) {
		case vt_null:	continue;
		case vt_undef:	continue;
		default: break;
		}

		val = conv2Str(v, false);
		vec_push(values, val);
		len += val.aux;

		if (idx < vec_count(thisVal.aval->values) - 1)
			len += delim.aux;
	}

	val.bits = vt_string;
	val.aux = len;
	val.str = js_alloc(len + 1, false);

	val.refcount = 1;
	val.str[len] = 0;

	for (int idx = 0; idx < vec_count(values); idx++) {
		next = values[idx];

		memcpy(val.str + off, next.str, next.aux);
		off += next.aux;

		if (idx < vec_count(values) - 1) {
			memcpy(val.str + off, delim.str, delim.aux);
			off += delim.aux;
		}

		abandonValue(next);
	}

	assert(off == len);

	abandonValue (delim);
	return val;
}

value_t fcnArrayToString(value_t *args, value_t thisVal) {
	return conv2Str(thisVal, false);
}

/*
value_t fcnArrayLock(value_t *args, value_t thisVal) {
	value_t val, mode;

	if (vec_count(args) > 0)
		mode = conv2Int(args[0], false);
	else
		return val.bits = vt_undef, val;

	if (mode.type != vt_int)
		return val.bits = vt_undef, val;

	switch (mode.nval) {
	case 0:	readLock(thisVal.aval->lock); break;
	case 1:	writeLock(thisVal.aval->lock); break;
	}

	val.bits = vt_bool;
	val.boolean = true;
	return val;
}

value_t fcnArrayUnlock(value_t *args, value_t thisVal) {
	array_t *array = thisVal.aval;
	value_t val;

	rwUnlock(array->lock);
	val.bits = vt_bool;
	val.boolean = true;
	return val;
}
*/
value_t fcnArraySetBaseVal(value_t *args, value_t thisVal) {
	value_t undef;

	if (vec_count(args))
		undef = args[0];
	else
		undef.bits = vt_undef;

	return thisVal.aval->obj->base = undef;
}

value_t fcnStrValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnStrToString(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnStrSplit(value_t *args, value_t thisVal) {
	int off, count, prev, max;
	value_t val = newArray(array_value);
	value_t limit, delim;
	value_t s = thisVal;
	
	if (vec_count(args) > 0)
		delim = conv2Str(args[0], false);
	else {
		vec_push(val.aval->values, thisVal);
		incrRefCnt(thisVal);
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

value_t fcnStrConcat(value_t *args, value_t thisVal) {
	uint32_t length = thisVal.aux, off, idx;
	value_t *strings = NULL, val;

	for (idx = 0; idx < vec_count(args); idx++) {
		value_t n, v = args[idx];
		n = conv2Str(v, false);

		vec_push(strings, n);
		length += n.aux;
	}

	val.bits = vt_string;
	val.str = js_alloc(length + 1, false);
	val.refcount = 1;
	val.aux = length;

	off = thisVal.aux;
	memcpy(val.str, thisVal.str, off);

	for (idx = 0; idx < vec_count(strings); idx++) {
		memcpy(val.str + off, strings[idx].str, strings[idx].aux);
		off += strings[idx].aux;
		abandonValue(strings[idx]);
	}

	val.str[length] = 0;
	vec_free(strings);
	return val;
}

value_t fcnStrRepeat(value_t *args, value_t thisVal) {
	value_t count, val;
	int off, len;

	if (vec_count(args) > 0)
		count = conv2Int(args[0], false);
	else
		count.type = vt_undef;

	if (count.type != vt_int)
		count.nval = 0;

	off = len = thisVal.aux;

	val.bits = vt_string;
	val.aux = len * count.nval;
	val.str = js_alloc(val.aux + 1, false);
	val.refcount = 1;

	memcpy(val.str, thisVal.str, len);

	while (off < val.aux) {
		memcpy(val.str + off, thisVal.str, len);
		off += len;
	}

	val.str[val.aux] = 0;
	return val;
}

value_t fcnStrLastIndexOf(value_t *args, value_t thisVal) {
	value_t from, val, test;
	int start;

	val.bits = vt_int;
	val.nval = -1;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false);
	else
		return val;

	if (vec_count(args) > 1)
		from = conv2Int(args[1], false);
	else
		from.bits = vt_undef;

	if (from.type == vt_int)
		start = from.nval;
	else
		start = thisVal.aux - test.aux;

	if (start > thisVal.aux)
		start = thisVal.aux;

	if (start < 0)
		start = 0;

	while (start >= 0)
		if (!memcmp(thisVal.str + start, test.str, test.aux))
			return val.nval = start, val;
		else
			start++;

	abandonValue(test);
	return val;
}

value_t fcnStrReplaceAll(value_t *args, value_t thisVal) {
	int off = 0, diff = 0, idx, prev;
	uint32_t *matches = NULL;
	value_t test, repl, val;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false);
	else
		return thisVal;

	if (vec_count(args) > 1)
		repl = conv2Str(args[0], false);
	else {
		repl.bits = vt_string;
		repl.string = "undefined";
		repl.aux = strlen(repl.string);
	}

	while (off <= thisVal.aux - test.aux)
		if (!memcmp(thisVal.str + off, test.str, test.aux)) {
			vec_push(matches, off);
			diff += repl.aux - test.aux;
			off += test.aux;
		} else
			off++;

	if (vec_count(matches) == 0)
		val = thisVal;
	else {
		val.bits = vt_string;
		val.str = js_alloc(thisVal.aux + diff + 1, false);
		val.str[thisVal.aux + diff] = 0;
		val.refcount = 1;
		val.aux = 0;
		prev = 0;

		for (idx = 0; idx < vec_count(matches); idx++) {
			memcpy(val.str + val.aux, thisVal.str + prev, matches[idx] - prev);
			val.aux += matches[idx] - prev;
			memcpy(val.str + val.aux, repl.str, repl.aux);
			val.aux += repl.aux;
			prev = matches[idx] + test.aux;
		}

		memcpy(val.str + val.aux, thisVal.str + prev, thisVal.aux - prev);
		val.aux += thisVal.aux - prev;

		assert(val.aux == thisVal.aux + diff);
	}

	abandonValue(test);
	abandonValue(repl);
	vec_free(matches);
	return val;
}

value_t fcnStrSubstring(value_t *args, value_t thisVal) {
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
		end.nval = thisVal.aux;

	if (end.type != vt_int)

	val.bits = vt_string;
	val.aux = 0;

	if (off.nval < 0)
		off.nval = 0;

	if (off.nval > thisVal.aux)
		off.nval = thisVal.aux;

	if (end.nval > thisVal.aux)
		end.nval = thisVal.aux;

	if (off.nval < 0)
		off.nval = 0;

	if (end.nval < 0)
		end.nval = 0;

	if (end.nval > off.nval)
		val = newString(thisVal.str + off.nval, end.nval - off.nval);
	else if (end.nval < off.nval)
		val = newString(thisVal.str + end.nval, off.nval - end.nval);
    else
		val.bits = vt_string;

	return val;
}

value_t fcnStrTrim(value_t *args, value_t thisVal) {
	int len = thisVal.aux, start = 0;

	while (len)
		if (isspace(thisVal.str[len - 1]))
			len--;
		else
			break;
 
	while (start < len)
		if (isspace(thisVal.str[start]))
			start++;
		else
			break;
 
	return newString(thisVal.str + start, len - start);
}

value_t fcnStrToUpperCase(value_t *args, value_t thisVal) {
	value_t val;
	int idx;

	val = newString(thisVal.str, thisVal.aux);

	for (idx = 0; idx < val.aux; idx++)
		val.str[idx] = toupper(val.str[idx]);

	return val;
}

value_t fcnStrToLowerCase(value_t *args, value_t thisVal) {
	value_t val;
	int idx;

	val = newString(thisVal.str, thisVal.aux);

	for (idx = 0; idx < val.aux; idx++)
		val.str[idx] = tolower(val.str[idx]);

	return val;
}

value_t fcnStrSubstr(value_t *args, value_t thisVal) {
	value_t off, count, val;

	val.bits = vt_string;
	val.aux = 0;

	if (vec_count(args) > 0)
		off = conv2Int(args[0], false);
	else
		return thisVal;

	if (off.type != vt_int)
		return thisVal;

	if (off.nval < 0)
		off.nval += thisVal.aux;

	if (vec_count(args) > 1)
		count = conv2Int(args[1], false);
	else
		count.bits = vt_undef;

	if (count.type != vt_int)
		count.nval = thisVal.aux - off.nval;

	if (count.nval > thisVal.aux - off.nval)
		count.nval = thisVal.aux - off.nval;

	if (count.nval > 0)
		val = newString(thisVal.str + off.nval, count.nval);

	return val;
}

value_t fcnStrSlice(value_t *args, value_t thisVal) {
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
		end.nval = thisVal.aux;
	}

	if (slice.type != vt_int)
		slice.nval = 0;

	if (end.nval < 0)
		end.nval += thisVal.aux;

	if (end.nval > thisVal.aux || end.nval == 0)
		end.nval = thisVal.aux;

	if (slice.nval < 0) {
		start = slice.nval + thisVal.aux;
		count = -slice.nval;
	} else {
		start = slice.nval;
		count = end.nval - start;
	}

	if (count > 0)
		val = newString(thisVal.str + start, count);
	else
		val.bits = vt_string, val.aux = 0;

	return val;
}

value_t fcnStrReplace(value_t *args, value_t thisVal) {
	value_t test, repl, val;
	int off = 0, diff;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false);
	else
		return thisVal;

	if (vec_count(args) > 1)
		repl = conv2Str(args[1], false);
	else {
		repl.bits = vt_string;
		repl.string = "undefined";
		repl.aux = strlen(repl.string);
	}

	diff = repl.aux - test.aux;
	val.bits = 0;

	while (off < thisVal.aux - test.aux)
		if (!memcmp(thisVal.str + off, test.str, test.aux)) {
			val.bits = vt_string;
			val.aux = thisVal.aux + diff;
			val.str = js_alloc(val.aux + 1, false);
			val.str[val.aux] = 0;
			val.refcount = 1;
			memcpy(val.str, thisVal.str, off);
			memcpy(val.str + off, repl.str, repl.aux);
			memcpy(val.str + off + repl.aux, thisVal.str + off + test.aux, thisVal.aux - off - test.aux);
			break;
		} else
			off++;

	if (!val.bits)
		val = test;
	else
		abandonValue(test);

	abandonValue(repl);
	return val;
}

value_t fcnStrStartsWith(value_t *args, value_t thisVal) {
	value_t test, off, val;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false);
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
	  if (off.nval < thisVal.aux - test.aux)
		val.boolean = !memcmp(thisVal.str + off.nval, test.str, test.aux);

	abandonValue(test);
	return val;
}

value_t fcnStrIndexOf(value_t *args, value_t thisVal) {
	value_t test, off, val;

	val.bits = vt_int;
	val.nval = -1;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false);
	else
		return val;

	if (vec_count(args) > 1)
		off = conv2Str(args[1], false);
	else {
		off.bits = vt_int;
		off.nval = 0;
	}

	if (off.type != vt_int)
		return val;

	if (off.nval < 0)
		off.nval = 0;

	while (off.nval < thisVal.aux - test.aux)
		if (!memcmp(thisVal.str + off.nval, test.str, test.aux)) {
			val.nval = off.nval;
			break;
		} else
			off.nval++;

	abandonValue(test);
	return val;
}

value_t fcnStrIncludes(value_t *args, value_t thisVal) {
	value_t test, off, val;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_count(args) > 0)
		test = conv2Str(args[0], false);
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

	while (off.nval < thisVal.aux - test.aux)
		if ((val.boolean = !memcmp(thisVal.str + off.nval, test.str, test.aux)))
			break;
		else
			off.nval++;

	abandonValue(test);
	return val;
}

value_t fcnStrEndsWith(value_t *args, value_t thisVal) {
	value_t test, len, val;
	int off;

	val.bits = vt_bool;
	val.boolean = false;

	if (vec_count(args) > 0)
		test = conv2Str(args[1], false);
	else
		return val;

	if (vec_count(args) > 1)
		len = conv2Int(args[1], false);
	else {
		len.bits = vt_int;
		len.nval = thisVal.aux;
	}

	off = len.nval - test.aux;

	if (off < 0)
		val.boolean = false;
	else
		val.boolean = !memcmp(thisVal.str + off, test.str, test.aux);

	abandonValue(test);
	return val;
}

value_t fcnStrCharCodeAt(value_t *args, value_t thisVal) {
	value_t idx, val;

	if (vec_count(args) > 0)
		idx = conv2Int(args[0], false);
	else
		idx.bits = vt_undef;

	if (idx.type != vt_int) {
		idx.bits = vt_int;
		idx.nval = 0;
	}

	if (idx.nval < 0 || idx.nval > thisVal.aux)
		return val.bits = vt_nan, val;

	val.bits = vt_int;
	val.nval = thisVal.str[idx.nval];
	return val;
}

value_t fcnStrCharAt(value_t *args, value_t thisVal) {
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

	if (idx.nval < thisVal.aux)
		val = newString(thisVal.str + idx.nval, 1);

	return val;
}

value_t fcnBoolToString(value_t *args, value_t thisVal) {
	value_t val;

	val.bits = vt_string;

	if (thisVal.boolean)
		val.string = "true", val.aux = 4;
	else
		val.string = "false", val.aux = 5;

	return val;
}

value_t fcnBoolValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnNumValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnNumToString(value_t *args, value_t thisVal) {
	return conv2Str(thisVal, false);
}

value_t fcnNumToPrecision(value_t *args, value_t thisVal) {
	value_t digits;
	char buff[512];
	double dbl;
	int len;

	if (thisVal.type == vt_int)
		dbl = thisVal.nval;

	if (thisVal.type == vt_dbl)
		dbl = thisVal.dbl;

	if (vec_count(args) > 0)
		digits = conv2Int(args[0], false);
	else {
		digits.bits = vt_int;
		digits.nval = 15;
	}

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%.*e", (int)digits.nval, dbl);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.*e", (int)digits.nval, dbl);
#endif
	return newString(buff, len);
}

value_t fcnNumToFixed(value_t *args, value_t thisVal) {
	value_t digits;
	char buff[512];
	double dbl;
	int len;

	if (thisVal.type == vt_int)
		dbl = thisVal.nval;

	if (thisVal.type == vt_dbl)
		dbl = thisVal.dbl;

	if (vec_count(args) > 0)
		digits = conv2Int(args[0], false);
	else {
		digits.bits = vt_int;
		digits.nval = 15;
	}

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%.*e", (int)digits.nval, dbl);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.*e", (int)digits.nval, dbl);
#endif
	return newString(buff, len);
}

value_t fcnNumToExponential(value_t *args, value_t thisVal) {
	value_t digits;
	char buff[512];
	double dbl;
	int len;

	if (thisVal.type == vt_int)
		dbl = thisVal.nval;

	if (thisVal.type == vt_dbl)
		dbl = thisVal.dbl;

	if (vec_count(args) > 0)
		digits = conv2Int(args[0], false);
	else {
		digits.bits = vt_int;
		digits.nval = 15;
	}

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%.*e", (int)digits.nval, dbl);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.*e", (int)digits.nval, dbl);
#endif
	return newString(buff, len);
}

struct PropVal builtinObjProp[] = {
	{ propObjLength, "length"},
	{ propObjProto, "prototype" },
	{ NULL, NULL}
};

struct PropVal builtinStrProp[] = {
	{ propStrLength, "length"},
	{ NULL, NULL}
};

struct PropVal builtinBoolProp[] = {
	{ propBoolLength, "length" },
	{ NULL, NULL}
};

struct PropVal builtinArrayProp[] = {
	{ propArrayLength, "length" },
	{ NULL, NULL}
};

struct PropVal builtinNumProp[] = {
	{ NULL, NULL}
};

extern struct PropVal builtinDateProp[];

struct PropVal builtinFcnProp[] = {
	{ propFcnProto, "prototype" },
	{ propFcnDisplayName, "displayName" },
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

struct PropFcn builtinObjFcns[] = {
//	{ fcnObjectLock, "lock" },
//	{ fcnObjectUnlock, "unlock" },
	{ fcnObjectToString, "toString" },
	{ fcnObjectSetBaseVal, "__setBaseVal" },
	{ fcnObjectValueOf, "valueOf" },
	{ NULL, NULL}
};

struct PropFcn builtinArrayFcns[] = {
//	{ fcnArrayLock, "lock" },
//	{ fcnArrayUnlock, "unlock" },
	{ fcnArrayToString, "toString" },
	{ fcnArraySetBaseVal, "__setBaseVal" },
	{ fcnArrayValueOf, "valueOf" },
	{ fcnArrayConcat, "concat" },
	{ fcnArraySlice, "slice" },
	{ fcnArrayJoin, "join" },
	{ NULL, NULL}
};

struct PropFcn builtinNumFcns[] = {
	{ fcnNumToString, "toString" },
	{ fcnNumToExponential, "toExponential" },
	{ fcnNumToFixed, "toFixed" },
	{ fcnNumToPrecision, "toPrecision" },
	{ NULL, NULL}
};

struct PropFcn builtinBoolFcns[] = {
	{ fcnBoolValueOf, "valueOf" },
	{ fcnBoolToString, "toString" },
	{ NULL, NULL}
};

extern struct PropFcn builtinDateFcns[];

struct PropVal *builtinProp[] = {
	builtinStrProp,
	builtinObjProp,
	builtinArrayProp,
	builtinNumProp,
	builtinBoolProp,
	builtinDateProp,
	builtinFcnProp,
	NULL
};

struct PropFcn *builtinFcn[] = {
	builtinStrFcns,
	builtinObjFcns,
	builtinArrayFcns,
	builtinNumFcns,
	builtinBoolFcns,
	builtinDateFcns,
	NULL,
	NULL
};

//  install built-in properties into system level object

value_t js_installProps(uint32_t args, environment_t *env) {
	struct PropVal *proptbl;
	struct PropFcn *fcntbl;
	value_t table, obj, s;
	value_t name;
	value_t fcn;

	s.bits = vt_status;

	obj = eval_arg(&args, env);

	if (vt_closure != obj.type) {
		fprintf(stderr, "Error: installProps => expecting closure => %s\n", strtype(obj.type));
		return s.status = ERROR_script_internal, s;
	}

	table = eval_arg(&args, env);

	if (vt_int != table.type) {
		fprintf(stderr, "Error: installProps => expecting int => %s\n", strtype(table.type));
		return s.status = ERROR_script_internal, s;
	}

	if (table.nval >= sizeof(builtinProp) / sizeof(void *)) {
		fprintf(stderr, "Error: installProps => expecting int < 6 => %" PRIi64 "\n", table.nval);
		return s.status = ERROR_script_internal, s;
	}

	if ((proptbl = builtinProp[table.nval]))
	  while (proptbl->fcn) {
		name.bits = vt_string;
		name.string = proptbl->name;
		name.aux = strlen(name.string);

		fcn.bits = vt_propval;
		fcn.propval = proptbl->fcn;

		*lookup(obj.closure->proto, name, true) = fcn;
		proptbl++;
	  }
	
	if ((fcntbl = builtinFcn[table.nval]))
	  while (fcntbl->fcn) {
		name.bits = vt_string;
		name.string = fcntbl->name;
		name.aux = strlen(name.string);

		fcn.bits = vt_propfcn;
		fcn.propfcn = fcntbl->fcn;

		*lookup(obj.closure->proto, name, true) = fcn;
		fcntbl++;
	  }
	
	if (args) for(;;) {
		value_t v = eval_arg(&args, env);

		if (v.type == vt_endlist)
			break;

		if (v.type == vt_int)
			builtinObj[v.nval] = obj;
	}

	s.status = OK;
	return s;
}
