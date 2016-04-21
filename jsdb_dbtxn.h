enum TxnStep {
	Unused = 0,
	DocStore,
	DocUpdate,
	DocDelete,
	KeyInsert,
	KeyDelete
};

typedef struct {
	uint64_t timestamp;
	uint32_t keyLen;
	uint32_t mapIdx;
	uint8_t key[0];
} TxnStep;

bool addTxnStep(array_t *docStore, uint32_t idx, DbAddr *head, uint8_t *keyBuff, int keyLen, enum TxnStep step, uint32_t set);
uint64_t startTxn(DbMap *map, DocId docId, enum TxnStep step);
Status commitTxn(DbMap *map, DbAddr *slot, DbAddr docId);
Status rollbackTxn(array_t *docStore, DbDoc *doc);
