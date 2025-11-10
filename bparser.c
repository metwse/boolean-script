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
	p->tokens = NULL;
	p->cur = p->root = NULL;
	b_lex_init(&p->lex);
}

void *b_parser_clearinput(struct b_parser *p)
{
	if (p->token_count)
		free(p->tokens);
	p->token_count = 0;
	p->tokens = NULL;

	return b_lex_clearinput(&p->lex);
}

void *b_parser_setinput(struct b_parser *p, struct bio *bio)
{
	if (p->token_count)
		free(p->tokens);
	p->token_count = 0;
	p->tokens = NULL;

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
	n->nt.variant = 0;
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

void push_token(struct b_parser *p, struct btoken tk)
{
	if (p->tokens == NULL) {
		p->tokens = malloc(sizeof(struct btoken));
		p->token_count = 0;
	} else {
		p->tokens = realloc(p->tokens,
			       sizeof(struct btoken) * (p->token_count + 1));
	}
	b_assert_expr(p->tokens, "nomem");

	p->tokens[p->token_count] = tk;
	p->token_count++;
}

/* teardown the tree into tokens. returns constructed the tree. */
void teardown_tree(struct b_parser *p, struct bsymbol *sym)
{
	if (sym->ty == BSYMBOL_NONTERMINAL) {
		for (b_umem i = 0; i < sym->nt.child_count; i++)
			teardown_tree(p, sym->nt.children[i]);

		if (sym->nt.children)
			free(sym->nt.children);
	} else {
		push_token(p, sym->tk);
	}
	free(sym);
}

void teardown_children(struct b_parser *p, struct bsymbol *sym)
{
	for (b_umem i = 0; i < sym->nt.child_count; i++)
		teardown_tree(p, sym->nt.children[i]);

	sym->nt.child_count = 0;
}

struct btoken next_token(struct b_parser *p)
{
	if (p->token_count == 0) {
		struct btoken out;

		b_lex_next(&p->lex, &out);

		return out;
	} else {
		return p->tokens[--p->token_count];
	}
}

struct production *get_body(struct bsymbol *sym)
{
	return cast(struct production *,
		    productions[sym->nt.ty][sym->nt.variant]);
}

struct production get_current_rule(struct bsymbol *sym)
{
	return get_body(sym)[sym->nt.child_count];
}

int is_grammar_complete(struct bsymbol *sym)
{
	return get_current_rule(sym).end == EOB;
}

int is_construct_end(struct bsymbol *sym)
{
	return get_body(sym)[0].end == EOC;
}

struct bsymbol *left_nonterminal(struct b_parser *p, struct bsymbol *sym)
{
	struct bsymbol *parent;

	while ((parent = sym->parent)) {
		for (b_umem i = parent->nt.child_count; i > 0; i--) {
			struct bsymbol *child = parent->nt.children[i - 1];

			if (child->ty == BSYMBOL_TOKEN || child == sym) {
				parent->nt.child_count--;
				if (child->ty == BSYMBOL_TOKEN) {
					push_token(p, child->tk);
				} else {
					teardown_children(p, sym);
				}
			} else {
				break;
			}
		}

		if (parent->nt.child_count) {
			sym = parent->nt.children[parent->nt.child_count - 1];
			if (is_construct_end(sym)) {
				teardown_tree(p, sym);
				sym = parent;
			} else {
				teardown_children(p, sym);
				sym->nt.variant++;
				return sym;
			}
		} else {
			sym = parent;
		}
	}

	return NULL;
}

enum b_parser_result b_parser_try_next(struct b_parser *p, struct bsymbol *out)
{
	if (p->root == NULL) {
		p->cur = p->root = new_nt_node(NULL, BNT_STMT);
	}

	struct bsymbol *sym = p->cur;
	struct btoken tk;

	while ((tk = next_token(p)).ty != BTK_NOTOKEN) {
		while (1) {
			if (is_grammar_complete(sym)) {
				sym = sym->parent;

				if (sym == NULL) {
					push_token(p, tk);
					*out = *p->root;
					return BPARSER_READY;
				}

				continue;
			}

			if (is_construct_end(sym)) {
				sym = left_nonterminal(p, sym);

				if (sym == NULL) {
					push_token(p, tk);
					return BPARSERE_NOMATCH;
				}

				continue;
			}

			struct production rule = get_current_rule(sym);

			if (rule.ty == BSYMBOL_TOKEN) {
				if (tk.ty == rule.tk_ty) {
					new_tk_node(sym, rule.tk_ty);
					break;
				} else {
					teardown_children(p, sym);
					sym->nt.variant++;
				}
			} else if (rule.ty == BSYMBOL_NONTERMINAL) {
				sym = new_nt_node(sym, rule.nt_ty);
			}
		}
	}

	return BPARSER_READY;
}
