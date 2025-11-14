/**
 * @file bmem.h
 * @brief Memory management interface
 *
 * Safe memory allocation wrappers.
 */

#ifndef BMEM_H
#define BMEM_H

#include "bdef.h"


#define B_STACK_CHUNK_SIZE 32


/** @brief zero-cost array wrapper */
struct b_buffer {
	b_ubyte *b /** underlying array */;
	b_umem cap /** buffer capacity */;
};

/** @brief zero-cost array wrapper */
struct b_stack {
	struct b_buffer buf /** underlying buffer */;
	b_umem len /** elements count in stack */;
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
static inline b_ubyte *b_buffer_at(struct b_buffer buf, b_umem i)
{
	b_assert_boundary(i < buf.cap, "buffer out of capacity");

	return &buf.b[i];
}

/** @brief segmentate buffer with size s, return element at i */
static inline void *b_buffer_elem_at(struct b_buffer buf, b_umem i, b_umem s)
{
	b_assert_boundary(s * i < buf.cap, "buffer out of capacity");

	return cast(void *, &buf.b[i * s]);
}

/** @brief initializes an empty stack */
void b_stack_init(struct b_stack *);

/** @brief initializes a stack with given cap */
void b_stack_with_cap(struct b_stack *, b_umem cap);

/** @brief sets stack capacity back to 0 */
void b_stack_reset(struct b_stack *);

/** @brief resizes underlying stack */
void b_stack_resize(struct b_stack *, b_umem cap);

/** @brief removes the top byte from the buffer */
b_ubyte b_stack_pop(struct b_stack *);

/** @brief pushes a byte to the stack, expands the stack if required */
void b_stack_push(struct b_stack *, b_ubyte);

/** @brief returns the top byte on the stack */
b_ubyte b_stack_peek(const struct b_stack *);

/** @brief removes the top element with size s from the buffer */
void *b_stack_pop_elem(struct b_stack *, b_umem s);

/** @brief pushes an element with size s to the stack */
void b_stack_push_elem(struct b_stack *, const void *, b_umem s);

/** @brief returns the top element with size s on the stack */
void *b_stack_peek_elem(const struct b_stack *, b_umem s);


#endif
