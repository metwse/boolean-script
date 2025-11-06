/**
 * @file bparser.h
 * @brief Boolean script parser
 *
 * Parsed node definitions and syntax-directed translators
 */

#ifndef BPARSER_H
#define BPARSER_H

#include "bdef.h"
#include "blex.h"

#include <stdbool.h>


/**
 * Macro for defining types related with terminals/symbols
 *
 * A macro named "x" must be defined to process those symbols.
 */
#define BT_CONSTRUCT \
	/* primitive types */ \
	x(BIT), x(POSITIVE_INT), \
	/* identifiers */ \
	x(IDENT), x(IDENT_OR_MEMBER), \
	\
	x(TY), \
	\
	/* right-recursive expressions */\
	x(EXPR), x(EXPR_REST), \
	x(TERM), x(FACTOR_REST), \
	x(ATOM), \
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


/** @brief parse tree node kind */
enum b_ptree_construct {
	/** @see BT_CONSTRUCT @internal */
	#define x(t) BT_ ## t
	BT_CONSTRUCT,
	#undef x
};

static const char *const b_ptree_constructs[] = {
	/** @see BT_CONSTRUCT @internal */
	#define x(t) # t
	BT_CONSTRUCT,
	#undef x
};

/** @brief parse tree node */
struct b_ptree_node {
	struct b_ptree_node *parent /** parent node */;
	struct b_ptree_node *nodes /** child nodes */;
	enum b_ptree_construct kind /** node kind */;
	union b_seminfo seminfo /** additional semantic info */;
	b_umem node_count /** child node count */;
};

/** @brief parse tree plus parser state for incremental parse */
struct b_ptree {
	struct b_ptree_node *root /** root of the tree */;
	bool fulfilled /** a statement has successfully been matched */;
};

/** @brief parsing status */
enum b_parser_result {
	BPARSER_READY = 1 /** a tree is ready for consumption */,
	BPARSER_CONTINUE /** new tokens should be provided */,
	BPARSER_NOTOKEN /** new input source should be provided */,
	BPARSERE
};

struct b_parser {
	struct b_tokens *tokens /** stack of tokens */;
	b_umem token_count /** size of token stack */;
	struct b_lex *lex /** underlying lexer */;
};


/** initializes a new parser */
void b_parser_init(struct b_parser *);

/** @see b_lex_clearinput */
void *b_parser_clearinput(struct b_parser *);

/** @see b_lex_setinput */
void *b_parser_setinput(struct b_parser *, struct bio *);

/** matches the next statement */
enum b_parser_result b_parser_try_next(struct b_parser *, struct b_ptree *out);


#endif
