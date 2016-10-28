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
