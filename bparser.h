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

/**
 * @brief State for the stateful, backtracking parser.
 *
 * The parser uses a backtracking (recursive descent) method to resolve grammar
 * conflicts (e.g., `<atom> ::= "(" <expr> ")" | "(" <asgn> ")" ...`).
 *
 * When a rule fails, it uses the `tokens` stack to rewind the input stream and
 * attempts the next rule variant (`bnonterminal::variant`).
 */
struct b_parser {
	/** The token stack for backtracking. When a parsing rule fails,
	 * consumed tokens are pushed back onto this stack */
	struct b_stack tokens;
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

/**
 * @brief Continues the parsing process.
 *
 * This function acts like a coroutine, advancing the parse state until it
 * either completes a the start symbol sey by `b_parser_start` or requires more
 * tokens from the lexer.
 *
 * @param p Pointer to the parser.
 * @param out If the result is `BPARSER_READY`, this pointer is set to the
 *            root of the completed CST (`bsymbol *`).
 * @returns The status of the parse operation.
 */
enum b_parser_result b_parser_continue(struct b_parser *p, struct bsymbol **out);

/** @brief recursively frees a node and its children */
void b_parser_destroy_tree(struct bsymbol *);


#endif
