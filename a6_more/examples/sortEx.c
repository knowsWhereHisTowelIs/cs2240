/* qsort example */
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int price;
    int id;
} order;
order list[6];
order **list2;
int i = 0;

int compare(const void * a, const void * b) {

    order *orderA = (order *) a;
    order *orderB = (order *) b;

    return ( orderB->price - orderA->price);
}

int main() {
    srand(time(NULL));

    printf("Before sorting\n");
    list2 = malloc(6);
    for (i = 0; i < 6; i++) {
        list[i].price = rand() % 10;
        list[i].id = i;

        list2[i] = malloc(sizeof (order));
        list2[i]->id = i;
        list2[i]->price = list[i].price;

        printf("Order id = %d Price = %d \n", list[i].id, list[i].price);
    }
    printf("AFTER sorting\n");
    int n;
    qsort(list, 6, sizeof (order), compare);
    for (n = 0; n < 6; n++)
        printf("Order id = %d Price = %d \n", list[n].id, list[n].price);

    printf("\n------------------------------\n");
    for (n = 0; n < 6; n++)
        printf("Order id = %d Price = %d \n", list2[n]->id, list2[n]->price);
    return 0;
}
