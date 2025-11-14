/**
 * @file bparser.h
 * @brief Boolean script parser
 *
 * Parsed node definitions and syntax-directed translators
 */

#ifndef BPARSER_H
#define BPARSER_H

#include "bmem.h"
#include "bgrammar.h"
#include "blex.h"


/** @brief parsing status */
enum b_parser_result {
	BPARSER_READY /** a tree is ready for consumption */,
	BPARSER_CONTINUE /** new tokens should be provided */,
	BPARSERE_NOMATCH
};

/** @brief struct for constructing parse trees */
struct b_parser {
	struct b_stack tokens /** stack of tokens */;
	struct b_lex lex /** underlying lexer */;
	struct bsymbol *root /** root of the tree */;
	struct bsymbol *cur /** (current) node that parsing continues on */;
};


/** @brief initializes a new parser */
void b_parser_init(struct b_parser *);

/** @brief @see b_lex_clearinput */
void *b_parser_clearinput(struct b_parser *);

/** @brief @see b_lex_setinput */
void *b_parser_setinput(struct b_parser *, struct bio *);

/** @brief set start symbol for next match */
void b_parser_start(struct b_parser *, enum bnt_type);

/** @brief continues current match */
enum b_parser_result b_parser_continue(struct b_parser *, struct bsymbol **out);

/** @brief recursively frees a node and its children */
void b_parser_destroy_tree(struct bsymbol *);


#endif
