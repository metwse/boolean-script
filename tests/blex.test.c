#include "../bdef.h"
#include "../bgrammar.h"
#include "../bio.h"
#include "../blex.h"

#include <stdlib.h>
#include <string.h>

#include "mock_input_stream.h"


const struct btoken tokens[] = {
	{ .ty = BTK_IDENT, .info.ident = "long_identifier_aaaaaaaaaaaaaaaa", },
	{ .ty = BTK_ASGN, },
	{ .ty = BTK_TRUE, },
	{ .ty = BTK_STMT_DELIM, },

	// { .ty = BTK_NEWLINE, },


	{ .ty = BTK_TY_BOOL, },
	{ .ty = BTK_IDENT, .info.ident = "b", },
	{ .ty = BTK_ASGN, },
	{ .ty = BTK_IDENT, .info.ident = "a", },
	{ .ty = BTK_INVOLUTION, },
	{ .ty = BTK_STMT_DELIM, },

	{ .ty = BTK_IDENT, .info.ident = "c", },
	{ .ty = BTK_ASGN, },
	{ .ty = BTK_IDENT, .info.ident = "a", },
	{ .ty = BTK_AND, },
	{ .ty = BTK_IDENT, .info.ident = "b", },
	{ .ty = BTK_STMT_DELIM, },

	// { .ty = BTK_NEWLINE, },


	{ .ty = BTK_TY_VEC, },
	{ .ty = BTK_L_ANGLE_BRACKET, },
		{ .ty = BTK_POSITIVE_INT, .info.positive_int = 3, },
	{ .ty = BTK_R_ANGLE_BRACKET, },
	{ .ty = BTK_IDENT, .info.ident = "d", },
	{ .ty = BTK_ASGN, },
	{ .ty = BTK_L_BRACKET },
		{ .ty = BTK_L_PAREN },
			{ .ty = BTK_IDENT, .info.ident = "b", },
			{ .ty = BTK_AND, },
			{ .ty = BTK_FALSE, },
		{ .ty = BTK_R_PAREN },
		{ .ty = BTK_INVOLUTION },
	{ .ty = BTK_DELIM },
		{ .ty = BTK_TRUE },
		{ .ty = BTK_AND },
		{ .ty = BTK_L_PAREN },
			{ .ty = BTK_FALSE },
			{ .ty = BTK_OR },
			{ .ty = BTK_TRUE },
		{ .ty = BTK_R_PAREN },
	{ .ty = BTK_DELIM },
		{ .ty = BTK_L_PAREN },
			{ .ty = BTK_TRUE },
			{ .ty = BTK_OR },
			{ .ty = BTK_FALSE },
		{ .ty = BTK_R_PAREN },
		{ .ty = BTK_AND },
		{ .ty = BTK_TRUE },
	{ .ty = BTK_R_BRACKET },
	{ .ty = BTK_STMT_DELIM },

	// { .ty = BTK_NEWLINE, },


	{ .ty = BTK_TY_VEC, },
	{ .ty = BTK_L_ANGLE_BRACKET, },
		{ .ty = BTK_POSITIVE_INT, .info.positive_int = 1234567890, },
	{ .ty = BTK_R_ANGLE_BRACKET, },
	{ .ty = BTK_IDENT, .info.ident = "e", },
	{ .ty = BTK_STMT_DELIM },

	{ .ty = BTK_TY_VEC, },
	{ .ty = BTK_L_ANGLE_BRACKET, },
		{ .ty = BTK_POSITIVE_INT, .info.positive_int = 1, },
	{ .ty = BTK_R_ANGLE_BRACKET, },
	{ .ty = BTK_IDENT, .info.ident = "f", },
	{ .ty = BTK_STMT_DELIM },

	{ .ty = BTK_NOTOKEN },
};

void consume_tokenstream(struct b_lex *lex, const char *tokenstream)
{
	struct bio bio;

	bio_init(&bio, mock_input_stream,
		 new_mock_input_stream_state(tokenstream,
					     strlen(tokenstream)));

	b_lex_setinput(lex, &bio);

	for (b_umem i = 0; i < sizeof(tokens) / sizeof(struct btoken); i++) {
		struct btoken lexeme;
		b_lex_next(lex, &lexeme);

		b_assert_expr(lexeme.ty == tokens[i].ty, "lexeme error");

		if (lexeme.ty == BTK_POSITIVE_INT) {
			b_assert_expr(
				lexeme.info.positive_int == tokens[i].info.positive_int,
				"int error"
			);
		} else if (lexeme.ty == BTK_IDENT) {
			b_assert_expr(
				strcmp(lexeme.info.ident, tokens[i].info.ident) == 0,
				"ident error"
			);

			free(lexeme.info.ident);
		}
	}

	free(b_lex_clearinput(lex));
}

void test_invalid(struct b_lex *lex, const char *invalid_tokenstream,
		  enum b_lex_result expected_fail)
{
	struct bio bio;

	bio_init(&bio, mock_input_stream,
		 new_mock_input_stream_state(invalid_tokenstream,
					     strlen(invalid_tokenstream)));

	free(b_lex_setinput(lex, &bio));

	struct btoken lexeme;
	enum b_lex_result res = b_lex_next(lex, &lexeme);

	b_assert_expr(expected_fail == res,
		      "expected notoken due to invalid token")

	free(b_lex_clearinput(lex));
}

int main()
{
	struct b_lex lex;

	b_lex_init(&lex);

	const char *tokenstream =
		"long_identifier_aaaaaaaaaaaaaaaa \\\n= 1;"
		"bool b = a'\n c = a * \\\nb;"
		"vec<3> d = [(b * 0\n)', 1 * (0 + 1), (1 + 0) * 1];"
		"vec<1234567890> e\n vec<1\n> f;";

	const char *invalid_tokenstreams[] = {
		"long_identifier_aaaaaaaaaaaaaaaaa also this tests memory leak in terminated streams",
		"12345678901",
		"ınvâlıd",
		"\\not newline",
	};


	for (int _fuzz = 0; _fuzz < 4; _fuzz++)
		consume_tokenstream(&lex, tokenstream);


	test_invalid(&lex, invalid_tokenstreams[0], BLEXE_IDENT_TOO_LONG);
	test_invalid(&lex, invalid_tokenstreams[1], BLEXE_INTEGER_TOO_LARGE);
	test_invalid(&lex, invalid_tokenstreams[2], BLEXE_NO_MATCH);
	test_invalid(&lex, invalid_tokenstreams[3], BLEXE_INVALID_ESCAPE_CHAR);
}
