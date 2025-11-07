#include "bdef.h"
#include "bgrammar.h"

#include <stdbool.h>


#define TK(tk) { .ty = BSYMBOL_TOKEN, .tk_ty = BTK_ ## tk }
#define NT(nt) { .ty = BSYMBOL_NONTERMINAL, .nt_ty = BNT_ ## nt }

#define IEOB -1 /* integer representing EOB */
#define IEOC -2 /* integer representing EOC */

#define EOB (const struct production) { .end = IEOB } /* end of body */
#define EOC { { .end = IEOC } } /* end of construct */

// maximum number of variants constructing the same nonterminal
#define MAX_VARIANT_COUNT 26
// maximum body length of a rule
#define MAX_BODY_LENGTH 4

/** nonterminal rules */
struct production {
	enum bsymbol_type ty /** whether the symbol is a terminal or not */;
	union {
		enum btk_type tk_ty;
		enum bnt_type nt_ty;
		int end;
	};
};


static const struct production
productions[BNONTERMINAL_COUNT][MAX_VARIANT_COUNT][MAX_BODY_LENGTH + 1 /* +1 for EOB */] = {
	/* BIT */ {
		{ TK(TRUE), EOB },
		{ TK(FALSE), EOB },
		EOC
	},
	/* POSITIVE_INT */ {
		{ TK(POSITIVE_INT), EOB },
		EOC
	},
	/* IDENT */ {
		{ TK(IDENT), EOB },
		EOC
	},
	/* IDENT_OR_MEMBER */ {
		{ NT(IDENT), EOB },
		{ NT(IDENT), TK(SUBSCRIPT), NT(POSITIVE_INT), EOB },
		EOC
	},
	/* TY */ {
		{ TK(TY_BOOL), EOB },
		{ TK(TY_VEC), TK(L_ANGLE_BRACKET), NT(POSITIVE_INT), TK(R_ANGLE_BRACKET), EOB },
		EOC
	},

	/* EXPR */ {
		{ NT(TERM), NT(EXPR_REST), EOB },
		EOC
	},
	/* EXPR_REST */ {
		{ TK(OR), NT(EXPR), NT(EXPR_REST), EOB },
		{ EOB },
		EOC
	},

	/* TERM */ {
		{ NT(FACTOR), NT(TERM_REST), EOB },
		EOC
	},
	/* TERM_REST */ {
		{ TK(AND), NT(TERM), NT(TERM_REST), EOB },
		{ EOB },
		EOC
	},

	/* FACTOR */ {
		{ NT(ATOM), NT(OPTINVOLUTION), EOB },
		EOC
	},
	/* BNT_OPTINVOLUTION */ {
		{ TK(INVOLUTION), EOB },
		{ EOB },
		EOC
	},
	/* ATOM */ {
		{ TK(L_PAREN), NT(EXPR), TK(R_PAREN), EOB },
		{ TK(L_BRACKET), NT(EXPR_LS), TK(R_BRACKET), EOB },
		{ TK(L_PAREN), NT(ASGN), TK(R_PAREN), EOB },
		{ NT(CALL), EOB },
		{ NT(IDENT_OR_MEMBER), EOB },
		{ NT(BIT), EOB },
		EOC
	},
	/* CALL */ {
		{ NT(IDENT), TK(L_PAREN), NT(OPTPARAMS), TK(L_PAREN), EOB },
		EOC
	},
	/* OPTPARAMS */ {
		{ NT(EXPR_LS), EOB },
		{ EOB },
		EOC
	},

	/* STMT */ {
		{ NT(DECL), TK(STMT_DELIM), EOB },
		{ NT(ASGN), TK(STMT_DELIM), EOB },
		{ NT(EXPR), TK(STMT_DELIM), EOB },
		{ TK(L_CURLY), NT(STMTS), TK(R_CURLY), EOB },
		EOC
	},
	/* STMTS */ {
		{ NT(STMT), NT(STMTS), EOB },
		{ EOB },
		EOC
	},

	/* DECL */ {
		{ NT(TY), NT(IDENT_LS), NT(DECL_OPTASGN), EOB },
		{ NT(IDENT_LS), NT(DECL_OPTASGN), EOB },
		EOC
	},
	/* DECL_OPTASGN */ {
		{ TK(ASGN), NT(ASGN), EOB },
		{ TK(ASGN), NT(EXPR), EOB },
		{ EOB },
		EOC
	},

	/* ASGN */ {
		{ NT(IDENT_OR_MEMBER_LS), NT(ASGN_REST), TK(ASGN), NT(EXPR_LS), EOB },
		EOC
	},
	/* ASGN_REST */ {
		{ TK(ASGN), NT(IDENT_OR_MEMBER_LS), NT(ASGN_REST), EOB },
		{ EOB },
		EOC
	},

	/* IDENT_LS */ {
		{ NT(IDENT), NT(IDENT_LS_REST), EOB },
		EOC
	},
	/* IDENT_LS_REST */ {
		{ TK(DELIM), NT(IDENT), NT(IDENT_LS_REST), EOB },
		{ EOB },
		EOC
	},

	/* IDENT_OR_MEMBER_LS */ {
		{ NT(IDENT_OR_MEMBER), NT(IDENT_OR_MEMBER_LS_REST), EOB },
		EOC
	},
	/* IDENT_OR_MEMBER_LS_REST */ {
		{ TK(DELIM), NT(IDENT_OR_MEMBER), NT(IDENT_OR_MEMBER_LS_REST), EOB },
		{ EOB },
		EOC
	},

	/* EXPR_LS */ {
		{ NT(EXPR), NT(EXPR_LS_REST), EOB },
		EOC
	},
	/* EXPR_LS_REST */ {
		{ TK(DELIM), NT(EXPR), NT(EXPR_LS_REST), EOB },
		{ EOB },
		EOC
	},
};

/* no lock/once used lazy initialization for widest compability */
__attribute__((unused)) static inline b_umem child_cap_of(enum bnt_type nt)
{
	static b_umem child_caps[BNONTERMINAL_COUNT];
	bool initialized = false;

	if (!initialized) {
		for (b_umem i = 0; i < BNONTERMINAL_COUNT; i++) {
			b_umem len;
			child_caps[i] = 0;

			for (b_umem j = 0; j < MAX_VARIANT_COUNT; j++) {
				if (productions[i][j][0].end == IEOC)
					break;

				for (len = 0; len < MAX_BODY_LENGTH; len++)
					if (productions[i][j][len].end == IEOB)
						break;

				if (len > child_caps[i])
					child_caps[i] = len;
			}
		}

		initialized = true;
	}

	return child_caps[nt];
}
