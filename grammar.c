/**
 * @file grammar.c
 * @brief Context-free grammar definition for the BS parser.
 *
 * This file is not a standard, independently compiled C file. It is designed
 * to be directly `#include`'d into the parser implementation `bparser.c`.
 *
 * 1. Defines the 'productions' table: A static array that represents the
 *    complete context-free grammar of the language as data.
 * 2. Provides macros (r, rrr, ropt) to define these production rules in a
 *    readable way.
 * 3. Provides 'static inline' helper functions (child_cap_of
 *    variant_count_of) that query this 'productions' table to configure the
 *    parser engine at runtime.
 */

#include "bdef.h"
#include "bgrammar.h"

#include <stdbool.h>

 /** integer representing EOB (end of body) */
#define EOB -1
 /** integer representing EOC (end of construct) */
#define EOC -2

/** maximum number of variants constructing the same nonterminal */
#define MAX_VARIANT_COUNT 7
/** maximum body length of a rule, +1 for EOB */
#define MAX_BODY_LENGTH 11


/**
 * @brief A single symbol within a production rule.
 *
 * 1. A terminal symbol (Token)
 * 2. A non-terminal symbol (Grammar rule)
 * 3. A sentinel value (EOB or EOC)
 */
struct production {
	enum bsymbol_type ty /** The symbol's type (tag for the union) */;
	union {
		enum btk_type tk_ty /** token type */;
		enum bnt_type nt_ty /** nonterminal type */;
		int end /** a sentinel value (EOB or EOC) */;
	};
};


/** @brief Sentinel struct for the end of a rule's body (EOB) */
#define SEOB (const struct production) { .end = EOB }
/** @brief Sentinel struct for the end of a construct's variants (EOC) */
#define SEOC { { .end = EOC } }

/** @brief Macro to create a terminal (Token) production symbol. */
#define TK(tk) { .ty = BSYMBOL_TOKEN, .tk_ty = BTK_ ## tk }
/** @brief Macro to create a non-terminal production symbol. */
#define NT(nt) { .ty = BSYMBOL_NONTERMINAL, .nt_ty = BNT_ ## nt }


/** @brief Macro to define a standard grammar rule. */
#define r(...) { { __VA_ARGS__ SEOB }, SEOC }

/**
 * @brief Macro to define a pair of rules for a right-recursive list.
 *
 * This is the standard pattern for lists and operator precedence. It
 * automatically defines the rule for 'head' and 'head_REST'.
 *
 * For example, `rrr(EXPR_LS, EXPR, COMMA)` defines:
 * 1. `<expr_ls> ::= <expr> <expr_ls_rest>`
 * 2. `<expr_ls_rest> ::= "," <expr_ls> <expr_ls_rest> | E`
 *
 * @param head The base non-terminal.
 * @param listelem The non-terminal for the list element.
 * @param delim The token used as a separator.
 */
#define rrr(head, listelem, delim) \
	{ { NT(listelem), NT(head ## _REST), SEOB }, SEOC }, \
	{ { TK(delim), NT(head), NT(head ## _REST), SEOB }, { SEOB }, SEOC }

/**
 * @brief Macro to define an optional grammar rule (epsilon production).
 *
 * This is a shortcut for a rule with two variants:
 * 1. The symbols provided in `__VA_ARGS__`
 * 2. An empty (epsilon) production
 */
#define ropt(...) { { __VA_ARGS__, SEOB }, { SEOB }, SEOC }

/**
 * @brief Macro for syntactic sugar to separate variants in an 'r' macro.
 *
 * Expands to the end-of-body sentinel and new variant syntax.
 */
#define or SEOB, }, {


/* The complete, context-free grammar for the language. */
static const struct production
productions[BNONTERMINAL_COUNT][MAX_VARIANT_COUNT][MAX_BODY_LENGTH] = {
// ===== primitive types ======================================================
	/* <bit> ::= */ r(
		TK(TRUE),
	or	TK(FALSE),
	),
	/* <positive_int> ::= */ r(
		TK(POSITIVE_INT),
	),

// ===== identifiers ==========================================================
	/* <ident> ::= */ r(
		TK(IDENT),
	),
	/* <ident_or_member> ::= */ r(
		NT(IDENT),
	or	NT(IDENT), TK(DOT), NT(POSITIVE_INT),
	),

// ===== types ================================================================
	/* <ty> ::= */ r(
		NT(TY_BOOL),
	or	NT(TY_VEC),
	),
	/* <ty_bool> ::= */ r(
		TK(TY_BOOL),
	or	/* empty */
	),
	/* <ty_vec> ::= */ r(
		TK(TY_VEC), TK(LANGLE_BRACKET), NT(POSITIVE_INT), TK(RANGLE_BRACKET),
	),

	/* <expr_or_initlist> ::= */ r(
		NT(INITLIST),
	or	NT(EXPR),
	),
	/* <initlist> ::= */ r(
		TK(LBRACKET), NT(EXPR_LS), TK(RBRACKET),
	),

// ===== expressions ==========================================================
	/* <expr> ::= */
		rrr(EXPR, TERM, OR),
	/* <term> ::= */
		rrr(TERM, FACTOR, AND),
	/* <factor> ::= */ r(
		NT(ATOM), NT(OPTINVOLUTION),
	),
	/* <atom> ::= */ r(
		TK(LPAREN), NT(EXPR), TK(RPAREN),
	or	TK(LPAREN), NT(ASGN), TK(RPAREN),
	or	NT(CALL),
	or	NT(IDENT_OR_MEMBER),
	or	NT(BIT),
	),
	/* <optinvolution> ::= */ ropt(TK(INVOLUTION)),
	/* <call> ::= */ r(
		NT(IDENT), TK(LPAREN), NT(CALL_OPTPARAMS), TK(RPAREN),
	),
	/* <call_params> ::= */
		rrr(CALL_PARAMS, EXPR_OR_INITLIST, COMMA),
	/* <call_optparams> ::= */
		ropt(NT(CALL_PARAMS)),

// ===== statements ===========================================================
	/* <stmt> ::= */ r(
		TK(SEMI),
	or	NT(VAR_DECL), TK(SEMI),
	or	NT(EXPR_DECL), TK(SEMI),
	or	NT(ASGN), TK(SEMI),
	or	NT(CALL), TK(SEMI),
	or	TK(LCURLY), NT(STMTS), TK(RCURLY),
	),
	/* <stmts> ::= */ ropt(NT(STMT), NT(STMTS)),

// ===== variable or expression declarations ==================================
	/* <var_decl> ::= */ r(
		NT(TY), NT(IDENT_LS), NT(VAR_DECL_OPTASGN),
	),
	/* <var_decl_optasgn> ::= */ r(
		TK(EQ), NT(ASGN),
	or	TK(EQ), NT(EXPR_OR_INITLIST_LS),
	or	/* empty */
	),
	/* <expr_decl> ::= */ r(
		// "expr" <ident> "(" <expr_decl_optparams> ")"
		TK(EXPR), NT(IDENT), TK(LPAREN), NT(EXPR_DECL_OPTPARAMS), TK(RPAREN),
			// "->" "(" <expr_decl_optparams> ")"
			TK(RARROW), TK(LPAREN), NT(EXPR_DECL_OPTPARAMS), TK(RPAREN),
			NT(STMT),
	),
	/* <expr_decl_param> ::= */ r(
		NT(TY), NT(IDENT),
	),
	/* <expr_decl_params> ::= */
		rrr(EXPR_DECL_PARAMS, EXPR_DECL_PARAM, COMMA),
	/* <expr_decl_optparams> ::= */
		ropt(NT(EXPR_DECL_PARAMS)),

// ===== right-recursive list types ===========================================
	/* <asgn> ::= */ r(
		NT(IDENT_OR_MEMBER_LS), NT(ASGN_REST), TK(EQ), NT(EXPR_OR_INITLIST_LS),
	),
	/* <asgn_rest> ::= */
		ropt(TK(EQ), NT(IDENT_OR_MEMBER_LS), NT(ASGN_REST)),
	/* <ident_ls> ::= */
		rrr(IDENT_LS, IDENT, COMMA),
	/* <ident_or_member_ls> ::= */
		rrr(IDENT_OR_MEMBER_LS, IDENT_OR_MEMBER, COMMA),
	/* <expr_ls> ::= */
		rrr(EXPR_LS, EXPR, COMMA),
	/* <expr_or_initlist_ls> ::= */
		rrr(EXPR_OR_INITLIST_LS, EXPR_OR_INITLIST, COMMA),
};


/* no lock/once used lazy initialization for widest compability */
static inline b_umem b_unused_fn(child_cap_of)(enum bnt_type nt)
{
	static b_umem child_caps[BNONTERMINAL_COUNT];
	bool initialized = false;

	if (!initialized) {
		for (b_umem i = 0; i < BNONTERMINAL_COUNT; i++) {
			b_umem len;
			child_caps[i] = 0;

			for (b_umem j = 0; j < MAX_VARIANT_COUNT; j++) {
				if (productions[i][j][0].end == EOC)
					break;

				for (len = 0; len < MAX_BODY_LENGTH; len++)
					if (productions[i][j][len].end == EOB)
						break;

				if (len > child_caps[i])
					child_caps[i] = len;
			}
		}

		initialized = true;
	}

	return child_caps[nt];
}

/* no lock/once used lazy initialization for widest compability */
static inline b_umem b_unused_fn(variant_count_of)(enum bnt_type nt)
{
	static b_umem variant_counts[BNONTERMINAL_COUNT];
	bool initialized = false;

	if (!initialized) {
		for (b_umem i = 0; i < BNONTERMINAL_COUNT; i++) {
			b_umem count = 0;

			for (b_umem j = 0;
			     productions[i][j][0].end != EOC && j < MAX_VARIANT_COUNT;
			     j++)
				count++;

			variant_counts[i] = count;
		}

		initialized = true;
	}

	return variant_counts[nt];
}

#undef SEOB
#undef SEOC

#undef TK
#undef NT

#undef r
#undef rrr
#undef ropt

#undef or
