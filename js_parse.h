//
// Parse context
//

typedef struct {
	uint32_t beginning;			// beginning of parse tree
	uint32_t tableSize;			// size of the parsetable
	uint32_t tableNext;			// size of the parsetable
	uint32_t lineNo;			// script line number
	void *scanInfo;				// yyscanner context
	char *script;				// name of the script
	Node *table;
} parseData;

typedef enum {
	node_first = 0,
	node_endlist,
	node_incr,		// ++, --
	node_enum,		// enum decl
	node_math,		// math expr
	node_neg,		// - expr
	node_num,		// number
	node_string,	// string
	node_fcncall,	// func( exprlist )
	node_builtin,	// builtinfunc( exprlist )
	node_var,		// symbol
	node_ref,		// &symbol
	node_assign,	// lval (+|-)= rval
	node_return,	// return stmt
	node_ifthen,	// if (c) {..} else {..}
	node_while,		// while (c) do {..}
	node_dowhile,	// do {..} while (c)
	node_for,		// for (e; c; e) {..}
	node_forin,		// for (var x in|of y) {..}
	node_list,		// {expr,decl,arg}list
	node_elemlist,	// elem, elem, ...
	node_array,		// [ a, b, ... ]
	node_obj,		// { elem, elem, ... }
	node_fcnexpr,	// fcn ( paramlist )
	node_fcndef,	// function f(.) { .. }
	node_elem,		// name : value
	node_lookup,	// x[1] or x["b"]
	node_access,	// x.prop
	node_typeof,	// typeof x
	node_ternary,	// expr ? expr : expr
	node_land,		// expr && expr
	node_lor,		// expr || expr
	node_MAX
} nodeType;

union ParseNode {
	struct {
		uint64_t type:6;			// type of parse node
		uint64_t flag:5;			// node flags
		uint64_t aux:22;			// node length/parameter
		uint64_t lineNo:31;			// script line number
	};
	nodeType display:6;
	uint64_t bits;
};

typedef enum {
	for_in,
	for_of
} forin;

typedef enum {
	incr_before,
	incr_after,
	decr_before,
	decr_after,
} incrdecr;

typedef enum {
	pm_assign,
	pm_add,
	pm_sub,
	pm_mpy,
	pm_div,
	pm_mod,
	pm_math,
	pm_and,
	pm_xor,
	pm_or,
	pm_lshift,
	pm_rshift
} plusminus;

typedef enum {
	neg_uminus,
	neg_bitnot,
	neg_not
} negate;

typedef enum {
	math_add,		// expr + epxr
	math_sub,		// expr - expr
	math_mpy,		// expr * expr
	math_div,		// expr / expr
	math_mod,		// expr % expr
	math_comp,		// last comp type
	math_or,		// expr | expr
	math_and,		// expr & expr
	math_xor,		// expr ^ expr
	math_lshift,	// expr << expr
	math_rshift,	// expr >> expr
	math_rushift,	// expr >>> expr
	math_bits,		// last bits type
	math_lt,		// expr < expr
	math_le,		// expr <= expr
	math_eq,		// expr == expr
	math_ne,		// expr != expr
	math_ge,		// expr >= expr
	math_gt,		// expr > expr
} mathops;

typedef enum {
	nn_dbl,
	nn_int,
	nn_bool,
	nn_null,
	nn_this,
	nn_args,
	nn_undef,
	nn_infinity,
	nn_nan,
} numNodeType;

typedef struct {
	Node hdr[1];
	uint32_t begin;
	uint32_t fcnChain;
	uint32_t moduleSize;
	uint8_t script[1];
} firstNode;

uint32_t newNode (parseData *pd, nodeType type, uint32_t size, bool zero);
uint32_t newStrNode (parseData *pd, char *text, uint32_t size);
firstNode *findFirstNode(Node *table, uint32_t slot);

typedef struct {
	Node hdr[1];
	uint32_t condexpr;
	uint32_t trueexpr;
	uint32_t falseexpr;
} ternaryNode;

typedef struct {
	Node hdr[1];
	uint32_t condexpr;
	uint32_t thenstmt;
	uint32_t elsestmt;
} ifThenNode;

typedef struct {
	Node hdr[1];
	uint32_t name;
	uint32_t args;
} fcnCallNode;

typedef struct {
	Node hdr[1];
	char string[0];
} stringNode;

typedef struct {
	Node hdr[1];
	uint32_t right;
	uint32_t left;
} binaryNode;

typedef struct {
	Node hdr[1];
	union {
		int64_t intval;
		double dblval;
		bool boolval;
	};
} numNode;

typedef struct {
	Node hdr[1];
	uint16_t frameIdx;
	uint16_t level;
	uint32_t name;
} symNode;

typedef struct {
	Node hdr[1];
	uint32_t exprlist;
} arrayNode;

typedef struct {
	Node hdr[1];
	uint32_t elemlist;
} objNode;

typedef struct {
	Node hdr[1];
	uint32_t init;
	uint32_t cond;
	uint32_t incr;
	uint32_t stmt;
} forNode;

typedef struct {
	Node hdr[1];
	uint32_t var;
	uint32_t expr;
	uint32_t stmt;
} forInNode;

typedef struct {
	Node hdr[1];
	uint32_t cond;
	uint32_t stmt;
} whileNode;

typedef struct {
	Node hdr[1];
	uint32_t expr;
} exprNode;

typedef struct {
	Node hdr[1];
	uint32_t elem;
} listNode;

struct FcnDeclNode {
	Node hdr[1];
	uint32_t name;
	uint32_t body;
	uint32_t next;
	uint32_t params;
	uint32_t nparams;
	uint32_t nsymbols;
	symtab_t symbols[1];
};

