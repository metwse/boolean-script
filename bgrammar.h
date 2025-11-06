/**
 * @file bgrammar.h
 * @brief Definitions for Boolen script context-free grammar.
 */

#ifndef BGRAMMAR_H
#define BGRAMMAR_H

#include "bdef.h"


/**
 * Macro for defining types related with tokens.
 *
 * A macro named "x" must be defined to process those nonterminals.
 */
#define BTOKENS \
	x(NOTOKEN) /* no more token */, \
	/* terminal symbols denoted by reserved words or symbols */ \
	x(FALSE), x(TRUE), \
	\
	/* 1-character tokens */ \
	x(OR), x(AND), x(INVOLUTION), x(DELIM), x(ASGN), x(SUBSCRIPT), \
	x(L_PAREN), x(R_PAREN), \
	x(L_ANGLE_BRACKET), x(R_ANGLE_BRACKET), \
	x(L_BRACKET), x(R_BRACKET), \
	x(L_CURLY), x(R_CURLY), \
	\
	x(ESCAPE), x(NEWLINE), x(STMT_DELIM), \
	\
	/* keywords and reserved words */ \
	x(TY_BOOL), x(TY_VEC), \
	\
	/* tokens with auxiliary data */ \
	x(POSITIVE_INT), x(IDENT)

/**
 * Macro for defining types related with nonterminals.
 *
 * A macro named "x" must be defined to process those nonterminals.
 */
#define BNONTERMINALS \
	/* for empty leaves */ \
	x(EPSILON), \
	\
	/* primitive types */ \
	x(BIT), x(POSITIVE_INT), \
	/* identifiers */ \
	x(IDENT), x(IDENT_OR_MEMBER), \
	\
	x(TY), \
	\
	/* right-recursive expressions */\
	x(EXPR), x(EXPR_REST), \
	x(TERM), x(TERM_REST), \
	x(FACTOR), x(ATOM), \
	/* funciton calls */\
	x(CALL), x(OPTPARAMS), \
	\
	/* statements */\
	x(STMT), x(STMTS),\
	x(DECL), x(DECL_OPTASGNS), \
	\
	/* right-recursive list types */\
	x(ASGNS), x(ASGNS_REST), \
	\
	x(IDENT_LS), x(IDENT_LS_REST), \
	x(IDENT_OR_MEMBER_LS), x(IDENT_OR_MEMBER_LS_REST), \
	\
	x(EXPR_LS), x(EXPR_LS_REST)


static const char *const btokens[] = {
	"<>",

	"0", "1",

	"+", "*", "'", ",", "=", ".",
	"(", ")", "<", ">", "[", "]", "{", "}",

	"\\", "\n", ";",

	"bool", "vec",

	"<positive-int>", "<ident>",
};

static const char *const btoken_names[] = {
	/** @see BTOKENS @internal */
	#define x(t) # t
	BTOKENS,
	#undef x
};

static const char *const bnonterminals[] = {
	/** @see BNONTERMINALS @internal */
	#define x(t) # t
	BNONTERMINALS,
	#undef x
};

/** @brief terminal (token) object for context-free grammar */
struct btoken {
	/** kind of a token */
	enum btk_type {
		BTK_NOTOKEN = 0,

		BTK_FALSE, BTK_TRUE,

		BTK_OR, BTK_AND, BTK_INVOLUTION, BTK_DELIM, BTK_ASGN, BTK_SUBSCRIPT,
		BTK_L_PAREN, BTK_R_PAREN,
		BTK_L_ANGLE_BRACKET, BTK_R_ANGLE_BRACKET,
		BTK_L_BRACKET, BTK_R_BRACKET,
		BTK_L_CURLY, BTK_R_CURLY,

		BTK_ESCAPE, BTK_NEWLINE, BTK_STMT_DELIM,

		BTK_TY_BOOL, BTK_TY_VEC,

		BTK_POSITIVE_INT, BTK_IDENT
	} ty /** type of the token (terminal) */;

	union {
		/** for `TK_POSITIVE_INT` */
		b_umem positive_int;
		/** for `TK_IDENT` */
		char *ident;
	} info /** auxiliary token info */;
};

/** @brief nonterminal (syntactic variable) object for context-free grammar */
struct bnonterminal {
	/** @brief parse tree node kind */
	enum bnt_type {
		/** @see BNONTERMINALS @internal */
		#define x(t) BNT_ ## t
		BNONTERMINALS,
		#undef x
	} ty /** type of the nonterminal */;

	struct bsymbol *children /** child nodes */;
	b_umem child_count /** child node count */;
};

/** @brief terminal or nonterminal token */
struct bsymbol {
	union {
		/** nonterminal type of symbol */
		struct bnonterminal nt;
		/** terminal (token) type of symbol */
		struct btoken tk;
	};
	/** @brief symbol type */
	enum bsymbol_type {
		BSYMBOL_NONTERMINAL,
		BSYMBOL_TOKEN,
	} ty /** type of the symbol */;
};


#endif
