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
	uint8_t key[0];
} TxnStep;

bool addTxnStep(DbMap *map, DbAddr *head, uint8_t *keyBuff, int keyLen, enum TxnStep step, uint32_t set);
uint64_t startTxn(DbMap *map, DocId docId, enum TxnStep step);
Status rollbackTxn(DbMap *map, DbDoc *doc);
Status commitTxn(DbMap *map, DbDoc *doc);
