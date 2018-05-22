#include <stdio.h>
#include <stdlib.h>
#include "./h/from_int_to_string.h"
#include "./h/concat_string.h"
#include "./h/string_length.h"
#include "./h/copy_string.h"

int main(){

    int numero = 5637482;
    char* stringa1 = from_int_to_string(numero);
    char* stringa2 = " provissima";
    char* new_stringa = concat_string(stringa1, stringa2);
    char* copy = (char*) malloc(sizeof(char) * string_length(new_stringa));
    copy = copy_string(copy, new_stringa);

    printf("conversione %s\n", stringa1);
    printf("concatenazione %s\n", new_stringa);
    printf("copia %s\n", copy);



    return 0;
}