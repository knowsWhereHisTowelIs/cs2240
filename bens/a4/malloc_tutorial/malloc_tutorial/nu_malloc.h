//
//  nu_malloc.h
//  malloc_tutorial
//
//  Created by Benjamin Masters on 10/13/15.
//  Copyright © 2015 Benjamin Masters. All rights reserved.
//

#ifndef nu_malloc_h
#define nu_malloc_h

#include <stdio.h>

void *nu_malloc(size_t size);
void nu_free(void *ptr);
void *nu_realloc(void *ptr, size_t size);
void *nu_calloc(size_t nelem, size_t elsize);


#endif /* nu_malloc_h */
