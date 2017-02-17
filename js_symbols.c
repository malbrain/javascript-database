#include "js.h"

extern int builtin (string_t *name);
extern symtab_t globalSymbols;

// symbol table

symbol_t *lookupSymbol(string_t *name, symtab_t *symbols, symtab_t *block) {
	value_t *symbol, symName;

	symName.bits = vt_string;
	symName.addr = name;

	if (debug)
		printf("lookupSymbol('%.*s')\n", name->len, name->val);

	while (block) {
	  if ((symbol = lookup(&block->entries, symName, false, 0)))
		return symbol->sym;
	  else
	  	block = block->parent;
	}

	while (symbols) {
	  if ((symbol = lookup(&symbols->entries, symName, false, 0)))
		return symbol->sym;
	  else
	  	symbols = symbols->parent;
	}

	return NULL;
}

uint32_t insertSymbol(string_t *name, symtab_t *symbols, bool scoped) {
	value_t *symbol, symName;

	symName.bits = vt_string;
	symName.addr = name;

	if (!symbols)
		symbols = &globalSymbols;

	if (debug)
		printf("insertSymbol('%.*s')\n", name->len, name->val);

	if ((symbol = lookup(&symbols->entries, symName, true, 0))) {
		symbol->sym->frameIdx = ++symbols->frameIdx;
		symbol->sym->depth = symbols->depth;
		symbol->sym->scoped = scoped;
		return symbol->sym->frameIdx;
	}

	return 0;
}

void hoistSymbols(uint32_t slot, Node *table, symtab_t *symbols, symtab_t *block) {

  while (slot)
	switch (table[slot].type) {
	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(table + slot);
			hoistSymbols(ln->elem, table, symbols, block);
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
		hoistSymbols(fc->name, table, symbols, block);
		
		slot = fc->args;
		continue;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		hoistSymbols(iftn->thenstmt, table, symbols, block);
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
		forn->symbols.depth = block ? block->depth + 1 : 0; 
		forn->symbols.parent = block;

		// do we need a new LET frame?

		if (forn->hdr->flag & flag_frame) {
			hoistSymbols(forn->var, table, symbols, &forn->symbols);
			hoistSymbols(forn->stmt, table, symbols, &forn->symbols);

			// did we expand enclosing block's maximum block idx?

			if (symbols->scopeCnt < forn->symbols.frameIdx)
				symbols->scopeCnt = forn->symbols.frameIdx;
		} else {
			hoistSymbols(forn->var, table, symbols, block);
			hoistSymbols(forn->stmt, table, symbols, block);
		}

		slot = forn->expr;
		continue;
	}
	case node_for: {
		forNode *forn = (forNode*)(table + slot);
		forn->symbols.depth = block ? block->depth + 1 : 0; 
		forn->symbols.parent = block;

		// do we need a new LET frame?

		if (forn->hdr->flag & flag_frame) {
			hoistSymbols(forn->init, table, symbols, &forn->symbols);
			hoistSymbols(forn->stmt, table, symbols, &forn->symbols);

			// did we expand enclosing block's maximum block idx?

			if (symbols->scopeCnt < forn->symbols.frameIdx)
				symbols->scopeCnt = forn->symbols.frameIdx;
		} else {
			hoistSymbols(forn->init, table, symbols, block);
			hoistSymbols(forn->stmt, table, symbols, block);
		}

		slot = forn->stmt;
		continue;
	}
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		hoistSymbols(bn->left, table, symbols, block);
		slot = bn->right;
		continue;
	}
	case node_var: {
		symNode *sym = (symNode *)(table + slot);
		stringNode *sn = (stringNode *)(table + sym->name);

		if (sym->hdr->flag & flag_decl)
		  if (sym->hdr->flag & flag_scope && block)
			insertSymbol(&sn->str, block, true);
		  else
			insertSymbol(&sn->str, symbols, false);

		return;
	}
	case node_block: {
		blkEntryNode *be = (blkEntryNode *)(table + slot);
		be->symbols.parent = block ? block : symbols;
		be->symbols.depth = symbols->depth;

		//	are we in a FOR block?

		if ((be->hdr->flag & flag_frame))
			hoistSymbols(be->body, table, symbols, &be->symbols);
		else
			hoistSymbols(be->body, table, symbols, block);

		if (symbols->scopeCnt < be->symbols.frameIdx)
			symbols->scopeCnt = be->symbols.frameIdx;

		return;
	}
	case node_fcndef: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);

		// install the function name in symbol table

		symNode *sym = (symNode *)(table + fd->name);
		stringNode *sn = (stringNode *)(table + sym->name);
		sym->frameIdx = insertSymbol(&sn->str, symbols, false);
		sym->level = 0;

		if (debug)
			printf("node %d hoist fcndecl: %s\n", slot, sn->str.val);

		// add to list of child fcns

		fd->next = symbols->childFcns;
		symbols->childFcns = slot;
	}
	case node_fcnexpr: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);
		fd->symbols.depth = symbols->depth + 1;
		fd->symbols.parent = symbols;

		hoistSymbols(fd->params, table, &fd->symbols, NULL);
		fd->nparams = fd->symbols.frameIdx;

		// install fcn name from fcn expression

		if (fd->hdr->type == node_fcnexpr)
		  if (fd->name && table[fd->name].type == node_var) {
 	 		symNode *sym = (symNode *)(table + fd->name);
			stringNode *sn = (stringNode *)(table + sym->name);

			// install the function name in the table

			sym->frameIdx = insertSymbol(&sn->str, &fd->symbols, false);
			sym->level = 0;

			if (debug)
				printf("node %d hoist fcnexpr: %s\n", slot, sn->str.val);
		  }

		// hoist function body declarations

		hoistSymbols(fd->body, table, &fd->symbols, NULL);
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

void assignSlots(uint32_t slot, Node *table, symtab_t *symbols, symtab_t *block)
{
  while (slot)
	switch (table[slot].type) {
	case node_fcndef:
	case node_fcnexpr: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);
		fd->symbols.depth = symbols->depth + 1;
		assignSlots(fd->body, table, &fd->symbols, block);
		return;
	}

	case node_endlist:
	case node_list: {
		listNode *ln;

		do {
			ln = (listNode *)(table + slot);
			assignSlots(ln->elem, table, symbols, block);
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
		assignSlots(tn->condexpr, table, symbols, block);
		assignSlots(tn->trueexpr, table, symbols, block);
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
		assignSlots(bn->left, table, symbols, block);
		slot = bn->right;
		continue;
	}
	case node_ifthen: {
		ifThenNode *iftn = (ifThenNode *)(table + slot);
		assignSlots(iftn->condexpr, table, symbols, block);
		assignSlots(iftn->thenstmt, table, symbols, block);
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
		assignSlots(wn->cond, table, symbols, block);
		slot = wn->stmt;
		continue;
	}
	case node_forin: {
		forInNode *forn = (forInNode*)(table + slot);

		if (forn->hdr->flag & flag_frame) {
			hoistSymbols(forn->var, table, &forn->symbols, block);
			hoistSymbols(forn->stmt, table, &forn->symbols, block);
		} else {
			assignSlots(forn->var, table, symbols, &forn->symbols);
			assignSlots(forn->stmt, table, symbols, &forn->symbols);
		}

		slot = forn->expr;
		continue;
	}
	case node_for: {
		forNode *forn = (forNode*)(table + slot);
		assignSlots(forn->init, table, symbols, block);
		assignSlots(forn->cond, table, symbols, block);
		assignSlots(forn->incr, table, symbols, block);
		slot = forn->stmt;
		continue;
	}
	case node_var: {
		symNode *sym = (symNode *)(table + slot);
		stringNode *sn = (stringNode *)(table + sym->name);
		symbol_t *symbol = lookupSymbol(&sn->str, symbols, block);

		if (!symbol) {
			firstNode *fn = findFirstNode(table, slot);
			fprintf(stderr, "%s: Symbol not found: %s line = %d node = %d\n", fn->script, sn->str.val, (int)sym->hdr->lineNo, slot);
			exit(1);
		}

		if (symbol->scoped)
			sym->hdr->flag |= flag_scope;

		sym->level = symbols->depth - symbol->depth;
		sym->frameIdx = symbol->frameIdx;
		return;
	}
	case node_block: {
		blkEntryNode *be = (blkEntryNode *)(table + slot);

		if ((be->hdr->flag & flag_frame)) {
			assignSlots(be->body, table, symbols, &be->symbols);
			return;
		}

		slot = be->body;
		continue;
	}
	case node_fcncall: {
		fcnCallNode *fc = (fcnCallNode *)(table + slot);
		assignSlots(fc->args, table, symbols, block);

		symNode *sym = (symNode *)(table + fc->name);

		if (sym->hdr->type != node_var) {
			assignSlots(fc->name, table, symbols, block);
			return;
		}

		stringNode *sn = (stringNode *)(table + sym->name);
		symbol_t *symbol = lookupSymbol(&sn->str, symbols, block);

		if (symbol) {
			sym->level = symbols->depth - symbol->depth;
			sym->frameIdx = symbols->frameIdx;
			return;
		}

		int idx = builtin(&sn->str);

		if (idx < 0) {
			firstNode *fn = findFirstNode(table, slot);
			fprintf(stderr, "%s: Function not found: %s line = %d node = %d\n", fn->script, sn->str.val, (int)sym->hdr->lineNo, slot);
			exit(1);
		}

		fc->hdr->type = node_builtin;
		fc->hdr->aux = idx;
		return;
	}
	default:
		if (debug)
			fprintf(stderr, "node %d type %d assignment skipped\n", slot, (int)table[slot].type);
		return;
	}
}

//	hoist and assign frame slots for symbol table entries

void compileScripts(uint32_t max, Node *table, symtab_t *symbols) {
	uint32_t start = 0;
	firstNode *fn;

	while(start < max) {
		fn = (firstNode *)(table + start);
		start += fn->moduleSize;
		hoistSymbols(fn->begin, table, symbols, NULL);
	}

	start = 0;

	while(start < max) {
		fn = (firstNode *)(table + start);
		assignSlots(fn->begin, table, symbols, NULL);
		start += fn->moduleSize;
	}
}

