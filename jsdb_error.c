#include "jsdb.h"

void errorText(Status s) {
	switch (s) {
	case OK:
		printf("Status: OK\n"); 
		break;
	case ERROR_outofmemory:
		printf("Error: out of memory\n");
		break;
	case ERROR_handleclosed:
		printf("Error: handle closed\n");
		break;
	case ERROR_badhandle:
		printf("Error: bad handle\n");
		break;
	case ERROR_badrecid:
		printf("Error: bad recid\n");
		break;
	case ERROR_endoffile:
		printf("Error: end of file\n");
		break;
	case ERROR_notbasever:
		printf("Error: not base version\n");
		break;
	case ERROR_recorddeleted:
		printf("Error: record deleted\n");
		break;
	case ERROR_recordnotvisible:
		printf("Error: record not visible\n");
		break;
	case ERROR_notcurrentversion:
		printf("Error: not current version\n");
		break;
	case ERROR_cursornotpositioned:
		printf("Error: cursor not positioned\n");
		break;
	case ERROR_invaliddeleterecord:
		printf("Error: invalid delete record\n");
		break;
	case ERROR_cursorbasekeyerror:
		printf("Error: cursor basekey error\n");
		break;
	case ERROR_writeconflict:
		printf("Error: write conflict error\n");
		break;
	case ERROR_duplicatekey:
		printf("Error: duplicate key\n");
		break;
	case ERROR_keynotfound:
		printf("Error: key not found\n");
		break;
	case ERROR_badtxnstep:
		printf("Error: bad txn step\n");
		break;
	case ERROR_arena_already_closed:
		printf("Error: arena already closed\n");
		break;
	case ERROR_script_internal:
		printf("Error: script internal error\n");
		break;
	case ERROR_script_unrecognized_function:
		printf("Error: script unrecognized function\n");
		break;
	default: printf("Unrecognized error: %d", s);
	}
}

value_t makeError(Node *node, environment_t *env, char *msg) {
	value_t v;

	fprintf (stderr, "jsdb error line = %d, node id = %d type = %d: %s\n", node->lineno, node - env->table, node->type, msg);

	v.bits = vt_control;
	v.aux = strlen(msg);
	v.str = msg;
	return v;
}
