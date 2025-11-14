/**
 * @file bgrammar.h
 * @brief Boolean script Context-Free Grammar (CFG) and Concrete Syntax Tree
 * (CST) definitions.
 *
 * This file defines two main components:
 * 1. The list of terminal (`BTOKENS`) and non-terminal (`BNONTERMINALS`)
 *    symbols that make up the language's grammar.
 * 2. The `bsymbol` struct, which is the node type for the CST produced by
 *    `b_parser`.
 *
 * See @ref grammar.c to context-free grammar definition.
 *
 * @note The `bsymbol` (CST) is a detailed, literal representation of the
 * source code, including all syntax (parentheses, operators, etc.). It should
 * be transformed into a cleaner Abstract Syntax Tree (AST) (defined in
 * `bast.h`) for interpretation.
 */

#ifndef BGRAMMAR_H
#define BGRAMMAR_H

#include "bdef.h"


/**
 * @brief Macro for defining types related with tokens.
 *
 * A macro named "x" must be defined to process those nonterminals.
 */
#define BTOKENS \
	x(NOTOKEN) /* no more token */, \
	/* terminal symbols denoted by reserved words or symbols */ \
	x(FALSE), x(TRUE), \
	\
	/* 1-character tokens */ \
	x(OR), x(AND), x(INVOLUTION), x(COMMA), x(EQ), x(DOT), \
	x(LPAREN), x(RPAREN), \
	x(LANGLE_BRACKET), x(RANGLE_BRACKET), \
	x(LBRACKET), x(RBRACKET), \
	x(LCURLY), x(RCURLY), \
	\
	x(ESCAPE), x(NEWLINE), x(SEMI), \
	\
	/* multi-character punctuation */ \
	x(RARROW), \
	\
	/* keywords and reserved words */ \
	x(TY_BOOL), x(TY_VEC), x(EXPR), x(RETURN), \
	\
	/* tokens with auxiliary data */ \
	x(POSITIVE_INT), x(IDENT)

/** number of token types */
#define BTOKEN_COUNT 27

/**
 * @brief Macro for defining types related with nonterminals.
 *
 * A macro named "x" must be defined to process those nonterminals.
 */
#define BNONTERMINALS \
	/* primitive types */ \
	x(BIT), x(POSITIVE_INT), \
	/* identifiers */ \
	x(IDENT), x(IDENT_OR_MEMBER), \
	\
	x(TY), x(TY_BOOL), x(TY_VEC), \
	\
	/* right-recursive expressions */\
	x(EXPR_OR_INITLIST), \
	x(INITLIST), \
	BNT_RR(EXPR), \
	BNT_RR(TERM), \
	x(FACTOR), x(ATOM), x(OPTINVOLUTION), \
	/* funciton calls */ \
	x(CALL), BNT_RR(CALL_PARAMS), x(CALL_OPTPARAMS), \
	\
	/* statements */ \
	x(STMT), x(STMTS), \
	x(VAR_DECL), x(VAR_DECL_OPTASGN), \
	/* function declarations */ \
	x(EXPR_DECL), \
	x(EXPR_DECL_PARAM), BNT_RR(EXPR_DECL_PARAMS), x(EXPR_DECL_OPTPARAMS), \
	\
	/* right-recursive list types */\
	BNT_RR(ASGN), \
	\
	BNT_RR(IDENT_LS), \
	BNT_RR(IDENT_OR_MEMBER_LS), \
	\
	BNT_RR(EXPR_LS), \
	BNT_RR(EXPR_OR_INITLIST_LS)

/** number of nonterminal types */
#define BNONTERMINAL_COUNT 39

/** right-recursive nonterminal definition */
#define BNT_RR(nt) x(nt), x(nt ## _REST)


static const char *const btokens[BTOKEN_COUNT] = {
	"<>",

	"0", "1",

	"+", "*", "'", ",", "=", ".",
	"(", ")", "<", ">", "[", "]", "{", "}",

	"\\", "\n", ";",

	"->",

	"bool", "vec", "expr", "return",

	"@[1-9] [0-9]*", "@[a-z] ([a-z] | [A-Z] | [0-9] | \"_\")*",
};

static const char *const btoken_names[BTOKEN_COUNT] = {
	/** @see BTOKENS @internal */
	#define x(t) # t
	BTOKENS,
	#undef x
};

static const char *const bnonterminals[BNONTERMINAL_COUNT] = {
	/** @see BNONTERMINALS @internal */
	#define x(t) # t
	BNONTERMINALS,
	#undef x
};

/** @brief terminal (token) object for context-free grammar */
struct btoken {
	/** kind of a token */
	enum btk_type {
		/** @see BTOKENS @internal */
		#define x(t) BTK_ ## t
		BTOKENS,
		#undef x
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
	} ty /** Type of the nonterminal */;

	struct bsymbol **children /** child nodes */;
	b_umem child_count /** child node count */;

	/** @brief The production rule variant being parsed
	 *
	 * This field is for the internal use of the `b_parser`. When the
	 * backtracking parser tries different production rules for a
	 * non-terminal (e.g., `<atom> ::= "(" <expr> ")" | <call> ...`), it
	 * increments this index to track which variant it is currently
	 * attempting. This is purely parse-time data. */
	b_umem variant;
};

/**
 * @brief A node in the Concrete Syntax Tree (CST).
 *
 * `bsymbol` is a tagged union that can represent either:
 * 1. A Non-Terminal (an internal node, e.g., `<expr>`, `<stmt>`)
 * 2. A Token (a terminal/leaf node, e.g., `+`, `my_var`)
 */
struct bsymbol {
	union {
		/** nonterminal type of symbol */
		struct bnonterminal nt;
		/** terminal (token) type of symbol */
		struct btoken tk;
	};

	/** @brief The type of `bsymbol` (the union's tag) */
	enum bsymbol_type {
		BSYMBOL_NONTERMINAL,
		BSYMBOL_TOKEN,
	} ty /** type of the symbol */;

	/** A back-pointer to the parent node in the tree. This is NULL for the
	 * root node. */
	struct bsymbol *parent;
};


#endif
