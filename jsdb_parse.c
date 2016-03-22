#include "jsdb.h"

uint32_t newNode (parseData *pd, nodeType type, uint32_t size, bool zero) {
	uint32_t blks = (size + sizeof(Node) - 1)/sizeof(Node);
	uint32_t addr = pd->tablenext;
	Node *node;

	if( blks + pd->tablenext >= pd->tablesize ) {
		if( pd->tablesize )
			pd->tablesize *= 2;
		else
			pd->tablesize = 4096;

		pd->table = realloc (pd->table, pd->tablesize * sizeof(Node));
	}

	node = pd->table + pd->tablenext;

	if (zero)
		memset (node, 0, size);

	node->bits = type;
	node->lineno = pd->lineno;

	pd->tablenext += blks;
	return  addr;
}
