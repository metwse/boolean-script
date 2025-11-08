#include "bdef.h"
#include "bgrammar.h"
#include "bio.h"
#include "blex.h"
#include "bparser.h"

#include <stdlib.h>

#include "detail/bparser.h"
#include "grammar.c"


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

struct bsymbol *new_nt_node(struct bsymbol *parent,
			    enum bnt_type ty)
{
	struct bsymbol *n = malloc(sizeof(struct bsymbol));
	b_assert_expr(n, "nomem");

	n->parent = parent;
	if (parent)
		push_child(parent, n);

	n->ty = BSYMBOL_NONTERMINAL;

	b_umem child_cap = child_cap_of(ty);
	n->nt.ty = ty;
	n->nt.child_count = 0;
	b_assert_expr(child_cap, "a nonterminal with no children is not meaningful");
	b_assert_expr((
		n->nt.children =
			malloc(sizeof(struct bsymbol *) * child_cap)
	), "nomem");

	return n;
}

struct bsymbol *new_tk_node(struct bsymbol *parent,
			    enum btk_type ty)
{
	struct bsymbol *n = malloc(sizeof(struct bsymbol));
	b_assert_expr(n, "nomem");

	n->parent = parent;
	if (parent)
		push_child(parent, n);

	n->ty = BSYMBOL_TOKEN;

	n->tk.ty = ty;

	return n;
}

void push_child(struct bsymbol *parent, struct bsymbol *child)
{
	b_assert_expr(parent->ty == BSYMBOL_NONTERMINAL,
		      "nonterminal parent expected");

	b_assert_expr(parent->nt.child_count < child_cap_of(parent->nt.ty),
		      "child capacity of parent is exceeded");

	parent->nt.children[parent->nt.child_count++] = child;
}

/* teardown the tree into tokens. returns constructed the tree. */
void teardown_tree(struct bsymbol *sym, struct btoken **out, b_umem *out_len)
{
	if (sym->ty == BSYMBOL_NONTERMINAL) {
		for (b_umem i = 0; i < sym->nt.child_count; i++)
			teardown_tree(sym->nt.children[i], out, out_len);

		if (sym->nt.children)
			free(sym->nt.children);
	} else {
		if (*out == NULL) {
			*out = malloc(sizeof(struct btoken));
			*out_len = 0;
		} else {
			*out = realloc(*out,
				       sizeof(struct btoken) * (*out_len + 1));
		}
		b_assert_expr(*out, "nomem");

		(*out)[*out_len] = sym->tk;
		(*out_len)++;
	}
	free(sym);
}
