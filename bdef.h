/**
 * @file bdef.h
 * @brief Basic definitions.
 *
 * Limits, basic types, and some other definitions
 */

#ifndef BDEF_H
#define BDEF_H

#include <stddef.h>


/** maximum of two nums */
#define b_max(a, b) ((a) > (b) ? (a) : (b))

/** type casts (a macro highlights casts in the code) */
#define cast(t, exp)	((t) (exp))


#ifdef B_ASSERT
#include <assert.h>
#include <stdio.h>  // IWYU pragma: export

#define bI_assert_stringify_detail(a) #a
#define bI_assert_stringify(a) bI_assert_stringify_detail(a)

#define b_assert_expr(c, fmt, ...) \
	if (!(c)) { \
		fprintf(stderr, "["__FILE__ ":" bI_assert_stringify(__LINE__) "] " fmt "\n"\
			__VA_OPT__(,)__VA_ARGS__); \
		assert(c); \
	}
#define b_assert(c) c
#define b_unreachable(c) fprintf(stderr, "unreachable"); assert(0);
#else
/** @brief internal assertions for in-house debugging */
#define b_assert_expr(c, ...) ((void) 0)
/** @brief assertion code that can be disabled with a flag */
#define b_assert(c) ((void) 0)
/** @brief code get into an unreachable branch */
#define b_unreachable(c) ((void) 0)
#endif

/** for suppressing unused function wargnings */
#ifdef __GNUC__
#define b_unused_fn(x) __attribute__((__unused__)) x
#else
#define b_unused_fn(x) x
#endif


/**
 * @brief 'b_mem' is a signed integer big enough to count the total memory used
 * by BS.
 */
typedef unsigned long long b_umem;
/**
 * @brief 'b_umem' is the corresponding unsigned type to `b_mem`. Usually,
 * 'ptrdiff_t' should work, but we use 'long long' for 32-bit machines.
 */
typedef long long b_mem;

/**
 * @brief chars used as small naturals (so that 'char' is reserved for
 * characters)
 */
typedef unsigned char b_ubyte;
/** @brief 'b_ubyte' is the corresponding unsigned type to `b_byte`. */
typedef char b_byte;


#endif
