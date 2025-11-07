#include "../bdef.h"
#include "../bgrammar.h"

#include "../grammar.c"


int main()
{
	b_umem tests[][2] = {
		{ BNT_BIT, 1 },
		{ BNT_EXPR, 2 },
		{ BNT_ATOM, 3 },
		{ BNT_ASGN, 4 },
	};

	for (b_umem i = 0; i < sizeof(tests) / sizeof(b_umem) / 2; i++) {
		b_assert_expr(child_cap_of(tests[i][0]) == tests[i][1],
			      "child capacity counting failed");
	}
}
