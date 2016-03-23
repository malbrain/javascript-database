#include <stdlib.h>
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
	num.nval = vec_count(val.aval);
	return num;
}

value_t fcnStrValueOf(uint32_t args, environment_t *env) {
	return env->propBase;
}

value_t fcnStrToString(uint32_t args, environment_t *env) {
	return env->propBase;
}

value_t fcnStrSplit(uint32_t args, environment_t *env) {
	value_t delim = eval_arg(&args, env);
	value_t limit = eval_arg(&args, env);
	value_t s = env->propBase;
	value_t val = newArray();
	int off, count, prev, max;
	
	if (delim.type == vt_endlist) {
		vec_push(val.aval->array, env->propBase);
		return val;
	}

	delim = conv2Str(delim);
	max = s.aux - delim.aux;
	prev = 0;

	for (off = 0; off < max; off++) {
		if (!delim.aux || !memcmp(s.str+off, delim.str, delim.aux)) {
			value_t v = newString(s.str + prev, off - prev);
			vec_push(val.aval->array, v);
			off += delim.aux;
			prev = off;
		}
	}

	if (prev < s.aux) {
		value_t v = newString(s.str + prev, s.aux - prev);
		vec_push(val.aval->array, v);
	}

	return val;
}

value_t fcnStrConcat(uint32_t args, environment_t *env) {
	uint32_t length = env->propBase.aux, off, idx;
	value_t *strings = NULL, val;

	if (args)
	  while (true) {
		value_t n, v = eval_arg(&args, env);
		if (v.type == vt_endlist)
			break;
		n = conv2Str(v);
		vec_push(strings, n);
		length += n.aux;
	  }

	val.bits = vt_string;
	val.str = jsdb_alloc(length, false);
	val.aux = length;

	off = env->propBase.aux;
	memcpy(val.str, env->propBase.str, off);

	for (idx = 0; idx < vec_count(strings); idx++) {
		memcpy(val.str + off, strings[idx].str, strings[idx].aux);
		off += strings[idx].aux;
	}

	vec_free(strings);
	return val;
}

value_t fcnStrRepeat(uint32_t args, environment_t *env) {
	value_t count = conv2Int(eval_arg(&args, env));
	value_t val;
	int off, len;

	off = len = env->propBase.aux;

	val.bits = vt_string;
	val.aux = len * count.nval;
	val.str = jsdb_alloc(val.aux, false);
	memcpy(val.str, env->propBase.str, len);

	while (off < val.aux) {
		memcpy(val.str + off, env->propBase.str, len);
		off += len;
	}

	return val;
}

value_t fcnStrLastIndexOf(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	value_t offset = eval_arg(&args, env);
	value_t val;
	int start;

	if (offset.type == vt_endlist)
		start = env->propBase.aux - test.aux;
	else
		start = conv2Int(offset).nval;

	val.bits = vt_int;
	val.nval = -1;

	if (start < 0)
		return val;

	while (start >= 0)
		if (!memcmp(env->propBase.str + start, test.str, test.aux))
			return val.nval = start, val;
		else
			start++;

	return val;
}

value_t fcnStrReplaceAll(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	value_t repl = conv2Str(eval_arg(&args, env));
	int off = 0, diff = 0, idx, prev;
	uint32_t *matches = NULL;
	value_t val;

	while (off <= env->propBase.aux - test.aux)
		if (!memcmp(env->propBase.str + off, test.str, test.aux)) {
			vec_push(matches, off);
			diff += repl.aux - test.aux;
			off += test.aux;
		} else
			off++;

	if (vec_count(matches) == 0)
		return env->propBase;

	val.bits = vt_string;
	val.str = jsdb_alloc(env->propBase.aux + diff, false);
	val.aux = 0;
	prev = 0;

	for (idx = 0; idx < vec_count(matches); idx++) {
		memcpy(val.str + val.aux, env->propBase.str + prev, matches[idx] - prev);
		val.aux += matches[idx] - prev;
		memcpy(val.str + val.aux, repl.str, repl.aux);
		val.aux += repl.aux;
		prev = matches[idx] + test.aux;
	}

	memcpy(val.str + val.aux, env->propBase.str + prev, env->propBase.aux - prev);
	val.aux += env->propBase.aux - prev;

	assert(val.aux == env->propBase.aux + diff);
	return val;
}

value_t fcnStrSubstring(uint32_t args, environment_t *env) {
	int off = conv2Int(eval_arg(&args, env)).nval;
	value_t endidx = eval_arg(&args, env);
	int count, end;
	value_t val;

	if (off < 0)
		off = 0;

	if (endidx.type == vt_endlist)
		end = env->propBase.aux;
	else
		end = conv2Int(endidx).nval;

	if (off > env->propBase.aux)
		off = env->propBase.aux;

	if (end > env->propBase.aux)
		end = env->propBase.aux;

	if (off < 0)
		off = 0;

	if (end < 0)
		end = 0;

	if (end > off)
		return newString(env->propBase.str + off, end - off);

	if (end < off)
		return newString(env->propBase.str + end, off - end);

	val.bits = vt_string;
	val.aux = 0;
	return val;
}

value_t fcnStrTrim(uint32_t args, environment_t *env) {
	int len = env->propBase.aux, start = 0;
	value_t val;
	int idx;

	while (len)
		if (isspace(env->propBase.str[len - 1]))
			len--;
		else
			break;
 
	while (start < len)
		if (isspace(env->propBase.str[start]))
			start++;
		else
			break;
 
	return newString(env->propBase.str + start, len - start);
}

value_t fcnStrToUpperCase(uint32_t args, environment_t *env) {
	value_t val;
	int idx;

	val = newString(env->propBase.str, env->propBase.aux);

	for (idx = 0; idx < val.aux; idx++)
		val.str[idx] = toupper(val.str[idx]);

	return val;
}

value_t fcnStrToLowerCase(uint32_t args, environment_t *env) {
	value_t val;
	int idx;

	val = newString(env->propBase.str, env->propBase.aux);

	for (idx = 0; idx < val.aux; idx++)
		val.str[idx] = tolower(val.str[idx]);

	return val;
}

value_t fcnStrSubstr(uint32_t args, environment_t *env) {
	int off = conv2Int(eval_arg(&args, env)).nval;
	value_t cnt = eval_arg(&args, env);
	value_t val;
	int count;

	val.bits = vt_string;
	val.aux = 0;

	if (off < 0)
		off = env->propBase.aux + off;

	if (cnt.type == vt_endlist)
		count = env->propBase.aux - off;
	else
		count = conv2Int(cnt).nval;

	if (count > env->propBase.aux)
		count = env->propBase.aux;

	if (count > 0)
		return newString(env->propBase.str + off, count);

	return val;
}

value_t fcnStrSlice(uint32_t args, environment_t *env) {
	int slice = conv2Int(eval_arg(&args, env)).nval;
	int end = conv2Int(eval_arg(&args, env)).nval;
	int count, start;
	value_t val;

	if (end > env->propBase.aux || !end)
		end = env->propBase.aux;

	if (slice < 0) {
		start = env->propBase.aux + slice;
		count = -slice;
	} else {
		start = slice;
		count = end - start;
	}

	if (count > 0)
		return newString(env->propBase.str + start, count);

	val.bits = vt_null;
	return val;
}

value_t fcnStrReplace(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	value_t repl = conv2Str(eval_arg(&args, env));
	int off = 0, diff = repl.aux - test.aux;
	value_t val;

	while (off < env->propBase.aux - test.aux)
		if (!memcmp(env->propBase.str + off, test.str, test.aux)) {
			val.bits = vt_string;
			val.aux = env->propBase.aux + diff;
			val.str = jsdb_alloc(val.aux, false);
			memcpy(val.str, env->propBase.str, off);
			memcpy(val.str + off, repl.str, repl.aux);
			memcpy(val.str + off + repl.aux, env->propBase.str + off + test.aux, env->propBase.aux - off - test.aux);
			return val;
		} else
			off++;

	return env->propBase;
}

value_t fcnStrStartsWith(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	int off = conv2Int(eval_arg(&args, env)).nval;
	value_t val;

	val.bits = vt_bool;
	val.boolean = false;

	if (off < 0)
		return val;

	if (off < env->propBase.aux - test.aux)
		val.boolean = !memcmp(env->propBase.str + off, test.str, test.aux);

	return val;
}

value_t fcnStrIndexOf(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	value_t offset = eval_arg(&args, env);
	value_t val;
	int off;

	if (offset.type == vt_endlist)
		off = 0;
	else
		off = conv2Int(offset).nval;

	val.bits = vt_int;
	val.nval = -1;

	if (off < 0)
		return val;

	while (off < env->propBase.aux - test.aux)
		if (!memcmp(env->propBase.str + off, test.str, test.aux))
			return val.nval = off, val;
		else
			off++;

	return val;
}

value_t fcnStrIncludes(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	value_t offset = eval_arg(&args, env);
	value_t val;
	int off;

	if (offset.type == vt_endlist)
		off = 0;
	else
		off = conv2Int(offset).nval;

	val.bits = vt_bool;
	val.boolean = false;

	if (off < 0)
		return val;

	while (off < env->propBase.aux - test.aux)
		if (val.boolean = !memcmp(env->propBase.str + off, test.str, test.aux))
			return val;
		else
			off++;

	return val;
}

value_t fcnStrEndsWith(uint32_t args, environment_t *env) {
	value_t test = conv2Str(eval_arg(&args, env));
	value_t length = eval_arg(&args, env);
	value_t val;
	int off, len;

	if (length.type == vt_endlist)
		len = env->propBase.aux;
	else
		len = conv2Int(length).nval;

	val.bits = vt_bool;
	off = len - test.aux;

	if (off < 0)
		val.boolean = false;
	else
		val.boolean = !memcmp(env->propBase.str + off, test.str, test.aux);

	return val;
}

value_t fcnStrCharAt(uint32_t args, environment_t *env) {
	uint64_t idx = conv2Int(eval_arg(&args, env)).nval;
	value_t val;

	if (env->propBase.type == vt_string && idx < env->propBase.aux)
		return newString(env->propBase.str + idx, 1);

	val.bits = vt_null;
	return val;
}

value_t fcnIntValueOf(uint32_t args, environment_t *env) {
	return env->propBase;
}

value_t fcnIntToString(uint32_t args, environment_t *env) {
	return conv2Str(env->propBase);
}

value_t fcnIntToExponential(uint32_t args, environment_t *env) {
	uint64_t digits = conv2Int(eval_arg(&args, env)).nval;
	double dbl = env->propBase.nval;
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
{ fcnIntToExponential, "toExponential", vt_int },
{ fcnStrToString, "toString", vt_string },
{ fcnIntToString, "toString", vt_int },
/*
{ fcnIntToFixed, "toFixed", vt_int },
{ fcnIntToPrecision, "toPrecision", vt_int },
{ fcnIntValueOf, "valueOf", vt_int },
{ fcnDblToString, "toString", vt_dbl },
{ fcnDblToExponential, "toExponential", vt_dbl },
{ fcnDblToFixed, "toFixed", vt_dbl },
{ fcnDblToPrecision, "toPrecision", vt_dbl },
{ fcnDblValueOf, "valueOf", vt_dbl },
{ fcnBoolValueOf, "valueOf", vt_bool },
{ fcnBoolToString, "toString", vt_bool },
{ fcnBoolValueOf, "valueOf", vt_bool },
{ fcnBoolToString, "toString", vt_bool },
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
{ propArrayLength, "length", vt_array }
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
	uint64_t h, start;
	value_t val;
	int idx;

	h = start = hashStr(field) * obj.type % PROP_valhash;

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

	h = start;

	while ((idx = propFcnHash[h])) {
	  struct PropFcn *prop = &builtinPropFcns[idx - 1];

	  if (prop->type == obj.type)
		if (field.aux == strlen(prop->name))
			if (!memcmp(field.str, prop->name, field.aux)) {
				env->propBase = obj;
				val.bits = vt_propfcn;
				val.propfcn = prop->fcn;
				return val;
			}

	  h = (h+1) % PROP_valhash;

	  if (h == start)
		break;
	}

	val.bits = vt_null;
	return val;
}

