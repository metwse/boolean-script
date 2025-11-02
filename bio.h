/**
 * @file bio.h
 * @brief Buffered streams
 *
 * File I/O wrappers
 */

#ifndef BIO_H
#define BIO_H

#include "bdef.h"


#define B_EOF (-1)

struct bio;

typedef const char *(*bio_reader)(void *additional_data);

struct bio {
	// bytes unread
	b_umem n;
	// reader function
	bio_reader reader;
	// Additional state for use in reader
	void *data;
};


void bio_init(struct bio *, bio_reader *reader_func, void *aux_data);


#endif
