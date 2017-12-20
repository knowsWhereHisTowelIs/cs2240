/* Tiffany Lawrence 
*  Assignment 4: BYOB
*  10/8/15
*/
#include <stdio.h>
#include <string.h>
 

typedef struct _heap_block
{
    int size;
    int free;
}heap_block;
typedef heap_block *next;
 
char *mem_start_p;
int max_mem;
int allocated_mem; /* this is the memory in use. */
int _heap_block_count;
 
char *heap_end;
 
enum {NEW__heap_block=0,NO__heap_block,REUSE__heap_block};
enum {FREE,IN_USE};

void *nu_malloc(size_t size)
{
    /* check whether any chunk (allocated before) is free first */
 
    next p__heap_block;
    int flag = NO__heap_block;
 
    p__heap_block = (next)mem_start_p;
 
    int sz;
 
    sz = sizeof(heap_block);
 
    if( (size + sz)  > (max_mem - (allocated_mem + _heap_block_count * sz ) ) )
    {
        fprintf(stderr, "You're out of memory, Do not pass go, Do not collecgt $200...\n");
        return NULL;
    }
    while( heap_end > ( (char *)p__heap_block + size + sz)   )
    {
 
        if ( p__heap_block->free == 0)
        {
 
            if( p__heap_block->size == 0)
            {
                flag = NEW__heap_block;
                break;
            }
            if( p__heap_block->size > (size + sz) )
            {
                flag = REUSE__heap_block;
                break;
            }
        }
        p__heap_block = (next) ( (char *)p__heap_block + p__heap_block->size);
 
 
    }
 
    if( flag != NO__heap_block)
    {
        p__heap_block->free = 1;
 
        if( flag == NEW__heap_block)
        {
            p__heap_block->size = size + sizeof(heap_block);
            _heap_block_count++;
        }
        allocated_mem += size;
        return ( (char *) p__heap_block + sz);
    }
 
    printf(" Could not allocate any memory to _heap_block \n");
    return NULL;
 
 
}

void *nu_calloc(size_t count, size_t size){

	size_t total = count * size;
	void *p = nu_malloc(total);

	/* currently still a work in progress */

}

void *nu_realloc(void *ptr, size_t size)
{
    /* store the ptr and size in global variable */
    max_mem = size;
    mem_start_p = ptr;
    _heap_block_count = 0;
    allocated_mem = 0;
    heap_end = mem_start_p + size;
 
} 

void nu_free(void *p)
{
    /* Mark in _heap_block that this chunk is free */
    next ptr = (next)p;
    ptr--;
 
    _heap_block_count--;
    ptr->free = FREE;
    printf("\nAllocated mem: %d ",ptr->size);
    allocated_mem -= (ptr->size - sizeof(heap_block));
    printf("\nAllocated mem: %d ",allocated_mem);
    printf("\nMemory Freed...");
}

int heap_count()
{
    /* keep track of number of _heap_blocks in a global variable */
    _heap_block_count++;
    return _heap_block_count;
 
}
 
int main()
{
	/* Just me testing things to see how they are working, still haven't done test implementing */
    char buf[1024];
    memset(buf,0,1024);
 
    nu_realloc(buf,1024);
 
    char *str,*str1;
 
    str=nu_malloc(100);
    printf("\nMemory address: %p",str);
    printf("\n_heap_block count: %-3d \tAllocated Memory: %-10d",_heap_block_count,allocated_mem);
    
    str1=nu_calloc(_heap_block_count,200);
    printf("\n\nMemory address: %p",str1);
    printf("\n_heap_block count: %-3d \tAllocated Memory: %-10d\n",_heap_block_count,allocated_mem);
    nu_free(str);
    nu_free(str1);
}


