var builtinProp = enum {
	builtinStr,
	builtinObj,
	builtinArray,
	builtinNum,
	builtinBool,
	builtinDate,
	builtinFcn,
	builtinDb,
	builtinStore,
	builtinIdx,
	builtinCursor,
	builtinIter,
	builtinTxn,
	builtinDoc,
	builtinDocId,
	builtinCatalog
};

var _values = enum {
	vt_undef = 0,
	vt_bool,
	vt_int,
	vt_dbl,
	vt_date,
	vt_infinite,
	vt_number,
	vt_string,
	vt_nan,
	vt_null,
	vt_file,
	vt_status,
	vt_control,
	vt_closure,
	vt_endlist,
	vt_document,
	vt_docId,
	vt_txnId,
	vt_lval,
	vt_centi,
	vt_array,
	vt_object,
	vt_binary,
	vt_function,
	vt_symbol,
	vt_uuid,
	vt_md5,
	vt_propfcn,
	vt_propval,
	vt_weakref,
	vt_catalog,
	vt_db,
	vt_store,
	vt_index,
	vt_cursor,
	vt_iter,
	vt_txn,
	vt_key,
	vt_builtin,
	vt_MAX
};

function Object() {
	var ans;

	if(this)
		ans = this;
	else
		ans = {};

	if (arguments.length > 0)
		ans.setValue(arguments[0]);

	return ans;
}

jsdb_installProps(Object, builtinProp.builtinObj, _values.vt_object);

Object.assign = function() {
	var target, names, values;

	if (arguments.length < 1)
		return {};

	target = arguments[0];

	if (typeof target == "object")
	  for (var i = 1; i < arguments.length; i++)
		for (var key in arguments[i])
		  target[key] = arguments[i][key];

	return target;
};
