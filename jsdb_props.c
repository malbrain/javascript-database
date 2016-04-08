#include <stdlib.h>
#include <ctype.h>
#include "jsdb.h"

#define PROP_fcnhash 512
#define PROP_valhash 512

value_t propStrLength(value_t val) {
	value_t num;

	num.bits = vt_int;
	num.nval = val.aux;
	return num;
}

value_t propArrayLength(value_t val) {
	value_t num;

	num.bits = vt_int;
	num.nval = vec_count(val.aval->array);
	return num;
}

value_t propIntNop(value_t val) {
	return val;
}

value_t propPrototype(value_t val) {
	closure_t *closure = val.closure;
	value_t result;

	if(!val.lvalue)
		return closure->proto;

	result.bits = vt_lval;
	result.lval = &closure->proto;
	return result;
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

value_t fcnObjectLock(value_t *args, value_t thisVal) {
	value_t val, lockMode = args[0];
	object_t *object = thisVal.oval;
	int mode = conv2Int(lockMode, false).nval;

	val.bits = vt_undef;

	switch (mode) {
	case 0:	readLock(object->lock); break;
	case 1:	writeLock(object->lock); break;
	}

	return val;
}

value_t fcnObjectUnlock(value_t *args, value_t thisVal) {
	object_t *object = thisVal.oval;
	value_t val;

	val.bits = vt_undef;

	rwUnlock(object->lock);
	return val;
}

value_t fcnArrayToString(value_t *args, value_t thisVal) {
	return conv2Str(thisVal, false);
}

value_t fcnArrayLock(value_t *args, value_t thisVal) {
	value_t val, lockMode = args[0];
	array_t *array = thisVal.aval;
	int mode = conv2Int(lockMode, false).nval;

	val.bits = vt_undef;

	switch (mode) {
	case 0:	readLock(array->lock); break;
	case 1:	writeLock(array->lock); break;
	}

	return val;
}

value_t fcnArrayUnlock(value_t *args, value_t thisVal) {
	array_t *array = thisVal.aval;
	value_t val;

	val.bits = vt_undef;

	rwUnlock(array->lock);
	return val;
}

value_t fcnStrValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnStrToString(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnStrSplit(value_t *args, value_t thisVal) {
	int off, count, prev, max, limit;
	value_t delimVal, limitVal;
	value_t s = thisVal;
	value_t val = newArray();
	value_t delim;
	
	delimVal = args[0];
	limitVal = args[1];

	if (delimVal.type == vt_endlist) {
		vec_push(val.aval->array, thisVal);
		incrRefCnt(thisVal);
		return val;
	}

	delim = conv2Str(delimVal, false);
	limit = conv2Int(limitVal, false).nval;

	if (limitVal.type == vt_endlist)
		limit = 1024 * 1024;

	max = s.aux - delim.aux;
	count = 0;
	prev = 0;

	for (off = 0; count < limit && off < max; off++) {
		if (!delim.aux || !memcmp(s.str+off, delim.str, delim.aux)) {
			value_t v = newString(s.str + prev, off - prev);
			vec_push(val.aval->array, v);
			incrRefCnt(v);
			off += delim.aux;
			prev = off;
			count++;
		}
	}

	if (count < limit && prev < s.aux) {
		value_t v = newString(s.str + prev, s.aux - prev);
		vec_push(val.aval->array, v);
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
	val.str = jsdb_alloc(length + 1, false);
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
	value_t cntVal = args[0];
	int count = conv2Int(cntVal, false).nval;
	value_t val;
	int off, len;

	off = len = thisVal.aux;

	val.bits = vt_string;
	val.aux = len * count;
	val.str = jsdb_alloc(val.aux + 1, false);
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
	value_t testVal = args[0];
	value_t offset = args[1];
	value_t test = conv2Str(testVal, false);
	value_t val;
	int start;

	if (offset.type == vt_endlist)
		start = thisVal.aux - test.aux;
	else
		start = conv2Int(offset, false).nval;

	val.bits = vt_int;
	val.nval = -1;

	while (start >= 0)
		if (!memcmp(thisVal.str + start, test.str, test.aux))
			return val.nval = start, val;
		else
			start++;

	if (testVal.type != vt_string)
		abandonValue(test);

	return val;
}

value_t fcnStrReplaceAll(value_t *args, value_t thisVal) {
	value_t testVal = args[0];
	value_t replVal = args[1];
	value_t test = conv2Str(testVal, false);
	value_t repl = conv2Str(replVal, false);
	int off = 0, diff = 0, idx, prev;
	uint32_t *matches = NULL;
	value_t val;

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
		val.str = jsdb_alloc(thisVal.aux + diff + 1, false);
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

	if (testVal.type != vt_string)
		abandonValue(test);

	if (replVal.type != vt_string)
		abandonValue(repl);

	vec_free(matches);
	return val;
}

value_t fcnStrSubstring(value_t *args, value_t thisVal) {
	value_t offVal = args[0];
	value_t endVal = args[1];
	int off = conv2Int(offVal, false).nval;
	int count, end;
	value_t val;

	val.bits = vt_string;
	val.aux = 0;

	if (off < 0)
		off = 0;

	if (endVal.type == vt_endlist)
		end = thisVal.aux;
	else
		end = conv2Int(endVal, false).nval;

	if (off > thisVal.aux)
		off = thisVal.aux;

	if (end > thisVal.aux)
		end = thisVal.aux;

	if (off < 0)
		off = 0;

	if (end < 0)
		end = 0;

	if (end > off)
		val = newString(thisVal.str + off, end - off);

	if (end < off)
		val = newString(thisVal.str + end, off - end);

	return val;
}

value_t fcnStrTrim(value_t *args, value_t thisVal) {
	int len = thisVal.aux, start = 0;
	value_t val;
	int idx;

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
	value_t offVal = args[0];
	value_t cntVal = args[1];
	int off = conv2Int(offVal, false).nval;
	value_t val;
	int count;

	val.bits = vt_string;
	val.aux = 0;

	if (off < 0)
		off = thisVal.aux + off;

	if (cntVal.type == vt_endlist)
		count = thisVal.aux - off;
	else
		count = conv2Int(cntVal, false).nval;

	if (count > thisVal.aux)
		count = thisVal.aux;

	if (count > 0)
		val = newString(thisVal.str + off, count);

	return val;
}

value_t fcnStrSlice(value_t *args, value_t thisVal) {
	value_t sliceVal = args[0];
	value_t endVal = args[1];
	int slice = conv2Int(sliceVal, false).nval;
	int end = conv2Int(endVal, false).nval;
	int count, start;
	value_t val;

	val.bits = vt_null;

	if (end > thisVal.aux || !end)
		end = thisVal.aux;

	if (slice < 0) {
		start = thisVal.aux + slice;
		count = -slice;
	} else {
		start = slice;
		count = end - start;
	}

	if (count > 0)
		val = newString(thisVal.str + start, count);

	return val;
}

value_t fcnStrReplace(value_t *args, value_t thisVal) {
	value_t testVal = args[0];
	value_t replVal = args[1];
	value_t test = conv2Str(testVal, false);
	value_t repl = conv2Str(testVal, false);
	int off = 0, diff = repl.aux - test.aux;
	value_t val;

	val = thisVal;

	while (off < thisVal.aux - test.aux)
		if (!memcmp(thisVal.str + off, test.str, test.aux)) {
			val.bits = vt_string;
			val.aux = thisVal.aux + diff;
			val.str = jsdb_alloc(val.aux + 1, false);
			val.str[val.aux] = 0;
			val.refcount = 1;
			memcpy(val.str, thisVal.str, off);
			memcpy(val.str + off, repl.str, repl.aux);
			memcpy(val.str + off + repl.aux, thisVal.str + off + test.aux, thisVal.aux - off - test.aux);
			break;
		} else
			off++;

	if (testVal.type != vt_string)
		abandonValue(test);

	if (replVal.type != vt_string)
		abandonValue(repl);

	return val;
}

value_t fcnStrStartsWith(value_t *args, value_t thisVal) {
	value_t testVal = args[0];
	value_t offVal = args[1];
	value_t test = conv2Str(testVal, false);
	int off = conv2Int(offVal, false).nval;
	value_t val;

	val.bits = vt_bool;
	val.boolean = false;

	if (off < 0)
		return val;

	if (off >=  0)
	  if (off < thisVal.aux - test.aux)
		val.boolean = !memcmp(thisVal.str + off, test.str, test.aux);

	if (testVal.type != vt_string)
		abandonValue(test);

	return val;
}

value_t fcnStrIndexOf(value_t *args, value_t thisVal) {
	value_t testVal = args[0];
	value_t offVal = args[1];
	value_t test = conv2Str(testVal, false);
	value_t val;
	int off;

	if (offVal.type == vt_endlist)
		off = 0;
	else
		off = conv2Int(offVal, false).nval;

	val.bits = vt_int;
	val.nval = -1;

	if (off < 0)
		return val;

	if (off >= 0)
	  while (off < thisVal.aux - test.aux)
		if (!memcmp(thisVal.str + off, test.str, test.aux)) {
			val.nval = off;
			break;
		} else
			off++;

	if (testVal.type != vt_string)
		abandonValue(test);

	return val;
}

value_t fcnStrIncludes(value_t *args, value_t thisVal) {
	value_t testVal = args[0];
	value_t offVal = args[1];
	value_t test = conv2Str(testVal, false);
	value_t val;
	int off;

	if (offVal.type == vt_endlist)
		off = 0;
	else
		off = conv2Int(offVal, false).nval;

	val.bits = vt_bool;
	val.boolean = false;

	if (off >= 0)
	  while (off < thisVal.aux - test.aux)
		if (val.boolean = !memcmp(thisVal.str + off, test.str, test.aux))
			break;
		else
			off++;

	if (testVal.type != vt_string)
		abandonValue(test);

	return val;
}

value_t fcnStrEndsWith(value_t *args, value_t thisVal) {
	value_t testVal = args[0];
	value_t lenVal = args[1];
	value_t test = conv2Str(testVal, false);
	value_t val;
	int off, len;

	if (lenVal.type == vt_endlist)
		len = thisVal.aux;
	else
		len = conv2Int(lenVal, false).nval;

	val.bits = vt_bool;
	off = len - test.aux;

	if (off < 0)
		val.boolean = false;
	else
		val.boolean = !memcmp(thisVal.str + off, test.str, test.aux);

	if (testVal.type != vt_string)
		abandonValue(test);

	return val;
}

value_t fcnStrCharAt(value_t *args, value_t thisVal) {
	value_t idxVal = args[0];
	uint64_t idx = conv2Int(idxVal, false).nval;
	value_t val;

	val.bits = vt_null;

	if (thisVal.type == vt_string && idx < thisVal.aux)
		val = newString(thisVal.str + idx, 1);

	return val;
}

value_t fcnBoolToString(value_t *args, value_t thisVal) {
	value_t val;

	val.bits = vt_string;
	if (thisVal.boolean)
		val.str = "true", val.aux = 4;
	else
		val.str = "false", val.aux = 5;

	return val;
}

value_t fcnBoolValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnDblValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnIntValueOf(value_t *args, value_t thisVal) {
	return thisVal;
}

value_t fcnIntToString(value_t *args, value_t thisVal) {
	return conv2Str(thisVal, false);
}

value_t fcnIntToExponential(value_t *args, value_t thisVal) {
	value_t digVal = args[0];
	double dbl = thisVal.nval;
	uint64_t digits = conv2Int(digVal, false).nval;
	value_t result;
	char buff[64];
	int len;

#ifndef _WIN32
	len = snprintf(buff, sizeof(buff), "%.*e", digits, dbl);
#else
	len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.*e", digits, dbl);
#endif
	return newString(buff, len);
}

/*
{ fcnIntToExponential, "toExponential", vt_int },
{ fcnIntToFixed, "toFixed", vt_int },
{ fcnIntToPrecision, "toPrecision", vt_int },
*/

uint16_t propFcnHash[PROP_fcnhash];

struct PropFcn {
	propFcnEval fcn;
	char *name;
	char type;
} builtinPropFcns[] = {
{ fcnStrCharAt, "charAt", vt_string },
{ fcnStrEndsWith, "endsWith", vt_string },
{ fcnStrIncludes, "includes", vt_string },
{ fcnStrIndexOf, "indexOf", vt_string },
{ fcnStrLastIndexOf, "lastIndexOf", vt_string },
{ fcnStrRepeat, "repeat", vt_string },
{ fcnStrSlice, "slice", vt_string },
{ fcnStrSubstr, "substr", vt_string },
{ fcnStrSubstr, "substr", vt_string },
{ fcnStrSubstring, "substring", vt_string },
{ fcnStrSplit, "split", vt_string },
{ fcnStrStartsWith, "startsWith", vt_string },
{ fcnStrReplace, "replace", vt_string },
{ fcnStrReplaceAll, "replaceAll", vt_string },
{ fcnStrToLowerCase, "toLowerCase", vt_string },
{ fcnStrToUpperCase, "toUpperCase", vt_string },
{ fcnStrTrim, "trim", vt_string },
{ fcnStrConcat, "concat", vt_string },
{ fcnStrValueOf, "valueOf", vt_string },
{ fcnStrToString, "toString", vt_string },
{ fcnIntToExponential, "toExponential", vt_int },
{ fcnIntToString, "toString", vt_int },
{ fcnIntValueOf, "valueOf", vt_int },
{ fcnObjectLock, "lock", vt_object },
{ fcnObjectUnlock, "unlock", vt_object },
{ fcnObjectToString, "toString", vt_object },
{ fcnObjectSetBaseVal, "__setBaseVal", vt_object },
{ fcnObjectValueOf, "valueOf", vt_object },
{ fcnArrayLock, "lock", vt_array },
{ fcnArrayUnlock, "unlock", vt_array },
{ fcnArrayToString, "toString", vt_array },
{ fcnDblValueOf, "valueOf", vt_dbl },
{ fcnBoolValueOf, "valueOf", vt_bool },
{ fcnBoolToString, "toString", vt_bool },
/*
{ fcnIntToFixed, "toFixed", vt_int },
{ fcnIntToPrecision, "toPrecision", vt_int },
{ fcnDblToString, "toString", vt_dbl },
{ fcnDblToExponential, "toExponential", vt_dbl },
{ fcnDblToFixed, "toFixed", vt_dbl },
{ fcnDblToPrecision, "toPrecision", vt_dbl },
*/
};

uint16_t propValHash[PROP_valhash];

typedef value_t (*propValEval)(value_t arg);

struct PropVal {
	propValEval fcn;
	char *name;
	char type;
} builtinPropVals[] = {
{ propStrLength, "length", vt_string },
{ propArrayLength, "length", vt_array },
{ propIntNop, "nop", vt_int },
{ propPrototype, "prototype", vt_closure }
};

void installProps () {
	uint64_t h, start;
	value_t name;
	int idx;

	for (idx = 0; idx < sizeof(builtinPropVals) / sizeof(builtinPropVals[0]); idx++) {
		struct PropVal *prop = &builtinPropVals[idx];
		name.aux = strlen(prop->name);
		name.str = prop->name;

		h = start = hashStr(name) * prop->type % PROP_valhash;

		while (propValHash[h])
			h = (h+1) % PROP_valhash;

		propValHash[h] = idx + 1;
	}
	for (idx = 0; idx < sizeof(builtinPropFcns) / sizeof(builtinPropFcns[0]); idx++) {
		struct PropFcn *prop = &builtinPropFcns[idx];
		name.aux = strlen(prop->name);
		name.str = prop->name;

		h = start = hashStr(name) * prop->type % PROP_fcnhash;

		while (propFcnHash[h])
			h = (h+1) % PROP_fcnhash;

		propFcnHash[h] = idx + 1;
	}
}

value_t builtinProp(value_t obj, value_t field, environment_t *env) {
	uint64_t hash = hashStr(field);
	uint64_t h, start;
	value_t val;
	int idx;

	h = start = hash * obj.type % PROP_valhash;

	while ((idx = propValHash[h])) {
	  struct PropVal *prop = &builtinPropVals[idx - 1];

	  if (prop->type == obj.type)
		if (field.aux == strlen(prop->name))
			if (!memcmp(field.str, prop->name, field.aux))
				return (prop->fcn)(obj);

	  h = (h+1) % PROP_valhash;

	  if (h == start)
		break;
	}

	h = start = hash * obj.type % PROP_fcnhash;

	while ((idx = propFcnHash[h])) {
	  struct PropFcn *prop = &builtinPropFcns[idx - 1];

	  if (prop->type == obj.type)
		if (field.aux == strlen(prop->name))
			if (!memcmp(field.str, prop->name, field.aux)) {
				replaceSlotValue(&env->framev[vec_count(env->framev) - 1]->nextThis, obj);
				val.bits = vt_propfcn;
				val.propfcn = prop->fcn;
				return val;
			}

	  h = (h+1) % PROP_fcnhash;

	  if (h == start)
		break;
	}

	val.bits = vt_undef;
	return val;
}

