// btree control structure

#define BT_blk (1 << BT_bits)
#define BT_bits 14
#define BT_lvl 6			// maximum level of index blocks

#define BT_stopper	16		// length of stopper key

typedef enum {
	BTREE_ok = 0,
	BTREE_struct,
	BTREE_intr,
	BTREE_eof,
	BTREE_write,
	BTREE_hdr,				// header crc error
	BTREE_blk0,
	BTREE_log,				// short log read
	BTREE_ext,
	BTREE_zero,				// can't write zero length record
	BTREE_crc,				// index body crc error
	BTREE_map,				// unable to map block
	BTREE_stop,				// stopper key deleted
	BTREE_lvl,				// too many levels > BT_lvl
	BTREE_none,				// no key found
	BTREE_chg,				// update with different keys
	BTREE_cache,			// cache consistency error
	BTREE_max				// maximum offset testing
} BT_resp;

typedef struct {
	DbAddr rowid;			// last database row-id
	uchar resv[28];			// reserved for expansion
} BT_zero;

typedef struct {
	BT_hdr hdr[1];			// standard msg header
	uchar cnt[2];			// count of keys for record
	uchar off[2];			// next available key offset
	uchar amt[2];			// avail bytes in key area
	DbAddr rowid;			// row-id of record
	DbAddr chain;			// record chain
} BT_rec;

typedef struct {
	uint flush:1;			// record file flush
	uint lock:1;			// record offset locked

	BT_resp err;			// last error code

	uchar *map;				// address of page mapping
	off_t page;				// offset of page mapping
	uint xtent;				// extent of page mapping

	off_t off;				// offset of record
	off_t next;				// next offset for scan
	BT_rec *rec;			// mapped data record
	int len;				// length of record
	int log;				// fd of record file

	off_t chain;			// current chain offset
	off_t prev;				// previous chain offset

	int idx;				// DataBase index file
	int lvl;				// DataBase B-Tree depth
	int mode;				// DataBase open mode
	int size;				// DataBase idx block

	BT_rec *empty;			// DataBase empty block for splits
	BT_zero *zero;			// DataBase block zero extra data
	Datum name[1];			// DataBase name

	int cnt[BT_lvl];		// B-tree level key count
	int slot[BT_lvl];		// B-tree level slot index
	off_t addr[BT_lvl];		// B-tree index blk offset
	uchar *key[BT_lvl];		// B-tree key found
	BT_rec *array[BT_lvl];	// B-tree level memory address

	ushort base[BT_cache];	// B-tree cache base block
	void *virt[BT_cache];	// B-tree cache virtual addr
	ushort lru[BT_cache];	// B-tree cache next lru
	uint cache;				// B-tree cache starter cnt
} *Btree;

typedef struct BtPage_ {
	uint cnt;					// count of keys in page
	uint act;					// count of active keys
	uint min;					// next key/value offset
	uint fence;					// page fence key offset
	uint garbage;				// page garbage in bytes
	unsigned char lvl;			// level of page, zero = leaf
	unsigned char free;			// page is on the free chain
	unsigned char kill;			// page is being deleted
	unsigned char nopromote;	// page is being constructed
	DbAddr right, left;			// page numbers to right and left
} *BtPage;

BT_resp bt_retrieve (Btree, uchar *, uint size);
BT_resp bt_replace (Btree, BT_rec *, uint size, int flag);
BT_resp bt_update (Btree bt, BT_rec *rec, uint len);
BT_resp bt_insert (Btree, uchar *key, off_t off);
BT_resp bt_chain (Btree, uchar *key);
BT_resp bt_delete (Btree);
BT_resp bt_next (Btree bt);

BT_resp bt_find (Btree, uchar *key);

BT_resp bt_inskey (Btree, int lvl, uchar *key);
BT_resp bt_delkey (Btree, int lvl);

BT_resp bt_close (Btree);
BT_resp bt_open (Btree);

void bt_unlock (Btree);
void bt_lock (Btree);
