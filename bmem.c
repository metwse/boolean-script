#include "bmem.h"
#include "bdef.h"

#include <stdlib.h>


struct b_buffer {
	b_byte *b;
	b_umem cap;
};


void b_buffer_init(struct b_buffer *buf, b_umem cap) {
	buf->cap = cap;

	if (cap) {
		buf->b = malloc(cap);

		b_assert("malloc should not return NULL", buf->b);
	} else {
		buf->b = NULL;
	}
}

/* sets buffer capacity to 0 */
void b_buffer_reset(struct b_buffer *buf) {
	if (buf->cap) {
		buf->cap = 0;
		free(buf->b);
	}
}

void b_buffer_resize(struct b_buffer *buf, b_umem cap) {
	if (cap == 0) {
		return b_buffer_reset(buf);
	}

	buf->b = realloc(buf->b, cap);

	b_assert("realloc should not return NULL", buf->b);
}

inline b_byte b_buffer_at(struct b_buffer const *buf, b_umem i) {
	b_assert("buffer out of boundary", i < buf->cap);

	return buf->b[i];
}

inline char b_buffer_char_at(struct b_buffer const *buf, b_umem i) {
	return (char) b_buffer_at(buf, i);
}

inline void b_buffer_set(struct b_buffer *buf, b_umem i, b_byte b) {
	b_assert("buffer out of boundary", i < buf->cap);

	buf->b[i] = b;
}

inline void b_buffer_setall(struct b_buffer *buf, b_byte b) {
	for (b_umem i = 0; i < buf->cap; i++)
		buf->b[i] = b;
}
