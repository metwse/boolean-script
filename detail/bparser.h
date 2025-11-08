/**
 * Private methods of the parser.
 */

#ifndef BPARSER_DETAIL_H
#define BPARSER_DETAIL_H

#include "../bdef.h"
#include "../bgrammar.h"


struct bsymbol *new_nt_node(struct bsymbol *parent, enum bnt_type ty);

struct bsymbol *new_tk_node(struct bsymbol *parent, enum btk_type ty);

void push_child(struct bsymbol *parent, struct bsymbol *child);

void teardown_tree(struct bsymbol *sym, struct btoken **out, b_umem *out_len);


#endif
