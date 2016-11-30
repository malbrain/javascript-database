#include "js.h"

//	find node's root node

firstNode *findFirstNode(Node *table, uint32_t slot) {
	uint32_t start = 0;

	while(start < slot) {
		firstNode *fn = (firstNode *)(table + start);
		start += fn->moduleSize;

		if( start > slot ) 
			return fn;
	}

	return (firstNode *)table;
}

uint32_t newNode (parseData *pd, nodeType type, uint32_t size, bool zero) {
	uint32_t blks = (size + sizeof(Node) - 1)/sizeof(Node);
	uint32_t addr = pd->tableNext;
	Node *node;

	if( blks + pd->tableNext >= pd->tableSize ) {
		if( pd->tableSize )
			pd->tableSize *= 2;
		else
			pd->tableSize = 4090;

		if (pd->table)
			pd->table = js_realloc (pd->table, pd->tableSize * sizeof(Node), false);
		else
			pd->table = js_alloc (pd->tableSize * sizeof(Node), false);

		pd->tableSize = js_size(pd->table) / sizeof(Node);
	}

	node = pd->table + pd->tableNext;

	if (zero)
		memset (node, 0, size);

	node->bits = type;
	node->lineNo = pd->lineNo;

	pd->tableNext += blks;
	return  addr;
}

char convLit(char quoted) {
	switch(quoted) {
	case 0x0a:	return 0;
	case 0x0d:	return 0;
	case 'f':	return 0x0c;
	case 'r':	return 0x0d;
	case 'n':	return 0x0a;
	case 't':	return 0x09;
	case 'b':	return 0x08;
	}

	return quoted;
}

uint32_t newStrNode (parseData *pd, char *text, uint32_t size) {
	uint32_t len = 0, addr, off = 0, max;
	stringNode *sn;
	char c;

	for(max = 1; max < size; max++)
		switch(text[max]) {
		case '\\': continue;
		case 0x0a: pd->lineNo++; continue;
		case 0x0d: continue;
		default: len++; continue;
		}

	max--;	// discard trailing quote mark
	len--;	// discard trailing quote mark

	addr = newNode(pd, node_string, sizeof(stringNode) + len, false);
	sn = (stringNode *)(pd->table + addr);
	sn->hdr->aux = len;

	for(int idx = 1; idx < max && off < len; idx++)
		switch((c = text[idx])) {
		case '\\':
			if ((sn->string[off] = convLit(text[++idx])))
				off++;
			continue;

		case 0x0a: continue;
		case 0x0d: continue;
		default: sn->string[off++] = c;
		}

	return addr;
}
