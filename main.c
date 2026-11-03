#include <stdio.h>
#include <stdlib.h>

int main(){
    /*Paso 1: Medir bytes*/
    int x;

    printf("%zu\n", sizeof(x));
    printf("%zu\n", sizeof(int));

    /*Paso 2: Reservar memoria */
    int students[20];
    printf("%zu\n", sizeof(students)); // 80 bytes
    printf("%zu\n", sizeof(students[0])); // 4 bytes

    int *ptrstudents;
    ptrstudents = calloc(20, sizeof(*ptrstudents)); // 80 bytes
    printf("%d\n",ptrstudents[19]);
    return 0; 
}