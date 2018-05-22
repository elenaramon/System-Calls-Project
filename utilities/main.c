#include <stdio.h>
#include <stdlib.h>
#include "./h/from_int_to_string.h"
#include "./h/concat_string.h"
#include "./h/string_length.h"

int main(){

    int numero = 5637482;
    char* stringa1 = from_int_to_string(numero);
    char* stringa2 = " provissima";
    char* new_stringa = concat_string(stringa1, stringa2);

    printf("conversione %s\n", stringa1);
    printf("concatenazione %s\n", new_stringa);



    return 0;
}