#include "bdef.h"
#include "bgrammar.h"
#include "bio.h"
#include "blex.h"
#include "bparser.h"

#include <stdlib.h>


/*
 * maximum child capacity of nonterminals
 * see B_NONTERMINAL
 */
static const b_umem child_caps[] = {
	0 /* EPSILON */,

	1 /* BIT */, 1 /* POSITIVE_INT */,
	1 /* IDEN */, 3 /* IDENT_OR_MEMBER */,

	4 /* TY */,

	/* right-recursive expressions */
	2 /* EXPR */, 3 /* EXPR_REST */,
	2 /* TERM */, 3 /* TERM_OR_MEMBER */,
	2 /* FACTOR */, 3 /* ATOM */,
	/* function calls */
	4 /* CALL */, 1 /* OPTPARAMS */,

	/* statements */
	3 /* STMT */, 2 /* STMTS */,
	3 /* DECL */, 2 /* DECL_OPTASGNS */,

	/* right-recursive list types */
	4 /* ASGN */, 3 /* ASGNS_REST */,

	2 /* IDENT_LS */, 3 /* IDENT_LS_REST */,
	2 /* IDENT_OR_MEMBER */, 3 /* IDENT_OR_MEMBER_LS_REST */,

	2 /* EXPR_REST */, 3 /* EXPR_LS_REST */,
};

void b_parser_init(struct b_parser *p)
{
	p->token_count = 0;
	b_lex_init(&p->lex);
}

void *b_parser_clearinput(struct b_parser *p)
{
	if (p->token_count)
		free(p->tokens);
	p->token_count = 0;

	return b_lex_clearinput(&p->lex);
}

void *b_parser_setinput(struct b_parser *p, struct bio *bio)
{
	if (p->token_count)
		free(p->tokens);
	p->token_count = 0;

	return b_lex_setinput(&p->lex, bio);
}

struct b_ptree_node *new_construct_node(struct b_ptree_node *parent,
					enum bnt_type ty)
{
	struct b_ptree_node *n = malloc(sizeof(struct b_ptree_node));
	b_assert_expr(n, "nomem");

	n->parent = parent;
	n->symbol.ty = BSYMBOL_NONTERMINAL;

	b_umem child_cap = child_caps[ty];
	n->symbol.nt.ty = ty;
	n->symbol.nt.child_count = 0;
	if (child_cap)
		b_assert_expr((
			n->symbol.nt.children =
				malloc(sizeof(struct b_ptree_node) * child_cap)
		), "nomem");

	return n;
}

struct b_ptree_node *new_token_node(struct b_ptree_node *parent,
				    enum btk_type ty)
{
	struct b_ptree_node *n = malloc(sizeof(struct b_ptree_node));
	b_assert_expr(n, "nomem");

	n->parent = parent;
	n->symbol.ty = BSYMBOL_TOKEN;

	n->symbol.tk.ty = ty;

	return n;
}

/* teardown the tree into tokens. returns constructed the tree. */
void teardown_tree(struct bsymbol *sym, struct btoken **out, b_umem *out_len)
{
	if (sym->ty == BSYMBOL_NONTERMINAL) {
		for (b_umem i = 0; i < sym->nt.child_count; i++)
			teardown_tree(&sym->nt.children[i], out, out_len);
	} else {
		if (*out == NULL)
			*out = malloc(sizeof(struct btoken));
		else
			*out = realloc(*out,
				       sizeof(struct btoken) * (*out_len + 1));
		b_assert_expr(*out, "nomem");

		(*out)[*out_len] = sym->tk;
		(*out_len)++;
	}
}
