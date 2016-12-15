#include "js.h"

static bool debug = false;

extern int builtin (stringNode *name);
extern symtab_t globalSymbols[1];

// symbol table

symbol_t *lookupSymbol(char *name, uint32_t len, symtab_t *symbols) {
	value_t *symbol, symName;

	symName.bits = vt_string;
	symName.str = name;
	symName.aux = len;

	if (debug)
		printf("lookupSymbol('%.*s')\n", len, name);

	while (symbols) {
	  if ((symbol = lookup(symbols->entries, symName, false, true)))
		return symbol->sym;
	  else
	  	symbols = symbols->parent;
	}

	return NULL;
}

uint32_t insertSymbol(char *name, uint32_t len, symtab_t *symbols) {
	value_t *symbol, symName;

	symName.bits = vt_string;
	symName.str = name;
	symName.aux = len;

	if (!symbols)
		symbols = globalSymbols;

	if (debug)
		printf("insertSymbol('%.*s')\n", len, name);

	if ((symbol = lookup(symbols->entries, symName, true, false))) {
		symbol->sym->frameIdx = ++symbols->frameIdx;
		symbol->sym->depth = symbols->depth;
		return symbol->sym->frameIdx;
	}

	return 0;
}

void hoistSymbols(uint32_t slot, Node *table, symtab_t *symbols) {

  while (slot)
	switch (table[slot].type) {
	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(table + slot);
			hoistSymbols(ln->elem, table, symbols);
			slot -= sizeof(listNode) / sizeof(Node);
		} while ( ln->hdr->type == node_list );

		return;
	}
	case node_elem: {
		binaryNode *bn = (binaryNode *)(table + slot);
		slot = bn->right;
		continue;
	}
	case node_obj: {
		objNode *on = (objNode *)(table + slot);
		slot = on->elemlist;
		continue;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(table + slot);
		hoistSymbols(fc->name, table, symbols);
		
		slot = fc->args;
		continue;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		hoistSymbols(iftn->thenstmt, table, symbols);
		slot = iftn->elsestmt;
		continue;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(table + slot);
		slot = wn->stmt;
		continue;
	}
	case node_forin: {
		forInNode *forn = (forInNode*)(table + slot);
		hoistSymbols(forn->var, table, symbols);
		slot = forn->stmt;
		continue;
	}
	case node_for: {
		forNode *forn = (forNode*)(table + slot);
		hoistSymbols(forn->init, table, symbols);
		slot = forn->stmt;
		continue;
	}
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		hoistSymbols(bn->left, table, symbols);
		slot = bn->right;
		continue;
	}
	case node_var:
	case node_ref: {
		symNode *sym = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sym->name);

		if (sym->hdr->flag & flag_decl)
			insertSymbol(name->string, name->hdr->aux, symbols);

		return;
	}
	case node_fcndef: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);
		symNode *sym = (symNode *)(table + fd->name);
		stringNode *name = (stringNode *)(table + sym->name);

		// install the function name in symbol table

		sym->frameIdx = insertSymbol(name->string, name->hdr->aux, symbols);
		sym->level = 0;

		// add to list of child fcns

		fd->next = symbols->childFcns;
		symbols->childFcns = slot;
	}
	case node_fcnexpr: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);
		fd->symbols->depth = symbols->depth + 1;
		fd->symbols->parent = symbols;

		hoistSymbols(fd->params, table, fd->symbols);
		fd->nparams = fd->symbols->frameIdx;

		// install fcn name from fcn expression

		if (fd->hdr->type == node_fcnexpr)
		  if (fd->name && table[fd->name].type == node_var) {
 	 		symNode *sym = (symNode *)(table + fd->name);
			stringNode *name = (stringNode *)(table + sym->name);

			// install the function name in the table

			sym->frameIdx = insertSymbol(name->string, name->hdr->aux, fd->symbols);
			sym->level = 0;
		  }

		// hoist function body declarations

		hoistSymbols(fd->body, table, fd->symbols);
		fd->nsymbols = fd->symbols->frameIdx;
		return;
	}
	case node_return: {
		exprNode *en = (exprNode *)(table + slot);
		slot = en->expr;
		continue;
	}
	default:
		if (debug)
			printf("node %d unprocessed: %d\n", slot, (int)table[slot].type);

		return;
	}
}

void assignSlots(uint32_t slot, Node *table, symtab_t *symbols)
{
  while (slot)
	switch (table[slot].type) {
	case node_fcndef:
	case node_fcnexpr: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);
		fd->symbols->depth = symbols->depth + 1;
		assignSlots(fd->body, table, fd->symbols);
		return;
	}

	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(table + slot);
			assignSlots(ln->elem, table, symbols);
			slot -= sizeof(listNode) / sizeof(Node);
		} while (ln->hdr->type == node_list);

		return;
	}

	case node_neg:
	case node_enum:
	case node_incr:
	case node_typeof:
	case node_return:  {
		exprNode *en = (exprNode *)(table + slot);
		slot = en->expr;
		continue;
	}

	case node_ternary: {
		ternaryNode *tn = (ternaryNode *)(table + slot);
		assignSlots(tn->condexpr, table, symbols);
		assignSlots(tn->trueexpr, table, symbols);
		slot = tn->falseexpr;
		continue;
	}

	case node_lor:
	case node_land:
	case node_math:
	case node_access:
	case node_lookup:
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		assignSlots(bn->left, table, symbols);
		slot = bn->right;
		continue;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		assignSlots(iftn->condexpr, table, symbols);
		assignSlots(iftn->thenstmt, table, symbols);
		slot = iftn->elsestmt;
		continue;
	}
	case node_elem: {
		binaryNode *bn = (binaryNode *)(table + slot);
		slot = bn->right;
		continue;
	}
	case node_array: {
		arrayNode *an = (arrayNode *)(table + slot);
		slot = an->exprlist;
		continue;
	}
	case node_obj: {
		objNode *on = (objNode *)(table + slot);
		slot = on->elemlist;
		continue;
	}
	case node_while:
	case node_dowhile: {
		whileNode *wn = (whileNode *)(table + slot);
		assignSlots(wn->cond, table, symbols);
		slot = wn->stmt;
		continue;
	}
	case node_forin: {
		forInNode *forn = (forInNode*)(table + slot);
		assignSlots(forn->var, table, symbols);
		assignSlots(forn->expr, table, symbols);
		slot = forn->stmt;
		continue;
	}
	case node_for: {
		forNode *forn = (forNode*)(table + slot);
		assignSlots(forn->init, table, symbols);
		assignSlots(forn->cond, table, symbols);
		assignSlots(forn->incr, table, symbols);
		slot = forn->stmt;
		continue;
	}
	case node_var:
	case node_ref: {
		symNode *sym = (symNode *)(table + slot);
		stringNode *name = (stringNode *)(table + sym->name);
		symbol_t *symbol = lookupSymbol(name->string, name->hdr->aux, symbols);

		if (!symbol) {
			firstNode *fn = findFirstNode(table, slot);
			printf("%s: Symbol not found: %s line = %d node = %d\n", fn->script, name->string, (int)sym->hdr->lineNo, slot);
			exit(1);
		}

		sym->level = symbols->depth - symbol->depth;
		sym->frameIdx = symbol->frameIdx;
		return;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(table + slot);
		assignSlots(fc->args, table, symbols);

		symNode *sym = (symNode *)(table + fc->name);

		if (sym->hdr->type != node_var) {
			assignSlots(fc->name, table, symbols);
			return;
		}

		stringNode *name = (stringNode *)(table + sym->name);
		symbol_t *symbol = lookupSymbol(name->string, name->hdr->aux, symbols);

		if (symbol) {
			sym->level = symbols->depth - symbol->depth;
			sym->frameIdx = symbol->frameIdx;
			return;
		}

		int idx = builtin(name);

		if (idx < 0) {
			firstNode *fn = findFirstNode(table, slot);
			printf("%s: Function not found: %s line = %d node = %d\n", fn->script, name->string, (int)sym->hdr->lineNo, slot);
			exit(1);
		}

		fc->hdr->type = node_builtin;
		fc->hdr->aux = idx;
		return;
	}
	default:
		if (debug)
			printf("node type %d assignment skipped\n", (int)table[slot].type);
		return;
	}
}

//	hoist and assign frame slots for symbol table entries

void compileScripts(uint32_t max, Node *table, symtab_t *symbols) {
	uint32_t start = 0;

	while(start < max) {
		firstNode *fn = (firstNode *)(table + start);
		start += fn->moduleSize;
		hoistSymbols(fn->begin, table, symbols);
	}

	start = 0;

	while(start < max) {
		firstNode *fn = (firstNode *)(table + start);
		assignSlots(fn->begin, table, symbols);
		start += fn->moduleSize;
	}
}

