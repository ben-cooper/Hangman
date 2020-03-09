#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void *e_malloc(size_t size)
{
	void *result = malloc(size);
	
	if (!result)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	
	return result;
}

void *e_calloc(size_t nmemb, size_t size)
{
	void *result = calloc(nmemb, size);
	
	if (!result)
	{
		perror("calloc");
		exit(EXIT_FAILURE);
	}
	
	return result;
}

void *e_realloc(void *ptr, size_t size)
{
	void *result = e_realloc(ptr, size);
	
	if (!result)
	{
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	
	return result;
}
