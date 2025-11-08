#include "../../bgrammar.h"


#define TK(tk) { .ty = BTK_ ## tk, },
#define TK_DETAIL(tk, inf, value) { .ty = BTK_ ## tk, .info.inf = value },


struct test_case {
	const char *str;
	const struct btoken tk[100];
};


const struct test_case test_cases[] = {
{
	.str = "expr fn(a, bool b, vec<1> c) -> (d, bool e) {\n"
		"g = a * b + c\n"
		"return 0\n"
		"}",
	.tk = {
		TK(EXPR)
		TK_DETAIL(IDENT, ident, "fn")
		TK(L_PAREN)
		TK_DETAIL(IDENT, ident, "a")
		TK(COMMA)
		TK(TY_BOOL)
		TK_DETAIL(IDENT, ident, "b")
		TK(COMMA)
		TK(TY_VEC)
		TK(L_ANGLE_BRACKET)
		TK_DETAIL(POSITIVE_INT, positive_int, 1)
		TK(R_ANGLE_BRACKET)
		TK_DETAIL(IDENT, ident, "c")
		TK(R_PAREN)
		TK(RARROW)
		TK(L_PAREN)
		TK_DETAIL(IDENT, ident, "b")
		TK(COMMA)
		TK(TY_BOOL)
		TK_DETAIL(IDENT, ident, "e")
		TK(R_PAREN)
		TK(L_CURLY)
		TK(SEMI)

		TK_DETAIL(IDENT, ident, "g")
		TK(EQ)
		TK_DETAIL(IDENT, ident, "a")
		TK(AND)
		TK_DETAIL(IDENT, ident, "b")
		TK(OR)
		TK_DETAIL(IDENT, ident, "c")
		TK(SEMI)

		TK(RETURN)
		TK(FALSE)
		TK(SEMI)

		TK(R_CURLY)
	},
},
{
	.str = "bool_long_identifier_aaaaaaaaaaa \\\n"
		"= 1;",
	.tk = {
		TK_DETAIL(IDENT, ident, "bool_long_identifier_aaaaaaaaaaa")
		TK(EQ)
		TK(TRUE)
		TK(SEMI)
	},
},
{
	.str = "bool b = a'\n"
		" c = a * \\\n"
		"b;",
	.tk = {
		TK(TY_BOOL)
		TK_DETAIL(IDENT, ident, "b")
		TK(EQ)
		TK_DETAIL(IDENT, ident, "a")
		TK(INVOLUTION)
		TK(SEMI)

		TK_DETAIL(IDENT, ident, "c")
		TK(EQ)
		TK_DETAIL(IDENT, ident, "a")
		TK(AND)
		TK_DETAIL(IDENT, ident, "b")
		TK(SEMI)
	},
},
{
	.str = "vec<3> d = [(b * 0\n"
		")', 1 * (0 + 1), (1 + 0) * 1];",
	.tk = {
		TK(TY_VEC)
		TK(L_ANGLE_BRACKET)
			TK_DETAIL(POSITIVE_INT, positive_int, 3)
		TK(R_ANGLE_BRACKET)
		TK_DETAIL(IDENT, ident, "d")
		TK(EQ)
		TK(L_BRACKET)
			TK(L_PAREN)
				TK_DETAIL(IDENT, ident, "b")
				TK(AND)
				TK(FALSE)
			TK(R_PAREN)
			TK(INVOLUTION)
		TK(COMMA)
			TK(TRUE)
			TK(AND)
			TK(L_PAREN)
				TK(FALSE)
				TK(OR)
				TK(TRUE)
			TK(R_PAREN)
		TK(COMMA)
			TK(L_PAREN)
				TK(TRUE)
				TK(OR)
				TK(FALSE)
			TK(R_PAREN)
			TK(AND)
			TK(TRUE)
		TK(R_BRACKET)
		TK(SEMI)
	},
},
{
	.str = "vec<1234567890> e\n"
		" vec<1\n"
		"> f;",
	.tk = {
		TK(TY_VEC)
		TK(L_ANGLE_BRACKET)
			TK_DETAIL(POSITIVE_INT, positive_int, 1234567890)
		TK(R_ANGLE_BRACKET)
		TK_DETAIL(IDENT, ident, "e")
		TK(SEMI)

		TK(TY_VEC)
		TK(L_ANGLE_BRACKET)
			TK_DETAIL(POSITIVE_INT, positive_int, 1)
		TK(R_ANGLE_BRACKET)
		TK_DETAIL(IDENT, ident, "f")
		TK(SEMI)
	},
}
};
