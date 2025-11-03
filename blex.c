#include "bdef.h"
#include "blex.h"
#include "bio.h"

#include <stdlib.h>
#include <string.h>


void b_lex_init(struct b_lex *lex)
{
	lex->bio = NULL;
	lex->hold_char = 0;
}

void *b_lex_clearinput(struct b_lex *lex)
{
	void *state = NULL;

	lex->hold_char = 0;
	if (lex->bio)
		state = bio_destroy(lex->bio);

	lex->bio = NULL;

	return state;
}

void *b_lex_setinput(struct b_lex *lex, struct bio *bio)
{
	void *state = b_lex_clearinput(lex);
	lex->bio = bio;

	return state;
}

struct b_token b_lex_next(struct b_lex *lex)
{
	char token_string[33];

	// SIGNED, due to token_len--
	b_mem token_len;

	bool valid_ident = true, valid_positive_int = true;

	for (token_len = 0;
	     token_len <= 32 /* 32 for max length matching */;
	     token_len++) {
		char input;

		if (lex->hold_char) {
			input = lex->hold_char;
			lex->hold_char = 0;
		} else {
			const char *input_ = bio_read(lex->bio, 1);

			if (!input_)
				break;

			input = input_[0];
		}

		if (input == ' ' || input == '\n') {
			if (token_len == 0) {
				token_len--;
				continue;
			} else {
				break;
			}
		} else {
			// exceeded max identifier length
			//
			// TODO: syntax error
			if (token_len == 32) {
				valid_ident = false;
				break;
			}
		}

		for (int i = TK_OR; i <= TK_STMT_DELIM; i++)
			if (input == b_tokens[i][0]) {
				if (token_len == 0) {
					return (struct b_token) { .token = i };
				} else {
					// One-character "punctuation" can
					// break an identifier or integer
					// read. Hold the character for the
					// next lexeme.
					lex->hold_char = input;
					break;
				}
			}

		// transfer break from inner loop TK_OR -> TK_STMT_DELIM to
		// outer for loop.
		if (lex->hold_char)
			break;

		token_string[token_len] = input;
		token_string[token_len + 1] = '\0';

		if (strcmp(b_tokens[TK_TY_BOOL], token_string) == 0)
			return (struct b_token) { .token = TK_TY_BOOL };

		if (strcmp(b_tokens[TK_TY_VEC], token_string) == 0)
			return (struct b_token) { .token = TK_TY_VEC };

		if (!('0' <= input && input <= '9'))
			valid_positive_int = false;

		if (!(
			('0' <= input && input <= '9') ||
			('a' <= input && input <= 'z') ||
			('A' <= input && input <= 'Z') ||
			input == '_'
		))
			valid_ident = false;

		if (!valid_ident && !valid_positive_int) {
			lex->hold_char = input;
			break;
		}
	}

	// DO NOT REORDER valid_positive_int -> valid_ident check order
	//
	// valid_ident variable is true for positive integers
	if (token_len && valid_positive_int) {
		if (token_len == 1) {
			if (token_string[0] == b_tokens[TK_FALSE][0])
				return (struct b_token) { .token = TK_FALSE };

			if (token_string[0] == b_tokens[TK_TRUE][0])
				return (struct b_token) { .token = TK_TRUE };
		}

		// exceeded max positive integer
		//
		// TODO: syntax error
		return (token_len < 10) ?(struct b_token) {
			.token = TK_POSITIVE_INT,
			.info.positive_integer = strtoull(token_string, NULL, 10)
		} : b_notok;
	} else if (token_len && valid_ident) {
		char *ident = malloc(sizeof(char) * (token_len + 1));
		memcpy(ident, token_string, token_len + 1 /* +1 for \0 */);

		return (struct b_token) {
			.token = TK_IDENT,
			.info.identifier = ident
		};
	}

	return b_notok;
}
