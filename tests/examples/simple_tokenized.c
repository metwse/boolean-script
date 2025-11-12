#include "../../bgrammar.h"

#include "../testutil.h"


/** test case @internal */
struct test_case {
	/** raw string */
	const char *str;
	/** tokenized */
	const struct btoken tk[100];
};


const struct test_case test_cases[] = {
{
	.str = "expr fn(a, bool b, vec<1> c) -> (d, bool e) {\n"
		"g = a * b + c\n"
		"return 0\n"
		"}",
	.tk = {
		TK(EXPR),
		TK_DETAIL(IDENT, ident, "fn"),
		TK(LPAREN),
		TK_DETAIL(IDENT, ident, "a"),
		TK(COMMA),
		TK(TY_BOOL),
		TK_DETAIL(IDENT, ident, "b"),
		TK(COMMA),
		TK(TY_VEC),
		TK(LANGLE_BRACKET),
		TK_DETAIL(POSITIVE_INT, positive_int, 1),
		TK(RANGLE_BRACKET),
		TK_DETAIL(IDENT, ident, "c"),
		TK(RPAREN),
		TK(RARROW),
		TK(LPAREN),
		TK_DETAIL(IDENT, ident, "d"),
		TK(COMMA),
		TK(TY_BOOL),
		TK_DETAIL(IDENT, ident, "e"),
		TK(RPAREN),
		TK(LCURLY),
		TK(SEMI),

		TK_DETAIL(IDENT, ident, "g"),
		TK(EQ),
		TK_DETAIL(IDENT, ident, "a"),
		TK(AND),
		TK_DETAIL(IDENT, ident, "b"),
		TK(OR),
		TK_DETAIL(IDENT, ident, "c"),
		TK(SEMI),

		TK(RETURN),
		TK(FALSE),
		TK(SEMI),

		TK(RCURLY),
	},
},
{
	.str = "bool_long_identifier_aaaaaaaaaaa \\\n"
		"= 1;",
	.tk = {
		TK_DETAIL(IDENT, ident, "bool_long_identifier_aaaaaaaaaaa"),
		TK(EQ),
		TK(TRUE),
		TK(SEMI),
	},
},
{
	.str = "bool b = a'\n"
		" c = a * \\\n"
		"b;",
	.tk = {
		TK(TY_BOOL),
		TK_DETAIL(IDENT, ident, "b"),
		TK(EQ),
		TK_DETAIL(IDENT, ident, "a"),
		TK(INVOLUTION),
		TK(SEMI),

		TK_DETAIL(IDENT, ident, "c"),
		TK(EQ),
		TK_DETAIL(IDENT, ident, "a"),
		TK(AND),
		TK_DETAIL(IDENT, ident, "b"),
		TK(SEMI),
	},
},
{
	.str = "vec<3> d = [(b * 0\n"
		")', 1 * (0 + 1), (1 + 0) * 1];",
	.tk = {
		TK(TY_VEC),
		TK(LANGLE_BRACKET),
			TK_DETAIL(POSITIVE_INT, positive_int, 3),
		TK(RANGLE_BRACKET),
		TK_DETAIL(IDENT, ident, "d"),
		TK(EQ),
		TK(LBRACKET),
			TK(LPAREN),
				TK_DETAIL(IDENT, ident, "b"),
				TK(AND),
				TK(FALSE),
			TK(RPAREN),
			TK(INVOLUTION),
		TK(COMMA),
			TK(TRUE),
			TK(AND),
			TK(LPAREN),
				TK(FALSE),
				TK(OR),
				TK(TRUE),
			TK(RPAREN),
		TK(COMMA),
			TK(LPAREN),
				TK(TRUE),
				TK(OR),
				TK(FALSE),
			TK(RPAREN),
			TK(AND),
			TK(TRUE),
		TK(RBRACKET),
		TK(SEMI),
	},
},
{
	.str = "vec<1234567890> e\n"
		" vec<1\n"
		"> f;",
	.tk = {
		TK(TY_VEC),
		TK(LANGLE_BRACKET),
			TK_DETAIL(POSITIVE_INT, positive_int, 1234567890),
		TK(RANGLE_BRACKET),
		TK_DETAIL(IDENT, ident, "e"),
		TK(SEMI),

		TK(TY_VEC),
		TK(LANGLE_BRACKET),
			TK_DETAIL(POSITIVE_INT, positive_int, 1),
		TK(RANGLE_BRACKET),
		TK_DETAIL(IDENT, ident, "f"),
		TK(SEMI),
	},
}
};
