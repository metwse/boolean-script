/**
 * @file bparser.h
 * @brief Parser
 *
 * Parse tree.
 */

#ifndef BPARSER_H
#define BPARSER_H

#include "bdef.h"


/* SYMBOLS
 * ======================================================================
 */

/* -- general -- */

/** 1 or 0 */
enum bbit {
	BBIT_TRUE,
	BBIT_FALSE,
};

/** identifier */
struct bident {
	const char *ident /** identifier name */;
	b_umem index /** index for vectors */;
};

/** type */
struct bty {
	enum {
		BTY_BOOL,
		BTY_VEC,
	} type /** optional variable type, default to bool */;
	b_umem size /** for vectors */;
};

/* -- expressions -- */

/** unit of a expression (atomic) */
struct batom {
	enum {
		BATOM_EXPR,
		BATOM_INITLIST,
		BATOM_ASGNS,
		BATOM_CALL,
		BATOM_IDENT,
		BATOM_BIT,
	} kind /** kind of the atomic */;
	union {
		struct bexpr *expr /** @see bexpr */;
		struct binitlist *initlist /** @see binitlist */;
		struct basgns *asgns /** @see basgns */;
		struct bcall *call /** @see bcall */;
		struct bident *ident /** @see bident */;
		enum bbit bit;
	} atom /** underlying atomic */;
};

/** factor, for precedence of INVOLUTION */
struct bfactor {
	enum {
		BFACTOR,
		BFACTOR_INVERSE,
	} kind /** whether or not to take inverse of atom */;
	struct batom atom /** @see batom */;
};

/** term, for precedence of AND */
struct bterm {
	enum {
		BTERM_FACTOR,
		BTERM_AND,
	} kind /** a factor or a factor AND'ed with a term */;
	struct bterm *term /** available in AND form */;
	struct bfactor factor /** @see bfactor */;
};

/** expression */
struct bexpr {
	enum {
		BEXPR_TERM,
		BEXPR_OR,
	} kind /** a term or a term OR'ed with a expr */;
	struct bexpr *expr /** available in OR form */;
	struct bterm term /** @see bterm */;
};

/** vector initializer list */
struct binitlist {
	struct expr *exprs /** list of expressions */;
	b_umem size /** size of that list */;
};

/**
 * right-recursive assignment list
 *
 * `tuple_size` greater than "1" iff the expression is `bcall` and returns
 * a tuple.
 */
struct basgns {
	struct bident **idents /** list of tuple of identifiers */;
	b_umem size /** size of that list*/;
	b_umem tuple_size /** size of that tuple */;
	struct bexpr expr /** related expression */;
};

/** expression call */
struct bcall {
	const char *ident /** expression identifier */;
	struct expr *params /** optional list of params */;
	b_umem size /** size of that list*/;
};

/* -- statements -- */

/** variable declaration statement */
struct bdecl {
	struct bty type /** type of the variable */;
	const char *ident /** variable name */;
};

/** block statement */
struct bstmts {
	struct bstmt *stmts /** list of statements */;
	b_umem stmt_count /** size of that list */;
};

/** statement */
struct bstmt {
	enum {
		BSTMT_DECL,
		BSTMT_ASGNS,
		BSTMT_EXPR,
		BSTMT_BLOCK,
	} kind /** kind of the statement */;
	union {
		struct bdecl decl /** @see bdecl */;
		struct basgns asgns /** @see basgns */;
		struct bexpr bexpr /** @see bexpr */;
		struct bstmts stmts /** @see bstmts */;
	};
};

/* ======================================================================
 * END SYMBOLS
 */

/** @brief a Boolean script program */
struct b_prog {
	struct bstmts statements /** the parser tree */;
};


#endif
