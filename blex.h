/**
 * @file blex.h
 * @brief Lexer
 *
 * Boolean script lexical analyzer.
 */

#ifndef BLEX_H
#define BLEX_H

#include "bdef.h"
#include "bgrammar.h"
#include "bio.h"


/** maximum valid identifier length */
#define BLEX_MAX_IDENT_LEN 32

/** maximum valid positive integer length */
#define BLEX_MAX_POSITIVE_INT_LEN 10


/** @brief lexer error details */
enum b_lex_result {
	BLEXOK = 0,
	BLEXE_IDENT_TOO_LONG /** identifier longer than `BLEX_MAX_IDENT_LEN` */,
	BLEXE_INTEGER_TOO_LARGE /** integer string longer than
				    `BLEX_MAX_POSITIVE_INT_LEN` */,
	BLEXE_INVALID_ESCAPE_CHAR /** invalid escape charachter (backslash) */,
	BLEXE_NO_MATCH /** could not match token */,
};

/**
 * @brief Lexer state
 *
 * Holds the state of the lexeme scanner.
 */
struct b_lex {
	struct bio *bio /** The input stream */;
	/** A character that was part of the input stream but not consumed by
	 * the previous token. (e.g., the `+` in `a+b`). */
	char peek;
	enum btk_type lookahead /** A simple one-token lookahead. */;
	/** Parentheses nesting depth. Used to ignore newlines (BTK_NEWLINE)
	 * when inside parentheses '()' or blocks '{}'. */
	b_umem group_depth;
};


/** initializes a new lexer */
void b_lex_init(struct b_lex *);

/** returns the state of the current input's stream  */
void *b_lex_clearinput(struct b_lex *);

/** returns the state of previous input */
void *b_lex_setinput(struct b_lex *, struct bio *);

/** first matched token */
enum b_lex_result b_lex_next(struct b_lex *, struct btoken *);


#endif
