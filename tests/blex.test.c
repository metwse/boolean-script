#include "../bio.h"
#include "../blex.h"

#include <stdlib.h>
#include <string.h>

#include "mock_input_stream.h"


const char *tokenstream =
	"a = 1;\n"
	"bool b = a'; c = a * b;\n"
	"vec<3> d = [(b * 0)', 1 * (0 + 1), (1 + 0) * 1];";

const struct b_token tokens[] = {
	{ .token = TK_IDENT, .info.identifier = "a", },
	{ .token = TK_ASGN, },
	{ .token = TK_TRUE, },
	{ .token = TK_STMT_DELIM, },

	{ .token = TK_TY_BOOL, },
	{ .token = TK_IDENT, .info.identifier = "b", },
	{ .token = TK_ASGN, },
	{ .token = TK_IDENT, .info.identifier = "a", },
	{ .token = TK_INVOLUTION, },
	{ .token = TK_STMT_DELIM, },

	{ .token = TK_IDENT, .info.identifier = "c", },
	{ .token = TK_ASGN, },
	{ .token = TK_IDENT, .info.identifier = "a", },
	{ .token = TK_AND, },
	{ .token = TK_IDENT, .info.identifier = "b", },
	{ .token = TK_STMT_DELIM, },

	{ .token = TK_TY_VEC, },
	{ .token = TK_L_ANGLE_BRACKET, },
		{ .token = TK_POSITIVE_INT, .info.positive_integer = 3, },
	{ .token = TK_R_ANGLE_BRACKET, },
	{ .token = TK_IDENT, .info.identifier = "d", },
	{ .token = TK_ASGN, },
	{ .token = TK_L_BRACKET },
		{ .token = TK_L_PAREN },
			{ .token = TK_IDENT, .info.identifier = "b", },
			{ .token = TK_AND, },
			{ .token = TK_FALSE, },
		{ .token = TK_R_PAREN },
		{ .token = TK_INVOLUTION },
	{ .token = TK_DELIM },
		{ .token = TK_TRUE },
		{ .token = TK_AND },
		{ .token = TK_L_PAREN },
			{ .token = TK_FALSE },
			{ .token = TK_OR },
			{ .token = TK_TRUE },
		{ .token = TK_R_PAREN },
	{ .token = TK_DELIM },
		{ .token = TK_L_PAREN },
			{ .token = TK_TRUE },
			{ .token = TK_OR },
			{ .token = TK_FALSE },
		{ .token = TK_R_PAREN },
		{ .token = TK_AND },
		{ .token = TK_TRUE },
	{ .token = TK_R_BRACKET },
	{ .token = TK_STMT_DELIM },

	{ .token = NOTOKEN },
};

int main()
{
	struct b_lex lex;
	struct bio bio;

	b_lex_init(&lex);

	bio_init(&bio, mock_input_stream,
		 new_mock_input_stream_state(tokenstream,
					     strlen(tokenstream)));

	b_lex_setinput(&lex, &bio);

	for (b_umem i = 0; i < sizeof(tokens) / sizeof(struct b_token); i++) {
		struct b_token lexeme = b_lex_next(&lex);

		b_assert_expr(lexeme.token == tokens[i].token, "lexeme error");

		if (lexeme.token == TK_POSITIVE_INT) {
			b_assert_expr(
				lexeme.info.positive_integer == tokens[i].info.positive_integer,
				"int parse error"
			);
		} else if (lexeme.token == TK_IDENT) {
			b_assert_expr(
				strcmp(lexeme.info.identifier, tokens[i].info.identifier) == 0,
				"int parse error"
			);

			free(lexeme.info.identifier);
		}
	}

	free(bio_destroy(&bio));
}
