#include <stdio.h>
#include <stdlib.h>

#include "vkapi_alloc.h"


void *
vkapi_emalloc(size_t size)
{
	void *ptr;

	ptr = malloc(size);

	if (ptr == NULL) {
		fprintf(stderr,
		        "FATAL ERROR: vkapi_emalloc is failed to allocate memory.");
		exit(EXIT_FAILURE);
	}

	return ptr;
}

void *
vkapi_erealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);

	if (ptr == NULL) {
		fprintf(stderr,
		        "FATAL_ERROR: vkapi_realloc is failed to reallocate memory.");
		exit(EXIT_FAILURE);
	}

	return ptr;
}
