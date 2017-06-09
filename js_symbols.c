#include "js.h"

extern int builtin (string_t *name);
extern symtab_t globalSymbols;

// symbol table

symbol_t *findSymbol(value_t symTab, value_t name, bool create, uint64_t hash) {
	int idx;

	if ((idx = lookupValue(symTab, name, hash)) > 0)
		return symTab.oval->pairsPtr[idx - 1].value.sym;

	if (create)
		return setAttribute(symTab.oval, name, -idx)->sym;

	return NULL;
}

symbol_t *lookupSymbol(string_t *name, symtab_t *symbols, symtab_t *block) {
	uint64_t hash = hashStr(name->val, name->len);
	value_t symbol, symName, symTab;
	symbol_t *sym;

	symName.bits = vt_string;
	symName.addr = name;

	symTab.bits = vt_object;

	if (hoistDebug)
		printf("lookupSymbol('%.*s')\n", name->len, name->val);

	while (block) {
	  symTab.oval = &block->entries;

	  if ((sym = findSymbol(symTab, symName, false, hash))) {
		if (!sym->fixed) {
		  if (block->parent)
			sym->frameIdx += block->parent->frameIdx + block->parent->baseIdx;
		  else
			sym->frameIdx += symbols->frameIdx;

		  sym->fixed = 1;
		}
		return sym;
	  } else
	  	block = block->parent;
	}

	while (symbols) {
	  symTab.oval = &symbols->entries;

	  if ((sym = findSymbol(symTab, symName, false, hash)))
		return sym;
	  else
	  	symbols = symbols->parent;
	}

	return NULL;
}

uint32_t insertSymbol(string_t *name, symtab_t *symbols, bool scoped) {
	uint64_t hash = hashStr(name->val, name->len);
	value_t symbol, symName, symTab;
	symbol_t *sym;

	symName.bits = vt_string;
	symName.addr = name;

	symTab.bits = vt_object;
	symTab.oval = &symbols->entries;

	if (!symbols)
		symbols = &globalSymbols;

	if (hoistDebug)
		printf("insertSymbol('%.*s')\n", name->len, name->val);

	if ((sym = findSymbol(symTab, symName, true, hash))) {
		sym->frameIdx = ++symbols->frameIdx;
		sym->depth = symbols->depth;
		sym->scoped = scoped;
		return sym->frameIdx;
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

		// make a new scope frame

		forn->symbols.depth = symbols->depth;
		forn->symbols.parent = block;
		block = &forn->symbols;

		hoistSymbols(forn->var, table, symbols, block);

		if (forn->symbols.parent) {
		  if (forn->symbols.parent->scopeCnt < block->frameIdx)
		    forn->symbols.parent->scopeCnt = block->frameIdx;
		} else {
		  if (symbols->scopeCnt < block->frameIdx)
			symbols->scopeCnt = block->frameIdx;
		}

		slot = forn->stmt;
		continue;
	}
	case node_for: {
		forNode *forn = (forNode*)(table + slot);

		// make a new scope frame

		forn->symbols.depth = symbols->depth;
		forn->symbols.parent = block;
		block = &forn->symbols;

		hoistSymbols(forn->init, table, symbols, block);

		if (forn->symbols.parent) {
		  if (forn->symbols.parent->scopeCnt < block->frameIdx)
		    forn->symbols.parent->scopeCnt = block->frameIdx;
		} else {
		  if (symbols->scopeCnt < block->frameIdx)
			symbols->scopeCnt = block->frameIdx;
		}

		slot = forn->stmt;
		continue;
	}
	case node_opassign:
	case node_assign: {
		binaryNode *bn = (binaryNode *)(table + slot);
		hoistSymbols(bn->left, table, symbols, block);
		slot = bn->right;
		continue;
	}
	case node_var: {
		symNode *sym = (symNode *)(table + slot);
		stringNode *sn = (stringNode *)(table + sym->name);

		if (sym->hdr->flag & flag_decl) {
		  if (sym->hdr->flag & flag_scope && block)
			insertSymbol(&sn->str, block, true);
		  else
			insertSymbol(&sn->str, symbols, false);
		}

		return;
	}
	case node_block: {
		blkEntryNode *be = (blkEntryNode *)(table + slot);

		//	make new scope

		be->symbols.depth = symbols->depth;
		be->symbols.parent = block;
		block = &be->symbols;

		hoistSymbols(be->body, table, symbols, block);

		if (be->symbols.parent) {
		  if (be->symbols.parent->scopeCnt < block->frameIdx)
		    be->symbols.parent->scopeCnt = block->frameIdx;
		} else {
		  if (symbols->scopeCnt < block->frameIdx)
			symbols->scopeCnt = block->frameIdx;
		}

		return;
	}
	case node_fcndef: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);

		// install the function name in symbol table

		symNode *sym = (symNode *)(table + fd->name);
		stringNode *sn = (stringNode *)(table + sym->name);
		sym->frameIdx = insertSymbol(&sn->str, symbols, false);
		sym->level = 0;

		if (hoistDebug)
			printf("node %d hoist fcndecl: %s\n", slot, sn->str.val);

		// add to list of child fcns

		fd->next = symbols->childFcns;
		symbols->childFcns = slot;
	}
	case node_fcnexpr: {
		fcnDeclNode *fd = (fcnDeclNode *)(table + slot);
		fd->symbols.depth = symbols->depth + 1;
		fd->symbols.parent = symbols;

		hoistSymbols(fd->params, table, &fd->symbols, block);
		fd->nparams = fd->symbols.frameIdx;

		// install fcn name from fcn expression

		if (fd->hdr->type == node_fcnexpr)
		  if (fd->name && table[fd->name].type == node_var) {
 	 		symNode *sym = (symNode *)(table + fd->name);
			stringNode *sn = (stringNode *)(table + sym->name);

			// install the function name in the table

			sym->frameIdx = insertSymbol(&sn->str, &fd->symbols, false);
			sym->level = 0;

			if (hoistDebug)
				printf("node %d hoist fcnexpr: %s\n", slot, sn->str.val);
		  }

		// hoist function body declarations

		hoistSymbols(fd->body, table, &fd->symbols, block);
		return;
	}
	case node_return: {
		exprNode *en = (exprNode *)(table + slot);
		slot = en->expr;
		continue;
	}
	default:
		if (hoistDebug)
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
	case node_opassign:
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
		block = &forn->symbols;

		// set our baseIdx after parent's frameIdx

		if (block->parent)
			block->baseIdx = block->parent->baseIdx + block->parent->frameIdx;
		else
			block->baseIdx = symbols->frameIdx;

		assignSlots(forn->var, table, symbols, block);
		assignSlots(forn->expr, table, symbols, block);

		slot = forn->stmt;
		continue;
	}
	case node_for: {
		forNode *forn = (forNode*)(table + slot);
		block = &forn->symbols;

		// set our baseIdx after parent's frameIdx

		if (block->parent)
			block->baseIdx = block->parent->baseIdx + block->parent->frameIdx;
		else
			block->baseIdx = symbols->frameIdx;

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

		// prepare for nested stmt block scope

		block = &be->symbols;

		// set our baseIdx after parent's frameIdx

		if (block->parent)
			block->baseIdx = block->parent->baseIdx + block->parent->frameIdx;
		else
			block->baseIdx = symbols->frameIdx;

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
			sym->frameIdx = symbol->frameIdx;
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
		if (hoistDebug)
			fprintf(stderr, "node %d type %d assignment skipped\n", slot, (int)table[slot].type);
		return;
	}
}

//	hoist and assign frame slots for symbol table entries

void compileScripts(uint32_t max, Node *table, symtab_t *symbols, symtab_t *block) {
	uint32_t start = 0;
	firstNode *fn;

	while(start < max) {
		fn = (firstNode *)(table + start);
		start += fn->moduleSize;
		hoistSymbols(fn->begin, table, symbols, block);
	}

	start = 0;

	while(start < max) {
		fn = (firstNode *)(table + start);
		assignSlots(fn->begin, table, symbols, block);
		start += fn->moduleSize;
	}
}

