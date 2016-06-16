#include "jsdb.h"
#include "jsdb_db.h"

//  updateDoc (docStore, docArray, &docIdArray, &docCount)

value_t jsdb_updateDoc(uint32_t args, environment_t *env) {
	value_t s;

	s.bits = vt_status;
	return s.status = OK, s;
}

