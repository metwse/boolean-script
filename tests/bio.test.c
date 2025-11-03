#include "../bdef.h"
#include "../bio.h"

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "mock_input_stream.h"


void test(b_umem stream_size, b_umem read_limit)
{
	struct bio bio;

	// initial data
	char *stream = malloc(sizeof(char) * stream_size);

	for (b_umem i = 0; i < stream_size; i++)
		// fill random data
		stream[i] = rand() % 254 + 1;

	bio_init(&bio, mock_input_stream,
		 new_mock_input_stream_state(stream, stream_size));

	// null-terminated read output
	char *collect = malloc(sizeof(char) * (stream_size + 1));
	char *collect_cursor = collect;

	while (true) {
		// number of bytes trying to read
		b_umem wants_read = rand() % read_limit + 1;

		const char *data = bio_read(&bio, wants_read);

		if (!data)
			break;

		b_assert_expr(strlen(data) <= wants_read,
			      "data is not null-terminated");

		strcpy(collect_cursor, data);
		collect_cursor += wants_read;
	}

	b_assert_expr(memcmp(collect, stream, stream_size) == 0,
		      "collected data differs from initial data");

	free(collect);
	free(stream);

	free(bio_destroy(&bio));
}

int main()
{
	srand(time(NULL));

	for (int _fuzz = 0; _fuzz < 512; _fuzz++) {
		for (int i = 1; i < 8; i++) {
		b_umem stream_size = (rand() % 5 + 1) << i;
			for (int j = 1; j < 8; j++) {
				b_umem read_limit = (rand() % 5 + 1) << j;

				test(stream_size, read_limit);
			}
		}
	}
}
