#ifndef TESTUTIL_H
#define TESTUTIL_H


#define TK(tk) { .ty = BTK_ ## tk, }
#define TK_DETAIL(tk, inf, value) { .ty = BTK_ ## tk, .info.inf = value }


#endif
