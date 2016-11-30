#include "js.h"

char *strstatus(Status s) {
	switch (s) {
	case OK:					return "OK";
	case ERROR_outofmemory:		return "out of memory";
	case ERROR_script_internal:	return "script internal error";
	case ERROR_script_unrecognized_function:	return "script unrecognized function";
	case ERROR_tcperror:		return "tcperror";
	case ERROR_bsonformat:		return "bsonformat";
	case ERROR_notobject_or_array:	return "not object or array";
	default:;
	}
	return NULL;
}

void errorText(Status s) {
char *status = strstatus(s);

	if (!s)
		printf("Status: OK\n"); 
	else if (status)
		printf("Error: %s\n", status);
	else
		printf("Unrecognized error: %d", s);
}

value_t makeError(Node *node, environment_t *env, char *msg) {
value_t v;

	fprintf (stderr, "js error line = %d, node id = %d type = %d: %s\n", (int)node->lineNo, (int)(node - env->table), (int)node->type, msg);

	v.bits = vt_control;
	v.aux = strlen(msg);
	v.string = msg;
	return v;
}
