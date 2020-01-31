
typedef enum {
	OK,
	ERROR_outofmemory,
	ERROR_script_internal,
	ERROR_script_unrecognized_function,
	ERROR_tcperror,
	ERROR_bsonformat,
	ERROR_notobject_or_array,
	ERROR_mathdomain,
	ERROR_endoffile,
	ERROR_doesnot_exist,
	ERROR_script_parse,
	ERROR_json_parse,
	ERROR_not_document,
	ERROR_not_found,
	ERROR_toomany_local_docstores,
	ERROR_txn_being_committed,
	ERROR_txn_not_serializable,
	ERROR_no_visible_version,
	ERROR_write_conflict,
	ERROR_key_constraint_violation,
	ERROR_not_object,
	ERROR_not_key,
	ERROR_not_operator_int,
	ERROR_incorrect_handle_type,
} Status;

typedef void *JsStatus;

//	errors are values < 4096

#define jsError(val) ((uint64_t)val < 0x1000 ? (uint64_t)val : 0)
