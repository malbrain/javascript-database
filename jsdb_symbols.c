#include "jsdb.h"

static bool debug = false;

extern int builtin (stringNode *name);

// symbol table

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

uint32_t insertSymbol(char *name, uint32_t len, symtab_t *symtab, uint32_t level) {
	uint32_t sz = vec_count(symtab->entries);
	symbol_t sym, *psym;

	if (debug)
		printf("insertSymbol('%.*s', %llu)\n", len, name, (uint64_t)symtab);

	if ((psym = lookupSymbol(name, len, symtab)))
		if (psym->level == level)
			return psym->frameidx;

	sym.symbolName = name;
	sym.nameLen = len;
	sym.frameidx = sz;
	sym.level = level;

	vec_push(symtab->entries, sym); // copy!!
	return sz;
}

typedef struct {
	fcnDeclNode *parent;
	fcnDeclNode **fcns;
	symtab_t *symtab;
	uint32_t level;
	Node *table;
} hoistParams;

void hoistSymbols(uint32_t slot, hoistParams *hp) {

  while (slot)
	switch (hp->table[slot].type) {
	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(hp->table + slot);
			hoistSymbols(ln->elem, hp);
			slot -= sizeof(listNode) / sizeof(Node);
		} while ( ln->hdr->type == node_list );

		return;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(hp->table + slot);
		slot = fc->name;
		continue;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(hp->table + slot);
		hoistSymbols(iftn->thenstmt, hp);
		slot = iftn->elsestmt;
		continue;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(hp->table + slot);
		slot = wn->stmt;
		continue;
	}
	case node_forin: {
		forInNode *fn = (forInNode*)(hp->table + slot);
		hoistSymbols(fn->var, hp);
		slot = fn->stmt;
		continue;
	}
	case node_for: {
		forNode *fn = (forNode*)(hp->table + slot);
		hoistSymbols(fn->init, hp);
		slot = fn->stmt;
		continue;
	}
	case node_assign: {
		binaryNode *bn = (binaryNode *)(hp->table + slot);
		hoistSymbols(bn->left, hp);
		slot = bn->right;
		continue;
	}
	case node_var:
	case node_ref: {
		symNode *sym = (symNode *)(hp->table + slot);
		stringNode *name = (stringNode *)(hp->table + sym->name);

		if (sym->hdr->flag & flag_decl)
			insertSymbol(name->string, name->hdr->aux, hp->symtab, hp->level);

		return;
	}
	case node_fcndef: {
		fcnDeclNode *fn = (fcnDeclNode *)(hp->table + slot);
		symNode *sym = (symNode *)(hp->table + fn->name);
		stringNode *name = (stringNode *)(hp->table + sym->name);

		// install the function name in symbol table

		sym->frameidx = insertSymbol(name->string, name->hdr->aux, hp->symtab, hp->level);
		sym->level = hp->level;

		// install this fcn in parent's list

		fn->next = hp->parent->fcn;
		hp->parent->fcn = slot;
	}
	case node_fcnexpr: {
		vec_push (hp->fcns, (fcnDeclNode *)(hp->table + slot));
		return;
	}
	default:
		if (debug)
			printf("node %d unprocessed: %d\n", slot, hp->table[slot].type);

		return;
	}
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
	case node_enum:
	case node_incr:
	case node_typeof:
	case node_return:  {
		exprNode *en = (exprNode *)(table + slot);
		assignSlots(en->expr, table, symtab, level);
		break;
	}

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
	case node_forin: {
		forInNode *fn = (forInNode*)(table + slot);
		assignSlots(fn->var, table, symtab, level);
		assignSlots(fn->expr, table, symtab, level);
		assignSlots(fn->stmt, table, symtab, level);
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
		symNode *sym = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sym->name);
		symbol_t *symbol = lookupSymbol(name->string, name->hdr->aux, symtab);

		if (!symbol)
			printf(" Symbol not found: %.*s line = %d node = %d\n", name->hdr->aux, name->string, sym->hdr->lineno, slot), exit(1);

		sym->frameidx = symbol->frameidx;
		sym->level = symbol->level;
		break;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(table + slot);
		assignSlots(fc->args, table, symtab, level);

		symNode *sym = (symNode *)(table + fc->name);

		if (sym->hdr->type != node_var) {
			assignSlots(fc->name, table, symtab, level);
			break;
		}

		stringNode *name = (stringNode *)(table + sym->name);
		symbol_t *symbol = lookupSymbol(name->string, name->hdr->aux, symtab);

		if (symbol) {
			sym->frameidx = symbol->frameidx;
			sym->level = symbol->level;
			break;
		}

		int idx = builtin(name);

		if (idx < 0)
			printf(" Function not found: %.*s line = %d node = %d\n", name->hdr->aux, name->string, sym->hdr->lineno, slot), exit(1);

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
	hoistParams hp[1];
	uint32_t slot;
	listNode *ln;

	hp->symtab = symtab;
	hp->level = level;
	hp->table = table;
	hp->parent = pn;
	hp->fcns = NULL;

	memset (symtab, 0, sizeof(symtab_t));
	symtab->parent = parent;

	// install function parameter symbols

	hoistSymbols(pn->params, hp);
	pn->nparams = vec_count(symtab->entries);

	// install fcn name from fcn expression

	if (pn->hdr->type == node_fcnexpr)
	  if (pn->name && table[pn->name].type == node_var) {
		symNode *sym = (symNode *)(table + pn->name);
		stringNode *name = (stringNode *)(table + sym->name);

		// install the function name in the table

		sym->frameidx = insertSymbol(name->string, name->hdr->aux, symtab, level);
		sym->level = level;
	  }

	// hoist function body declarations

	hoistSymbols(pn->body, hp);

	// compile function definitions

	for (int idx = 0; idx < vec_count(hp->fcns); idx++)
		compileSymbols(hp->fcns[idx], table, symtab, level + 1);

	pn->nsymbols = vec_count(symtab->entries);

	//  assign slots to body variables

	assignSlots(pn->params, table, symtab, level);
	assignSlots(pn->body, table, symtab, level);
	vec_free(symtab->entries);
	vec_free(hp->fcns);
}
