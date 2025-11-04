/**
 * @file mock_input_stream.h
 * @brief mock bio implementation
 *
 * This header contains a mock bio implementation with static linkage.
 */

#ifndef MOCK_INPUT_STREAM_H

#include "../bdef.h"
#include "../bmem.h"

#include <stdlib.h>
#include <string.h>


/** @brief read state */
struct mock_input_stream_state {
	/** position in the buffer */
	b_umem p;
	/** length of the buffer */
	b_umem len;
	/** underlying buffer */
	const char *buf;
};


/** initializes a mock input stream from state */
b_umem mock_input_stream(struct b_buffer *buf, void *state_)
{
	struct mock_input_stream_state *state = state_;

	b_umem chunk_size = rand() % 1024 + 1;

	if (state->p + chunk_size > state->len)
		chunk_size = state->len - state->p;

	if (chunk_size == 0)
		return 0;

	b_buffer_with_cap(buf, chunk_size);
	memcpy(buf->b, state->buf + state->p, chunk_size);

	state->p += chunk_size;

	return chunk_size;
}

/** creates a new state from null-terminated string */
struct mock_input_stream_state *new_mock_input_stream_state(const char *buf,
							    b_umem len)
{
	struct mock_input_stream_state *state = malloc(
		sizeof(struct mock_input_stream_state)
	);

	state->p = 0;
	state->len = len;
	state->buf = buf;

	return state;
}


#endif
