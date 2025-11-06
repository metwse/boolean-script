/**
 * @file bparser.h
 * @brief Boolean script parser
 *
 * Parsed node definitions and syntax-directed translators
 */

#ifndef BPARSER_H
#define BPARSER_H

#include "bdef.h"
#include "bgrammar.h"
#include "blex.h"

#include <stdbool.h>


/** @brief parse tree node */
struct b_ptree_node {
	struct b_ptree_node *parent /** parent node */;
	struct bsymbol symbol /** terminal/nonterminal symbol */;
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

/** @brief struct for constructing parse trees */
struct b_parser {
	struct btoken *tokens /** stack of tokens */;
	b_umem token_count /** size of token stack */;
	struct b_lex lex /** underlying lexer */;
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
