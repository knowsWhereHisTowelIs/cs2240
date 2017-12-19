#include <stdio.h>
#include <stdlib.h>

int compf(const void *a, const void *b, void * dummy);

int values[] = { 3, 10, 5, 20, 1, 5 };

int compf(const void *a, const void *b, void * dummy) {
	return *(int*)a - *(int*)b;
}

int main(int argc, char *argv[]) {
	int i = 0;
	int a = 6;
	int b = 2;
     void * dummy = NULL;
	printf("%d\n",compf(&a,&b,dummy));
	qsort_r(&values[0], (size_t)a, (size_t)sizeof(values[0]), compf, dummy);
	for(i=0; i<6; i++) {
		printf("%d,",values[i]);
	}
	printf("\n");
}

