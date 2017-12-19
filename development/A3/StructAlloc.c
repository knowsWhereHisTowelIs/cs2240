// A demo of some dynamically allocated structs and arrays thereof
// And a test of struct switcheroo using a statically allocated intermediate
// Of course, we just need to move pointers to sort.
// R. Trenary
// 1/25/12
#include < stdio.h > #include < stdlib.h > // for calloc
#define DEBUG 1
typedef struct {
	char field1[2]; // notice an array field
	float field2;
}
MyStructDataType;
// Now we have a data type
static MyStructDataType * * MyStructPtrArray; // An array of pointers to structs
void Sort(MyStructDataType * * ); // Pass the Address of a pointer
int main() {
	MyStructDataType MyTempStruct; // a static struct for later use
	// Now use our array of pointers to MyStructDataTypes
	MyStructPtrArray = calloc(10, sizeof(MyStructDataType * )); // 10 is arbitrary
	// This allocates space for 10 pointers to MyStructDataTypes
	MyStructPtrArray[4] = (MyStructDataType * ) calloc(1, sizeof(MyStructDataType));
	// Here we fill a couple of the pointer variable with valid address
	// 4 and 7 are arbitrary
	MyStructPtrArray[7] = (MyStructDataType * ) calloc(1, sizeof(MyStructDataType));

	// And use the pointers to fill the dynamically allocated struct fields
	(MyStructPtrArray[4] - > field1[0]) = '5';
	((MyStructPtrArray[4]) - > field1[1]) = '6'; // fill array locations explicitly
	(MyStructPtrArray[4]) - > field2 = 5.6;
	// Using  pointers
	((MyStructPtrArray[7]) - > field1[0]) = '8';
	(MyStructPtrArray[7]) - > field1[1] = '9'; // As above
	(MyStructPtrArray[7]) - > field2 = 75.6;
	//
	printf("Before Switcheroo  \n");
	printf("Pointed to by 4  %c%c and %f\n", ((MyStructPtrArray[4]) - > field1[0]), ((MyStructPtrArray[4]) - > field1[1]), ((MyStructPtrArray[4]) - > field2));
	printf("Pointed to by 7  %c%c and %f\n", ((MyStructPtrArray[7]) - > field1[0]), ((MyStructPtrArray[7]) - > field1[1]), ((MyStructPtrArray[7]) - > field2));
	// An exchange switcheroo ?
	MyTempStruct = * (MyStructPtrArray[4]); // LOOK ! LOOK ! Struct assignment including arraysSome structs and pointers
	*
	(MyStructPtrArray[4]) = * (MyStructPtrArray[7]); * (MyStructPtrArray[7]) = MyTempStruct;
	// For both static and dynamic structs
	if (DEBUG) printf("Debug statement");


	printf("After Switcheroo ? \n");
	printf("Pointed to by 4  %c%c and %f\n", ((MyStructPtrArray[4]) - > field1[0]), ((MyStructPtrArray[4]) - > field1[1]), ((MyStructPtrArray[4]) - > field2));
	printf("Pointed to by 7  %c%c and %f\n", ((MyStructPtrArray[7]) - > field1[0]), ((MyStructPtrArray[7]) - > field1[1]), ((MyStructPtrArray[7]) - > field2));

	Sort(MyStructPtrArray);
	printf("After SORT ? \n");
	printf("Pointed to by 4  %c%c and %f\n", ((MyStructPtrArray[4]) - > field1[0]), ((MyStructPtrArray[4]) - > field1[1]), ((MyStructPtrArray[4]) - > field2));
	printf("Pointed to by 7  %c%c and %f\n", ((MyStructPtrArray[7]) - > field1[0]), ((MyStructPtrArray[7]) - > field1[1]), ((MyStructPtrArray[7]) - > field2));


}
void Sort(MyStructDataType * * MyOtherStructPtr) {
	int i, j;
	MyStructDataType MyTempStruct;
	// An exchange switcheroo ?
	MyTempStruct = * (MyOtherStructPtr[4]); // LOOK ! LOOK ! Struct assignment including arrays
	*
	(MyOtherStructPtr[4]) = * (MyOtherStructPtr[7]); * (MyOtherStructPtr[7]) = MyTempStruct; // For both static and dynamic structs


}
