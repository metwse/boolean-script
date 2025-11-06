/**
 * @file bmem.h
 * @brief Memory management interface
 *
 * Safe memory allocation wrappers.
 */

#ifndef BMEM_H
#define BMEM_H

#include "bdef.h"


/** @brief zero-cost buffer wrapper */
struct b_buffer {
	b_ubyte *b /** underlying buffer */;
	b_umem cap /** buffer capacity */;
};


/** @brief initializes a empty buffer */
void b_buffer_init(struct b_buffer *);

/** @brief initializes a buffer with given cap */
void b_buffer_with_cap(struct b_buffer *, b_umem cap);

/** @brief sets buffer capacity to 0 */
void b_buffer_reset(struct b_buffer *);

/** @brief resizes underlying buffer */
void b_buffer_resize(struct b_buffer *, b_umem cap);

/** @brief element at, with boundary checks */
static inline b_ubyte b_buffer_at(struct b_buffer buf, b_umem i)
{
	b_assert_expr(i < buf.cap, "buffer out of boundary");

	return buf.b[i];
}

/** @brief element at (casting to char), with boundary checks */
static inline char b_buffer_char_at(struct b_buffer buf, b_umem i)
{
	return cast(char, b_buffer_at(buf, i));
}

/** @brief reference to element at, with boundary checks */
static inline void b_buffer_set(struct b_buffer buf, b_umem i, b_ubyte b)
{
	b_assert_expr(i < buf.cap, "buffer out of boundary");

	buf.b[i] = b;
}

/** @brief sets all elements to given byte */
static inline void b_buffer_setall(struct b_buffer buf, b_ubyte b)
{
	for (b_umem i = 0; i < buf.cap; i++)
		buf.b[i] = b;
}


#endif
