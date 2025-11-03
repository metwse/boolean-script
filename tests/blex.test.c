#include "../bdef.h"
#include "../bio.h"
#include "../blex.h"

#include <stdlib.h>
#include <string.h>

#include "mock_input_stream.h"


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

void consume_tokenstream(struct b_lex *lex, const char *tokenstream)
{
	struct bio bio;

	bio_init(&bio, mock_input_stream,
		 new_mock_input_stream_state(tokenstream,
					     strlen(tokenstream)));

	b_lex_setinput(lex, &bio);

	for (b_umem i = 0; i < sizeof(tokens) / sizeof(struct b_token); i++) {
		struct b_token lexeme = b_lex_next(lex);

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

	free(b_lex_clearinput(lex));
}

void test_invalid(struct b_lex *lex, const char *invalid_tokenstream)
{
	struct bio bio;

	bio_init(&bio, mock_input_stream,
		 new_mock_input_stream_state(invalid_tokenstream,
					     strlen(invalid_tokenstream)));

	free(b_lex_setinput(lex, &bio));

	b_assert_expr(b_lex_next(lex).token == NOTOKEN,
		      "expected notoken due to invalid token")

	free(b_lex_clearinput(lex));
}

int main()
{
	struct b_lex lex;

	b_lex_init(&lex);

	const char *tokenstream =
		"a = 1;\n"
		"bool b = a'; c = a * b;\n"
		"vec<3> d = [(b * 0)', 1 * (0 + 1), (1 + 0) * 1];";

	const char *invalid_tokenstreams[] = {
		"long_identifier_aaaaaaaaaaaaaaaaa also this tests memory leak in terminated streams",
		"123457890",
		"ınvâlıd"
	};


	for (int _fuzz = 0; _fuzz < 4; _fuzz++)
		consume_tokenstream(&lex, tokenstream);


	for (b_umem i = 0; i < sizeof(invalid_tokenstreams) / sizeof(char *); i++)
		test_invalid(&lex, invalid_tokenstreams[i]);
}
