#include "bmem.h"
#include "bdef.h"

#include <stdlib.h>


void b_buffer_init(struct b_buffer *buf)
{
	buf->cap = 0;
	buf->b = NULL;
}

void b_buffer_with_cap(struct b_buffer *buf, b_umem cap)
{
	buf->cap = cap;

	if (cap) {
		buf->b = malloc(cap);

		b_assert_mem(buf->b);
	} else {
		buf->b = NULL;
	}
}

/* set buffer capacity to 0 */
void b_buffer_reset(struct b_buffer *buf)
{
	if (buf->cap) {
		free(buf->b);

		buf->b = NULL;
		buf->cap = 0;
	}
}

void b_buffer_resize(struct b_buffer *buf, b_umem cap)
{
	if (cap == 0)
		return b_buffer_reset(buf);

	buf->b = buf->cap == 0 ? malloc(cap) : realloc(buf->b, cap);
	buf->cap = cap;

	b_assert_mem(buf->b);
}

void b_stack_init(struct b_stack *s)
{
	s->len = 0;
	b_buffer_init(&s->buf);
}

void b_stack_with_cap(struct b_stack *s, b_umem cap)
{
	s->len = 0;
	b_buffer_with_cap(&s->buf, cap);
}

void b_stack_reset(struct b_stack *s)
{
	s->len = 0;
	b_buffer_reset(&s->buf);
}

void b_stack_resize(struct b_stack *s, b_umem cap)
{
	b_assert_boundary(cap >= s->len,
			  "cannot fit stack with len %llu to buffer with cap %llu",
			  s->len, cap);

	b_buffer_resize(&s->buf, cap);
}

b_ubyte b_stack_pop(struct b_stack *s)
{
	b_assert_boundary(s->len > 0, "stack is empty");

	return *b_buffer_at(s->buf, --s->len);
}

void b_stack_push(struct b_stack *s, b_ubyte b)
{
	if (s->len == s->buf.cap)
		b_buffer_resize(&s->buf, s->buf.cap + B_STACK_CHUNK_SIZE);

	*b_buffer_at(s->buf, s->len++) = b;
}

b_ubyte b_stack_peek(const struct b_stack *s)
{
	return *b_buffer_at(s->buf, s->len - 1);
}

void *b_stack_pop_elem(struct b_stack *s, b_umem size)
{
	b_assert_boundary(s->len > size - 1, "stack is empty");

	s->len -= size;

	return cast(void *, b_buffer_at(s->buf, s->len));
}

void b_stack_push_elem(struct b_stack *s, const void *elem, b_umem size)
{
	while (s->len + size >= s->buf.cap)
		b_buffer_resize(&s->buf, s->buf.cap + B_STACK_CHUNK_SIZE);

	for (b_umem i = 0; i < size; i++)
		s->buf.b[s->len + i] = cast(b_ubyte *, elem)[i];

	s->len += size;
}

void *b_stack_peek_elem(const struct b_stack *s, b_umem size)
{
	return cast(void *, b_buffer_at(s->buf, s->len - size));
}
