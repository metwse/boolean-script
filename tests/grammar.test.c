#include "../bdef.h"
#include "../bgrammar.h"

#include "../grammar.c"


int main()
{
	b_umem child_cap_tests[][2] = {
		{ BNT_BIT, 1 },
		{ BNT_EXPR, 2 },
		{ BNT_ATOM, 3 },
		{ BNT_ASGN, 4 },
	};
	b_umem variant_count_tests[][2] = {
		{ BNT_BIT, 2 },
		{ BNT_IDENT_OR_MEMBER, 2 },
		{ BNT_TY_BOOL, 2 },
		{ BNT_EXPR_DECL, 1 },
	};

	for (b_umem i = 0; i < sizeof(child_cap_tests) / sizeof(b_umem) / 2; i++)
		b_assert_expr(child_cap_of(child_cap_tests[i][0]) == child_cap_tests[i][1],
			      "child capacity counting failed");

	for (b_umem i = 0; i < sizeof(variant_count_tests) / sizeof(b_umem) / 2; i++)
		b_assert_expr(variant_count_of(variant_count_tests[i][0]) == variant_count_tests[i][1],
			      "child capacity counting failed");
}
