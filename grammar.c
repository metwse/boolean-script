#include "bgrammar.h"


#define TK(tk) { .ty = BSYMBOL_TOKEN, .tk_ty = BTK_ ## tk }
#define NT(nt) { .ty = BSYMBOL_NONTERMINAL, .nt_ty = BNT_ ## nt }

#define EOB { 0 } /* end of body */
#define EOC { 0 } /* end of construct */


__attribute__((unused)) static const struct {
	enum bsymbol_type ty;
	union {
		enum btk_type tk_ty;
		enum bnt_type nt_ty;
	};
} constructs[][10][5] = {
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
		EOB
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
