#include <stdio.h>
#include <stdlib.h>
#include "./h/copy_string.h"

#include <string.h>

char* copy_string(char *stringa1, char* stringa2){

    int i;
    for(i = 0; *stringa2 != '\0'; stringa1++, stringa2++, i++){
        *stringa1 = *stringa2;
    }
    i++;
    *stringa1++ = '\0';
    stringa1 = stringa1 - i;

    return stringa1;
}