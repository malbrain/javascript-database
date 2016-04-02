//
// Parse context
//

union ParseNode {
	struct {
		uint32_t type:5;			// type of parse node
		uint32_t flag:5;			// node flags
		uint32_t aux:22;			// node length/parameter
		uint32_t lineno;			// script line number
	};
	uint64_t bits;
};

typedef struct {
	uint32_t beginning;			// beginning of parse tree
	uint32_t tablesize;			// size of the parsetable
	uint32_t tablenext;			// size of the parsetable
	uint32_t lineno;			// beginning of parse tree
	void *scaninfo;				// yyscanner context
	char *script;				// name of the script
	Node *table;
} parseData;

typedef enum {
	node_endlist = 0,
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
	node_list,		// {expr,decl,arg}list
	node_elemlist,	// elem, elem, ...
	node_array,		// [ a, b, ... ]
	node_obj,		// { elem, elem, ... }
	node_fcnexpr,	// fcn ( paramlist )
	node_fcndef,	// function f(.) { .. }
	node_elem,		// name : value
	node_lookup,	// x[1] or x["b"]
	node_access,	// x.prop
	node_MAX
} nodeType;

enum flagType {
	flag_return	= 0,
	flag_continue = 1,
	flag_break	= 2,
	flag_error	= 3,
	flag_throw	= 4,
	flag_newobj	= 5,		// node produces new obj
	flag_typemask	= 7,
	flag_decl		= 8,	// node is a symbol declaration
	flag_lval		= 16,	// node produces lval
};

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
	pm_lshift,
	pm_rshift
} plusminus;

typedef enum {
	math_add,		// expr + epxr
	math_sub,		// expr - expr
	math_mpy,		// expr * expr
	math_div,		// expr / expr
	math_lshift,	// expr << expr
	math_rshift,	// expr >> expr
	math_comp,		// first comparison type
	math_lt,		// expr < expr
	math_le,		// expr <= expr
	math_eq,		// expr == expr
	math_ne,		// expr != expr
	math_ge,		// expr >= expr
	math_gt			// expr > expr
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

uint32_t newNode (parseData *pd, nodeType type, uint32_t size, bool zero);

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
	uint8_t string[0];
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
	uint32_t frameidx;
	uint32_t level;
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

typedef struct {
	Node hdr[1];
	uint32_t name;
	uint32_t params;
	uint32_t body;
	uint32_t next;
	uint32_t fcn;
	uint32_t nparams;
	uint32_t nsymbols;
} fcnDeclNode;

