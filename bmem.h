/**
 * @file bmem.h
 * @brief Memory management interface
 *
 * Safe memory allocation wrappers.
 */

#ifndef BMEM_H
#define BMEM_H

#include "bdef.h"


struct b_buffer;


/* initializes a buffer with cap */
void b_buffer_init(struct b_buffer *, b_umem cap);

/* sets buffer capacity to 0 */
void b_buffer_reset(struct b_buffer *);

void b_buffer_resize(struct b_buffer *, b_umem cap);

inline b_byte b_buffer_at(struct b_buffer const *, b_umem i);

inline char b_buffer_char_at(struct b_buffer const *, b_umem i);

inline void b_buffer_set(struct b_buffer *, b_umem i, b_byte b);

inline void b_buffer_setall(struct b_buffer *, b_byte b);


#endif
