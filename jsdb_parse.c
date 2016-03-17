#include "jsdb.h"

uint32_t newNode (parseData *pd, nodeType type, uint32_t size) {
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
	pd->tablenext += blks;
	node->bits = type;
	node->lineno = yyget_lineno(pd->scaninfo);
	return  addr;
}

void printNode(uint32_t slot, Node *table) {
	Node *a = table + slot;

	switch(a->type) {
	case node_int: {
		numNode *n = (numNode *)a;
		printf ("intNode: %d", n->intval);
		break;
	}
	case node_dbl: {
		numNode *n = (numNode *)a;
		printf ("dblNode: %G", n->dblval);
		break;
	}
	case node_bool: {
		numNode *n = (numNode *)a;
		printf ("boolNode: %d", n->boolval);
		break;
	}
	case node_string: {
		stringNode *sn = (stringNode *)a;
		printf ("stringNode: \"%.*s\"", sn->hdr->aux, sn->string);
		break;
	}
	case node_var: {
		symNode *sym = (symNode *)a;
		stringNode *name = (stringNode *)(table + sym->name);
		printf ("symNode: \"%.*s\"", name->hdr->aux, name->string);
		break;
	}
	case node_ref: {
		symNode *sym = (symNode *)a;
		stringNode *name = (stringNode *)(table + sym->name);
		printf ("refNode: \"%.*s\"", name->hdr->aux, name->string);
		break;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)a;
		symNode *fname = (symNode *)(table + fc->name);
		stringNode *name = (stringNode *)(table + fname->name);
		printf ("fcnCall: \"%.*s\"", name->hdr->aux, name->string);
		break;
	}
	default:;
	}
}

