#include <stdio.h>
#include <stdlib.h>

#include "nu_malloc.h"

int main()
{
	int *ptr = nu_malloc(sizeof(int));
	
	if (ptr == NULL)
	{
		printf("Memory failed to allocate!\n");
		return 1;
	}

	*ptr = 4;
	nu_free(ptr);

	printf("Memory was allocated, used, and freed!\n");	
	return 0;
}
