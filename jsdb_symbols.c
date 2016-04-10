#include "jsdb.h"

static bool debug = false;

extern int builtin (stringNode *name);

// symbol table

uint32_t insertSymbol(char *name, uint32_t len, symtab_t *symtab, uint32_t level) {
	uint32_t sz = vec_count(symtab->entries);
	symbol_t sym;

	if (debug)
		printf("insertSymbol('%.*s', %llu)\n", len, name, (uint64_t)symtab);

	sym.symbolName = name;
	sym.nameLen = len;
	sym.frameidx = sz;
	sym.level = level;

	vec_push(symtab->entries, sym); // copy!!
	return sz;
}

symbol_t *lookupSymbol(char *name, uint32_t len, symtab_t *symtab) {
	int i;

	if (debug)
		printf("lookupSymbol('%.*s')\n", len, name);

	do {
		int scount = vec_count(symtab->entries);

		for (i = 0; i < scount; i++) {
			symbol_t *sp = &symtab->entries[i];
			if (sp->nameLen == len)
				if (!memcmp(sp->symbolName, name, len))
					return sp;
		}
	} while ( (symtab = symtab->parent) );

	return NULL;
}

void hoistSymbols(uint32_t slot, Node *table, symtab_t *symtab, uint32_t level, fcnDeclNode *parent)
{
	switch (table[slot].type) {
	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(table + slot);
			hoistSymbols(ln->elem, table, symtab, level, parent);
			slot -= sizeof(listNode) / sizeof(Node);
		} while ( ln->hdr->type == node_list );

		break;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		hoistSymbols(iftn->thenstmt, table, symtab, level, parent);
		hoistSymbols(iftn->elsestmt, table, symtab, level, parent);
		break;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(table + slot);
		hoistSymbols(wn->stmt, table, symtab, level, parent);
		break;
	}
	case node_for: {
		forNode *fn = (forNode*)(table + slot);
		hoistSymbols(fn->init, table, symtab, level, parent);
		hoistSymbols(fn->stmt, table, symtab, level, parent);
		break;
	}
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		hoistSymbols(bn->right, table, symtab, level, parent);
		hoistSymbols(bn->left, table, symtab, level, parent);
		break;
	}
	case node_var:
	case node_ref: {
		symNode *sn = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sn->name);

		if (sn->hdr->flag & flag_decl)
			insertSymbol(name->string, name->hdr->aux, symtab, level);

		break;
	}
	case node_fcnexpr:
	case node_fcndef: {
		fcnDeclNode *fn = (fcnDeclNode *)(table + slot);
		compileSymbols(fn, table, symtab, level + 1);
		break;
	}
	default:
		if (debug)
			printf("unprocessed");
	}

	if (debug)
		printf ("\n");
}

void assignSlots(uint32_t slot, Node *table, symtab_t *symtab, uint32_t level)
{
	if (!slot) return;

	switch (table[slot].type) {
	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(table + slot);
			assignSlots(ln->elem, table, symtab, level);
			slot -= sizeof(listNode) / sizeof(Node);
		} while (ln->hdr->type == node_list);

		break;
	}

	case node_neg:
	case node_incr:
	case node_typeof:
	case node_return:  {
		exprNode *en = (exprNode *)(table + slot);
		assignSlots(en->expr, table, symtab, level);
		break;
	}

	case node_enum:
	case node_math:
	case node_access:
	case node_lookup:
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		assignSlots(bn->left, table, symtab, level);
		assignSlots(bn->right, table, symtab, level);
		break;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		assignSlots(iftn->condexpr, table, symtab, level);
		assignSlots(iftn->thenstmt, table, symtab, level);
		assignSlots(iftn->elsestmt, table, symtab, level);
		break;
	}
	case node_elem: {
		binaryNode *bn = (binaryNode *)(table + slot);
		assignSlots(bn->right, table, symtab, level);
		break;
	}
	case node_array: {
		arrayNode *an = (arrayNode *)(table + slot);
		assignSlots(an->exprlist, table, symtab, level);
		break;
	}
	case node_obj: {
		objNode *on = (objNode *)(table + slot);
		assignSlots(on->elemlist, table, symtab, level);
		break;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(table + slot);
		assignSlots(wn->cond, table, symtab, level);
		assignSlots(wn->stmt, table, symtab, level);
		break;
	}
	case node_for: {
		forNode *fn = (forNode*)(table + slot);
		assignSlots(fn->init, table, symtab, level);
		assignSlots(fn->cond, table, symtab, level);
		assignSlots(fn->incr, table, symtab, level);
		assignSlots(fn->stmt, table, symtab, level);
		break;
	}
	case node_var:
	case node_ref: {
		symNode *sn = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sn->name);
		symbol_t *sym = lookupSymbol(name->string, name->hdr->aux, symtab);

		if (!sym)
			printf(" Symbol not found: %.*s line = %d node = %d\n", name->hdr->aux, name->string, sn->hdr->lineno, slot), exit(1);

		sn->frameidx = sym->frameidx;
		sn->level = sym->level;
		break;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(table + slot);
		assignSlots(fc->args, table, symtab, level);

		symNode *sn = (symNode *)(table + fc->name);

		if (sn->hdr->type != node_var) {
			assignSlots(fc->name, table, symtab, level);
			break;
		}

		stringNode *name = (stringNode *)(table + sn->name);
		symbol_t *sym = lookupSymbol(name->string, name->hdr->aux, symtab);

		if (sym) {
			sn->frameidx = sym->frameidx;
			sn->level = sym->level;
			break;
		}

		int idx = builtin(name);

		if (idx < 0)
			printf(" Function not found: %.*s line = %d node = %d\n", name->hdr->aux, name->string, sn->hdr->lineno, slot), exit(1);

		fc->hdr->type = node_builtin;
		fc->hdr->aux = idx;
		break;
	}
	default:
		if (debug)
			printf("node type %d skipped\n", table[slot].type);
		break;
	}
}
void compileSymbols(fcnDeclNode *pn, Node *table, symtab_t *parent, uint32_t level) {
	symtab_t symtab[1];
	uint32_t slot;
	listNode *ln;

	memset (symtab, 0, sizeof(symtab_t));
	symtab->parent = parent;

	// install parameters into symbol table

	hoistSymbols(pn->params, table, symtab, level, pn);
	pn->nparams = vec_count(symtab->entries);

	// install name into fcn expression

	if (pn->hdr->type == node_fcnexpr)
		if (pn->name) {
			symNode *sn = (symNode *)(table + pn->name);
			stringNode *name = (stringNode *)(table + sn->name);

			// install the function name in the table

			sn->frameidx = insertSymbol(name->string, name->hdr->aux, symtab, level);
			sn->level = level;
		}

	// hoist top level declarations

	if (( slot = pn->body)) do {
		ln = (listNode *)(table + slot);
		Node *node = (Node *)(table + ln->elem);

		// install the function name in the table

		if (node->type == node_fcndef ) {
			fcnDeclNode *fn = (fcnDeclNode *)node;
			symNode *sn = (symNode *)(table + fn->name);
			stringNode *name = (stringNode *)(table + sn->name);

			// install the function name in the table

			sn->frameidx = insertSymbol(name->string, name->hdr->aux, symtab, level);
			sn->level = level;

			// install this fcn in parent's list

			fn->next = pn->fcn;
			pn->fcn = ln->elem;
		}

		hoistSymbols(ln->elem, table, symtab, level, pn);
		slot -= sizeof(listNode) / sizeof(Node);
	} while (ln->hdr->type == node_list);

	pn->nsymbols = vec_count(symtab->entries);

	//  assign slots to body variables

	assignSlots(pn->params, table, symtab, level);
	assignSlots(pn->body, table, symtab, level);
	vec_free(symtab->entries);
}
