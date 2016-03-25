#include "jsdb.h"

static bool debug = false;

extern int builtin (stringNode *name);

// symbol table

uint32_t insertSymbol(stringNode *name, symtab_t *symtab, uint32_t level) {
	uint32_t sz = vec_count(symtab->entries);
	symbol_t sym;

	if (debug)
		printf("insertSymbol('%.*s', %llu)\n", name->hdr->aux, name->string, (uint64_t)symtab);
	sym.frameidx = sz;
	sym.level = level;
	sym.name = name;

	vec_push(symtab->entries, sym); // copy!!
	return sz;
}

symbol_t *lookupSymbol(stringNode *name, symtab_t *symtab) {
	int i;

	if (debug)
		printf("lookupSymbol('%.*s')\n", name->hdr->aux, name->string);

	do {
		int scount = vec_count(symtab->entries);

		for (i = 0; i < scount; i++) {
			symbol_t *sp = &symtab->entries[i];
			if (sp->name->hdr->aux == name->hdr->aux)
				if (!memcmp(sp->name->string, name->string, name->hdr->aux))
					return sp;
		}
	} while ( (symtab = symtab->parent) );

	return NULL;
}

void hoist(uint32_t slot, Node *table, symtab_t *symtab, uint32_t level, fcnDeclNode *parent)
{
	switch (table[slot].type) {
	case node_list: {
		do {
			listNode *ln = (listNode *)(table + slot);

			if (!ln->hdr->type)
				return;

			hoist(ln->elem, table, symtab, level, parent);
			slot -= sizeof(listNode) / sizeof(Node);
		} while ( true );
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		hoist(iftn->thenstmt, table, symtab, level, parent);
		hoist(iftn->elsestmt, table, symtab, level, parent);
		break;
	}
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		hoist(bn->right, table, symtab, level, parent);
		hoist(bn->left, table, symtab, level, parent);
		break;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(table + slot);
		hoist(wn->stmt, table, symtab, level, parent);
		break;
	}
	case node_for: {
		forNode *fn = (forNode*)(table + slot);
		hoist(fn->init, table, symtab, level, parent);
		hoist(fn->stmt, table, symtab, level, parent);
		break;
	}
	case node_var:
	case node_ref: {
		symNode *sn = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sn->name);

		if (sn->hdr->flag & flag_decl)
			insertSymbol(name, symtab, level);

		break;
	}
	case node_fcnexpr:
	case node_fcndef: {
		fcnDeclNode *fn = (fcnDeclNode *)(table + slot);
		compile(fn, table, symtab, level + 1);
		break;
	}
	default:
		if (debug)
			printf("unprocessed");
	}

	if (debug)
		printf ("\n");
}

void compile(fcnDeclNode *pn, Node *table, symtab_t *parent, uint32_t level)
{
	symtab_t symtab[1];
	uint32_t slot;

	memset (symtab, 0, sizeof(symtab_t));
	symtab->parent = parent;

	// install parameters into symbol table

	hoist(pn->params, table, symtab, level, pn);
	pn->nparams = vec_count(symtab->entries);

	// install name into fcn expression

	if (pn->hdr->type == node_fcnexpr)
		if (pn->name) {
			symNode *sn = (symNode *)(table + pn->name);
			stringNode *name = (stringNode *)(table + sn->name);

			// install the function name in the table

			sn->frameidx = insertSymbol(name, symtab, level);
			sn->level = level;
		}

	// find top level fcn definitions

	if (( slot = pn->body)) do {
		listNode *ln = (listNode *)(table + slot);
		fcnDeclNode *fn = (fcnDeclNode *)(table + ln->elem);

		if (!ln->hdr->type) // end of list?
			break;
	
		slot -= sizeof(listNode) / sizeof(Node);

		if (fn->hdr->type == node_fcndef || fn->hdr->type == node_fcnexpr) {
			symNode *sn = (symNode *)(table + fn->name);
			stringNode *name = (stringNode *)(table + sn->name);

			// install the function name in the table

			sn->frameidx = insertSymbol(name, symtab, level);
			sn->level = level;

			// install this fcn in parent's list

			fn->next = pn->fcn;
			pn->fcn = ln->elem;
		}
	} while ( true );

	hoist(pn->body, table, symtab, level, pn);
	pn->nsymbols = vec_count(symtab->entries);

	//  assign slots to body variables

	assign(pn->params, table, symtab, level);
	assign(pn->body, table, symtab, level);
	vec_free(symtab->entries);
}

void assign(uint32_t slot, Node *table, symtab_t *symtab, uint32_t level)
{
	if (!slot) return;

	switch (table[slot].type) {
	case node_list: {
		do {
			listNode *ln = (listNode *)(table + slot);
	
			if (!ln->hdr->type)
				return;
	
			assign(ln->elem, table, symtab, level);
			slot -= sizeof(listNode) / sizeof(Node);
		} while ( true );
	}

	case node_neg:
	case node_return:  {
		exprNode *en = (exprNode *)(table + slot);
		assign(en->expr, table, symtab, level);
		break;
	}

	case node_math:
	case node_access:
	case node_lookup:
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		assign(bn->left, table, symtab, level);
		assign(bn->right, table, symtab, level);
		break;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		assign(iftn->condexpr, table, symtab, level);
		assign(iftn->thenstmt, table, symtab, level);
		assign(iftn->elsestmt, table, symtab, level);
		break;
	}
	case node_elem: {
		binaryNode *bn = (binaryNode *)(table + slot);
		assign(bn->right, table, symtab, level);
		break;
	}
	case node_array: {
		arrayNode *an = (arrayNode *)(table + slot);
		assign(an->exprlist, table, symtab, level);
		break;
	}
	case node_obj: {
		objNode *on = (objNode *)(table + slot);
		assign(on->elemlist, table, symtab, level);
		break;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(table + slot);
		assign(wn->cond, table, symtab, level);
		assign(wn->stmt, table, symtab, level);
		break;
	}
	case node_for: {
		forNode *fn = (forNode*)(table + slot);
		assign(fn->init, table, symtab, level);
		assign(fn->cond, table, symtab, level);
		assign(fn->incr, table, symtab, level);
		assign(fn->stmt, table, symtab, level);
		break;
	}
	case node_var:
	case node_ref: {
		symNode *sn = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sn->name);
		symbol_t *sym = lookupSymbol(name, symtab);

		if (!sym)
			printf(" Symbol not found: %.*s line = %d node = %d\n", name->hdr->aux, name->string, sn->hdr->lineno, slot), exit(1);

		sn->frameidx = sym->frameidx;
		sn->level = sym->level;
		break;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(table + slot);
		assign(fc->args, table, symtab, level);

		symNode *sn = (symNode *)(table + fc->name);

		if (sn->hdr->type != node_var) {
			assign(fc->name, table, symtab, level);
			break;
		}

		stringNode *name = (stringNode *)(table + sn->name);
		symbol_t *sym = lookupSymbol(name, symtab);

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
