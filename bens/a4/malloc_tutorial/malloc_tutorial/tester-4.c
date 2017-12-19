#include <stdio.h>
#include <stdlib.h>

#include "nu_malloc.h"

#define START_MALLOC_SIZE 1024*1024*128
#define STOP_MALLOC_SIZE  1024

void *reduce(void *ptr, int size)
{
	if (size > STOP_MALLOC_SIZE)
	{
		void *ptr1 = nu_realloc(ptr, size / 2);
		void *ptr2 = nu_malloc(size / 2);

		if (ptr1 == NULL || ptr2 == NULL)
		{
			printf("Memory failed to allocate!\n");
			exit(1);
		}

		ptr1 = reduce(ptr1, size / 2);
		ptr2 = reduce(ptr2, size / 2);

		if (*((int *)ptr1) != size / 2 || *((int *)ptr2) != size / 2)
		{
			printf("Memory failed to contain correct data after many allocations!\n");
			exit(2);
		}

		ptr1 = nu_realloc(ptr1, size);
		nu_free(ptr2);

		if (*((int *)ptr1) != size / 2)
		{
			printf("Memory failed to contain correct data after realloc()!\n");
			exit(3);
		}

		*((int *)ptr1) = size;
		return ptr1;
	}
	else
	{
		*((int *)ptr) = size;
		return ptr;
	}


}


int main()
{
	nu_malloc(1);

	int size = START_MALLOC_SIZE;
	while (size > STOP_MALLOC_SIZE)
	{
		void *ptr = nu_malloc(size);
		ptr = reduce(ptr, size / 2);
		nu_free(ptr);

		size /= 2;
	}

	printf("Memory was allocated, used, and freed!\n");
	return 0;
}
