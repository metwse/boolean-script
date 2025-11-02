#include "bdef.h"
#include "blex.h"
#include "bio.h"

#include <stdlib.h>
#include <string.h>


void b_lex_init(struct b_lex *lex)
{
	lex->bio = NULL;
}

void *b_lex_destroy(struct b_lex *lex)
{
	if (lex->bio)
		return bio_destroy(lex->bio);

	return NULL;
}

void *b_lex_setinput(struct b_lex *lex, struct bio *bio)
{
	void *state = NULL;

	if (lex->bio)
		state = bio_destroy(lex->bio);

	lex->bio = bio;

	return state;
}

struct b_token b_lex_next(struct b_lex *lex)
{
	char token_string[33];

	// SIGNED, due to token_len--
	b_mem token_len;

	bool valid_ident = true;
	bool valid_positive_int = true;

	for (token_len = 0; token_len < 32; token_len++) {
		const char *input_ = bio_read(lex->bio, 1);

		if (!input_)
			break;

		const char input = input_[0];

		if (input == ' ' || input == '\n') {
			if (token_len == 0) {
				token_len--;
				continue;
			} else {
				break;
			}
		}

		if (token_len == 1) {
			for (int i = TK_OR; i <= TK_ASGN; i++)
				if (input == b_tokens[i][0])
					return (struct b_token) { .token = i };

			if (input == b_tokens[TK_DELIM][0])
				return (struct b_token) { .token = TK_DELIM };

			if (input == b_tokens[TK_FALSE][0])
				return (struct b_token) { .token = TK_FALSE };

			if (input == b_tokens[TK_TRUE][0])
				return (struct b_token) { .token = TK_TRUE };
		}

		token_string[token_len] = input;
		token_string[token_len + 1] = '\0';

		if (strcmp(b_tokens[TK_TY_BOOL], token_string))
			return (struct b_token) { .token = TK_TY_BOOL };

		if (strcmp(b_tokens[TK_TY_VEC], token_string))
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
	}

	// DO NOT REORDER valid_positive_int -> valid_ident check order
	//
	// valid_ident variable is true for positive integers
	if (valid_positive_int) {
		return (token_len < 19) ? (struct b_token) {
			.token = TK_POSITIVE_INT,
			.info.positive_integer = strtoull(token_string, NULL, 10)
		} : b_notok;
	} else if (valid_ident) {
		char *ident = malloc(sizeof(char) * (token_len + 1));
		memcpy(ident, token_string, token_len + 1 /* +1 for \0 */);

		return (struct b_token) {
			.token = TK_IDENT,
			.info.identifier = ident
		};
	}

	return b_notok;
}
