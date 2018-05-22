#include <stdio.h>
#include <stdlib.h>
#include "./h/concat_string.h"
#include "./h/string_length.h"



char* concat_string(char* stringa1, char* stringa2){

    int size1 = string_length(stringa1);
    int size2 = string_length(stringa2);

    printf("lunghezza %i - %i \n", size1, size2);

    char* new_string = (char*) malloc(sizeof(char) * (size1 + size2 + 1));

    int i;
    for(i = 0; *stringa1 != '\0'; i++, stringa1++, new_string++){
        *new_string = *stringa1;
    }

    for( ; *stringa2 != '\0'; i++, stringa2++, new_string++){
        *new_string = *stringa2;
    }
    i++;
    *new_string++ = '\0';
    new_string = new_string - i;

    return new_string;

}

