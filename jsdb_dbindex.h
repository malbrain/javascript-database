
typedef union {
	struct {
		uint8_t docId[sizeof(DocId)];
		uint8_t docVer[sizeof(uint64_t)];
	};
	uint8_t bytes[1];
} KeySuffix;

enum KeyType {
	key_end = 0,
	key_bool,
	key_int,
	key_dbl,
	key_str,
	key_objId,
	key_mask = 7,
	key_first = 8,
	key_reverse = 16
};

typedef struct {
	uint32_t type:8;
	uint32_t len:24;
	uint8_t name[0];
} IndexKey;

typedef enum {
	index_unique = 1,
	index_sparse = 2,
} IndexOptions;

typedef struct {
	DbAddr keys;			// keys document
	DbAddr partial;			// partial document
	IndexOptions opts;		// database options
} DbIndex;

#define indexAddr(index)((DbIndex *)(index->arena + 1))

bool indexKey (DbMap *index, uint8_t *keyBuff, uint32_t keyLen, uint8_t *suffix);
int keyFld(value_t field, IndexKey *key, uint8_t *buff, uint32_t max);
