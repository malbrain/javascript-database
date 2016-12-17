#include <ctype.h>
#include "js.h"
#include "js_props.h"

value_t builtinProto[vt_MAX];

value_t propBoolLength(value_t val, bool lVal) {
	value_t len;

	len.bits = vt_int;
	len.nval = 1;
	return len;
}

value_t propFcnLength(value_t val, bool lVal) {
	value_t len;

	len.bits = vt_int;
	len.nval = val.closure->fd->nparams;
	return len;
}

value_t propFcnName(value_t val, bool lVal) {
	stringNode *sn;
	symNode *sym;
	value_t obj;

	if (!val.closure->fd->name)
		return obj.bits = vt_undef, obj;

	sym = (symNode *)(val.closure->table + val.closure->fd->name);
	sn = (stringNode *)(val.closure->table + sym->name);

	obj.bits = vt_string;
	obj.string = sn->string;
	obj.aux = sn->hdr->aux;
	return obj;
}

value_t propFcnDisplayName(value_t val, bool lVal) {
	stringNode *sn;
	symNode *sym;
	value_t obj;

	if (!val.closure->fd->name)
		return obj.bits = vt_undef, obj;

	sym = (symNode *)(val.closure->table + val.closure->fd->name);
	sn = (stringNode *)(val.closure->table + sym->name);

	obj.bits = vt_string;
	obj.string = sn->string;
	obj.aux = sn->hdr->aux;
	return obj;
}

value_t fcnFcnApply(value_t *args, value_t thisVal) {
	value_t arguments = newArray(array_value);

	arguments.aval->values = args + 1;
	return fcnCall(thisVal, arguments, args[0]);
}

value_t fcnFcnCall(value_t *args, value_t thisVal) {
	value_t arguments = newArray(array_value);
	value_t thisValue = args[0];

	arguments.aval->values = vec_slice(args, 1);
	return fcnCall(thisVal, arguments, thisValue);
}

value_t propFcnProto(value_t val, bool lVal) {
	value_t ref;

	if (!lVal)
		return val.closure->protoObj;

	ref.bits = vt_lval;
	ref.lval = &val.closure->protoObj;
	return ref;
}

value_t fcnBoolValueOf(value_t *args, value_t thisVal) {
	return thisVal;
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

value_t fcnNumToString(value_t *args, value_t thisVal) {
	value_t val, s;
	char buff[64];
	int len = 0;

	switch (thisVal.type) {
	  case vt_int:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%" PRIi64, thisVal.nval);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%" PRIi64, thisVal.nval);
#endif
		break;

	  case vt_dbl:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%.16G", thisVal.dbl);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.16G", thisVal.dbl);
#endif
		if (!(thisVal.dbl - (uint64_t)thisVal.dbl))
			buff[len++] = '.', buff[len++] = '0', buff[len] = 0;

		break;

	  case vt_infinite:
		val.bits = vt_string;

		if (thisVal.negative)
			val.string = "-Infinity", val.aux = 9;
		else
			val.string = "Infinity", val.aux = 8;

		return val;

	  case vt_null:
		val.bits = vt_string;
		val.string = "null";
		val.aux = 4;
		return val;

	  case vt_nan:
		val.bits = vt_string;
		val.string = "NaN";
		val.aux = 3;
		return val;

	  default:
		fprintf(stderr, "Error: NumberToString => invalid type: %s\n", strtype(thisVal.type));
		return s.status = ERROR_script_internal, s;
	}

	if (len > sizeof(buff))
		len = sizeof(buff);

	return newString(buff, len);
}

value_t fcnNumValueOf(value_t *args, value_t thisVal) {
	return thisVal;
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

extern struct PropFcn builtinDateFcns[];

extern struct PropVal builtinStrProp[];

extern struct PropVal builtinObjProp[];

extern struct PropVal builtinArrayProp[];

struct PropVal builtinBoolProp[] = {
	{ propBoolLength, "length" },
	{ NULL, NULL}
};

struct PropVal builtinNumProp[] = {
	{ NULL, NULL}
};

extern struct PropVal builtinDateProp[];

struct PropVal builtinFcnProp[] = {
	{ propFcnName, "name" },
	{ propFcnLength, "length" },
	{ propFcnProto, "prototype", true },
	{ propFcnDisplayName, "displayName" },
	{ NULL, NULL}
};

extern struct PropFcn builtinStrFcns[];

extern struct PropFcn builtinObjFcns[];

extern struct PropFcn builtinArrayFcns[];

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

struct PropFcn builtinFcnFcns[] = {
	{ fcnFcnApply, "apply" },
	{ fcnFcnCall, "call" },
	{ NULL, NULL}
};

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
	builtinFcnFcns,
	NULL,
	NULL
};

char *builtinNames[] = {
	"Function.prototype.",
	"Object.prototype.",
	"Array.prototype.",
	"Number.prototype.",
	"Boolean.prototype.",
	"Date.prototype.",
	"Functions.prototype."
};

//  install built-in properties into system level object

value_t js_installProps(uint32_t args, environment_t *env) {
	struct PropVal *proptbl;
	struct PropFcn *fcntbl;
	value_t table, obj, s;
	object_t *object;
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
		if (proptbl->isBase)
			object = obj.closure->obj.oval;
		else
			object = obj.closure->protoObj.oval;

		name.bits = vt_string;
		name.string = proptbl->name;
		name.aux = strlen(name.string);

		fcn.bits = vt_propval;
		fcn.nval = (struct PropVal *)proptbl - builtinProp[table.nval];
		fcn.subType = table.nval;

		replaceSlot(lookup(object, name, true, false), fcn);
		proptbl++;
	  }
	
	if ((fcntbl = builtinFcn[table.nval]))
	  while (fcntbl->fcn) {
		if (fcntbl->isBase)
			object = obj.closure->obj.oval;
		else
			object = obj.closure->protoObj.oval;

		name.bits = vt_string;
		name.string = fcntbl->name;
		name.aux = strlen(name.string);

		fcn.bits = vt_propfcn;
		fcn.nval = (struct PropFcn *)fcntbl - builtinFcn[table.nval];
		fcn.subType = table.nval;

		replaceSlot(lookup(object, name, true, false), fcn);
		fcntbl++;
	  }
	
	if (args) for(;;) {
		value_t v = eval_arg(&args, env);

		if (v.type == vt_endlist)
			break;

		if (v.type == vt_int)
			builtinProto[v.nval] = obj.closure->protoObj;
	}

	s.status = OK;
	return s;
}

value_t getPropFcnName(value_t fcn) {
	value_t name, ans[1];

	name.bits = vt_string;

	if (fcn.type == vt_propfcn)
		name.string = builtinFcn[fcn.subType][fcn.nval].name;
	else
		name.string = builtinProp[fcn.subType][fcn.nval].name;

	name.aux = strlen(name.string);

	ans->bits = vt_string;
	ans->string = builtinNames[fcn.subType];
	ans->aux = strlen(ans->string);

	valueCat(ans, name);
	return *ans;
}

value_t callObjFcn(value_t original, char *name, bool abandon) {
	value_t prop, *fcn, obj = original, result, args;
	bool noProtoChain;

	args.bits = vt_undef;

	prop.bits = vt_string;
	prop.string = name;
	prop.aux = strlen(name);

	if (obj.objvalue)
		obj = *obj.lval;

	if ((noProtoChain = obj.type != vt_object))
	  if (builtinProto[obj.type].type == vt_object)
		obj = builtinProto[obj.type];

	if (obj.type == vt_object)
	 if ((fcn = lookup(obj.oval, prop, false, noProtoChain)))
	  switch (fcn->type) {
	  case vt_closure:
		result = fcnCall(*fcn, args, original);
		if (abandon)
			abandonValue(original);
		return result;
	  case vt_propfcn:
		result = (builtinFcn[fcn->subType][fcn->nval].fcn)(NULL, original);
		if (abandon)
			abandonValue(original);
		return result;
	  default:
		fprintf(stderr, "Error: callObjFcn => invalid type: %s\n", strtype(obj.type));
	  }

	result.bits = vt_string;
	result.str = strtype(original.type);
	result.aux = strlen(result.str);
	return result;
}

value_t callFcnProp(value_t prop, value_t arg, bool lVal) {
	return (builtinProp[prop.subType][prop.nval].fcn)(arg, lVal);
}

value_t callFcnFcn(value_t fcn, value_t *args, value_t thisVal) {
	return (builtinFcn[fcn.subType][fcn.nval].fcn)(args, thisVal);
}
