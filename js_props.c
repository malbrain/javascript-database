#include <ctype.h>

#include "js.h"
#include "js_db.h"
#include "js_props.h"
#include "js_string.h"

value_t builtinProto[vt_MAX];
int builtinMap[vt_MAX];

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

	return newString(sn->str.val, sn->str.len);
}

value_t propFcnDisplayName(value_t val, bool lVal) {
	stringNode *sn;
	symNode *sym;
	value_t obj;

	if (!val.closure->fd->name)
		return obj.bits = vt_undef, obj;

	sym = (symNode *)(val.closure->table + val.closure->fd->name);
	sn = (stringNode *)(val.closure->table + sym->name);

	return newString(sn->str.val, sn->str.len);
}

value_t fcnFcnApply(value_t *args, value_t thisVal, environment_t *env) {
	value_t arguments = newArray(array_value, vec_cnt(args) - 1);
	array_t *aval = arguments.addr;
	uint32_t idx;

	for (idx = 1; idx < vec_cnt(args); idx++) {
		aval->valuePtr[idx - 1] = args[idx];
		incrRefCnt(args[idx]);
	}

	return fcnCall(thisVal, arguments, args[0], false, env);
}

value_t fcnFcnCall(value_t *args, value_t thisVal, environment_t *env) {
	value_t arguments = newArray(array_value, vec_cnt(args) - 1);
	array_t *aval = arguments.addr;
	uint32_t idx;

	for (idx = 1; idx < vec_cnt(args); idx++) {
		aval->valuePtr[idx - 1] = args[idx];
		incrRefCnt(args[idx]);
	}

	return fcnCall(thisVal, arguments, args[0], false, env);
}

value_t propFcnProto(value_t val, bool lVal) {
	value_t ref;

	if (!lVal)
		return val.closure->protoObj;

	ref.bits = vt_lval;
	ref.lval = &val.closure->protoObj;
	return ref;
}

value_t fcnFcnValueOf(value_t *args, value_t thisVal, environment_t *env) {

	if (vec_cnt(args))
		return *args;
	else
		return thisVal;
}

value_t fcnBoolValueOf(value_t *args, value_t thisVal, environment_t *env) {

	if (vec_cnt(args))
		return *args;
	else
		return thisVal;
}

value_t fcnBoolToString(value_t *args, value_t thisVal, environment_t *env) {
	value_t obj, val;

	if (vec_cnt(args))
		obj = *args;
	else
		obj = thisVal;

	val.bits = vt_string;

	if (obj.boolean)
		val.addr = &TrueStr;
	else
		val.addr = &FalseStr;

	return val;
}

value_t fcnNumToString(value_t *args, value_t thisVal, environment_t *env) {
	value_t val, s, obj;
	char buff[64];
	int len = 0;

	if (vec_cnt(args))
		obj = *args;
	else
		obj = thisVal;

	switch (obj.type) {
	  case vt_int:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%" PRIi64, obj.nval);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%" PRIi64, obj.nval);
#endif
		break;

	  case vt_dbl:
#ifndef _WIN32
		len = snprintf(buff, sizeof(buff), "%.16G", obj.dbl);
#else
		len = _snprintf_s(buff, sizeof(buff), _TRUNCATE, "%.16G", obj.dbl);
#endif
		if (!(obj.dbl - (uint64_t)obj.dbl))
			buff[len++] = '.', buff[len++] = '0', buff[len] = 0;

		break;

	  case vt_infinite:
		val.bits = vt_string;

		if (obj.negative)
			val.addr = &InfinityStr;
		else
			val.addr = &MInfinityStr;

		return val;

	  case vt_null:
		val.bits = vt_string;
		val.addr = &NullStr;
		return val;

	  case vt_nan:
		val.bits = vt_string;
		val.addr = &NaNStr;
		return val;

	  default:
		fprintf(stderr, "Error: NumberToString => invalid type: %s\n", strtype(obj));
		return s.status = ERROR_script_internal, s;
	}

	return newString(buff, len);
}

value_t fcnNumValueOf(value_t *args, value_t thisVal, environment_t *env) {

	if (vec_cnt(args))
		return *args;
	else
		return thisVal;
}

value_t fcnNumToPrecision(value_t *args, value_t thisVal, environment_t *env) {
	value_t digits;
	char buff[512];
	double dbl;
	int len;

	if (thisVal.type == vt_int)
		dbl = (double)thisVal.nval;

	if (thisVal.type == vt_dbl)
		dbl = thisVal.dbl;

	if (vec_cnt(args) > 0)
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

value_t fcnNumToFixed(value_t *args, value_t thisVal, environment_t *env) {
	value_t digits;
	char buff[512];
	double dbl;
	int len;

	if (thisVal.type == vt_int)
		dbl = (double)thisVal.nval;

	if (thisVal.type == vt_dbl)
		dbl = thisVal.dbl;

	if (vec_cnt(args) > 0)
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

value_t fcnNumToExponential(value_t *args, value_t thisVal, environment_t *env) {
	value_t digits;
	char buff[512];
	double dbl;
	int len;

	if (thisVal.type == vt_int)
		dbl = (double)thisVal.nval;

	if (thisVal.type == vt_dbl)
		dbl = thisVal.dbl;

	if (vec_cnt(args) > 0)
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

extern PropFcn builtinKeyFcns[];

extern PropFcn builtinDateFcns[];

extern PropVal builtinStrProp[];

extern PropVal builtinObjProp[];

extern PropVal builtinDocIdProp[];

extern PropVal builtinArrayProp[];

PropVal builtinBoolProp[] = {
	{ propBoolLength, "length" },
	{ NULL, NULL}
};

PropVal builtinNumProp[] = {
	{ NULL, NULL}
};

extern PropVal builtinDateProp[];

PropVal builtinFcnProp[] = {
	{ propFcnName, "name" },
	{ propFcnLength, "length" },
	{ propFcnProto, "prototype", true },
	{ propFcnDisplayName, "displayName" },
	{ NULL, NULL}
};

extern PropFcn builtinStrFcns[];

extern PropFcn builtinObjFcns[];

extern PropFcn builtinDocIdFcns[];

extern PropFcn builtinArrayFcns[];

PropFcn builtinNumFcns[] = {
	{ fcnNumValueOf, "valueOf" },
	{ fcnNumToString, "toString" },
	{ fcnNumToExponential, "toExponential" },
	{ fcnNumToFixed, "toFixed" },
	{ fcnNumToPrecision, "toPrecision" },
	{ NULL, NULL}
};

PropFcn builtinBoolFcns[] = {
	{ fcnBoolValueOf, "valueOf" },
	{ fcnBoolToString, "toString" },
	{ NULL, NULL}
};

PropFcn builtinFcnFcns[] = {
	{ fcnFcnValueOf, "valueOf" },
	{ fcnFcnApply, "apply" },
	{ fcnFcnCall, "call" },
	{ NULL, NULL}
};

extern PropVal builtinDbProp[];
extern PropVal builtinStoreProp[];
extern PropVal builtinIdxProp[];
extern PropVal builtinCursorProp[];
extern PropVal builtinIterProp[];
extern PropVal builtinTxnProp[];
extern PropVal builtinDocProp[];
extern PropVal builtinDocIdProp[];
extern PropVal builtinKeyProp[];
extern PropVal builtinCatalogProp[];

extern PropFcn builtinDbFcns[];
extern PropFcn builtinStoreFcns[];
extern PropFcn builtinIdxFcns[];
extern PropFcn builtinCursorFcns[];
extern PropFcn builtinIterFcns[];
extern PropFcn builtinTxnFcns[];
extern PropFcn builtinDocFcns[];
extern PropFcn builtinDocIdFcns[];
extern PropFcn builtinCatalogFcns[];

PropVal *builtinProp[] = {
	builtinStrProp,
	builtinObjProp,
	builtinArrayProp,
	builtinNumProp,
	builtinBoolProp,
	builtinDateProp,
	builtinFcnProp,
	builtinDbProp,
	builtinStoreProp,
	builtinIdxProp,
	builtinCursorProp,
	builtinIterProp,
	builtinTxnProp,
	builtinDocProp,
	builtinDocIdProp,
    builtinKeyProp,
	builtinCatalogProp,
	NULL
};

PropFcn *builtinFcn[] = {
	builtinStrFcns,
	builtinObjFcns,
	builtinArrayFcns,
	builtinNumFcns,
	builtinBoolFcns,
	builtinDateFcns,
	builtinFcnFcns,
	builtinDbFcns,
	builtinStoreFcns,
	builtinIdxFcns,
	builtinCursorFcns,
	builtinIterFcns,
	builtinTxnFcns,
	builtinDocFcns,
	builtinDocIdFcns,
    builtinKeyFcns,
	builtinCatalogFcns};

char *builtinNames[] = {
	"String.prototype.",
	"Object.prototype.",
	"Array.prototype.",
	"Number.prototype.",
	"Boolean.prototype.",
	"Date.prototype.",
	"Functions.prototype.",
	"Db.prototype.",
	"DocStore.prototype.",
	"Index.prototype.",
	"Cursor.prototype.",
	"Iterator.prototype.",
	"Txn.prototype.",
	"Doc.prototype.",
	"DocId.prototype.",
    "Key.prototype.", 
	"Catalog.prototype."};

value_t builtinVal[sizeof(builtinNames)/sizeof(char *)];

//  install built-in properties into system level object

value_t js_installProps(uint32_t args, environment_t *env) {
	PropVal *proptbl;
	PropFcn *fcntbl;
	value_t table, s;
	value_t fcn, obj;

	s.bits = vt_status;

	obj = eval_arg(&args, env);

	if (vt_closure != obj.type) {
		fprintf(stderr, "Error: installProps => expecting closure => %s\n", strtype(obj));
		return s.status = ERROR_script_internal, s;
	}

	table = eval_arg(&args, env);

	if (vt_int != table.type) {
		fprintf(stderr, "Error: installProps => expecting int => %s\n", strtype(table));
		return s.status = ERROR_script_internal, s;
	}

	if (table.nval < sizeof(builtinProp) / sizeof(*builtinProp)) {
	 if ((proptbl = builtinProp[table.nval])) {
	  value_t base;

	  while (proptbl->fcn) {
		if (proptbl->isBase)
			base = obj.closure->obj;
		else
			base = obj.closure->protoObj;

		proptbl->str = newString(proptbl->name, -1);

		fcn.bits = vt_propval;
		fcn.nval = (PropVal *)proptbl - builtinProp[table.nval];
		fcn.subType = (uint32_t)table.nval;

		replaceValue(lookup(base, proptbl->str, true, 0), fcn);
		proptbl++;
	  }
	 } else {
		fprintf(stderr, "Error: installProps => expecting property table idx => %d\n", (int)table.nval);
		return s.status = ERROR_script_internal, s;
	 }
	}
	
	if (table.nval < sizeof(builtinFcn) / sizeof(*builtinFcn)) {
	 if ((fcntbl = builtinFcn[table.nval])) {
	  value_t base;

	  while (fcntbl->fcn) {
		if (fcntbl->isBase)
			base = obj.closure->obj;
		else
			base = obj.closure->protoObj;

		fcntbl->str = newString(fcntbl->name, -1);

		fcn.bits = vt_propfcn;
		fcn.nval = (PropFcn *)fcntbl - builtinFcn[table.nval];
		fcn.subType = (uint32_t)table.nval;

		replaceValue(lookup(base, fcntbl->str, true, 0), fcn);
		fcntbl++;
	  }
	 } else {
		fprintf(stderr, "Error: installProps => expecting fcn table idx => %d\n", (int)table.nval);
		return s.status = ERROR_script_internal, s;
	 }
	}

	if (table.nval < sizeof(builtinNames) / sizeof(*builtinNames))
		builtinVal[table.nval] = newString(builtinNames[table.nval], -1);
	else {
		fprintf(stderr, "Error: installProps => expecting name table idx => %d\n", (int)table.nval);
		return s.status = ERROR_script_internal, s;
	}

	if (args) for(;;) {
		value_t v = eval_arg(&args, env);

		if (v.type == vt_endlist)
			break;

		if (v.type == vt_int) {
		  if (v.nval < vt_MAX) {
			builtinProto[v.nval] = obj.closure->protoObj;
			incrRefCnt(obj.closure->protoObj);
			builtinMap[v.nval] = (uint32_t)table.nval;
		  }
		}
	}

	s.status = OK;
	return s;
}

value_t getPropFcnName(value_t fcn) {
	value_t name, ans[1];

	if (fcn.type == vt_propfcn)
		name = builtinFcn[fcn.subType][fcn.nval].str;
	else
		name = builtinProp[fcn.subType][fcn.nval].str;

	*ans = builtinVal[fcn.subType];

	valueCat(ans, name, false);
	return *ans;
}

value_t callObjFcn(value_t obj, string_t *name, bool abandon, environment_t *env) {
	value_t result, original = obj;

	if (obj.type == vt_lval)
		obj = *obj.lval;

	if (obj.type == vt_document) 
		obj = getDocObject(obj);

	//	find the function in the object, or its prototype chain

	result = lookupAttribute(obj, name, original, false, true);

	if (abandon)
		abandonValueIfDiff(obj, result);

	return result;
}

value_t callFcnProp(value_t prop, value_t original, bool lVal) {
	value_t v;

	if (prop.subType == builtinMap[original.type])
		v = (builtinProp[prop.subType][prop.nval].fcn)(original, lVal);
	else
		v.bits = vt_undef;

	return v;
}

value_t callFcnFcn(value_t fcn, value_t *args, environment_t *env) {
	value_t thisVal = env->topFrame->nextThis;

	if (thisVal.type == vt_lval)
		thisVal = *thisVal.lval;

	return (builtinFcn[fcn.subType][fcn.nval].fcn)(args, thisVal, env);
}
