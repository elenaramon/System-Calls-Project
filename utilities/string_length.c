#include <stdio.h>
#include <stdlib.h>
#include "./h/string_length.h"


int string_length(char* string){
    int i;
    for(i = 0; *string != '\0'; i++, string++){}

    return i;
}