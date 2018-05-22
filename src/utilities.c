#include <stdio.h>
#include <stdlib.h>
#include "../include/utilities.h"

int string_length(char* string){
    int i;
    for(i = 0; *string != '\0'; i++, string++){}

    return i;
}

char* from_int_to_string(int numero){ 
 
    int val = numero; 
    int count = 0; 
 
    while(val != 0){ 
        val = val / 10; 
        count++; 
    } 
    char *stringa = (char*) malloc(sizeof(char) * count + 1); 
 
    int i; 
 
    for(i = 0; i < count; i++){ 
        val = numero % 10; 
        numero = numero / 10; 
        switch(val){ 
            case 1: 
                stringa[i] = '1'; 
                break; 
            case 2: 
                stringa[i] = '2'; 
                break; 
            case 3: 
                stringa[i] = '3'; 
                break; 
            case 4: 
                stringa[i] = '4'; 
                break; 
            case 5: 
                stringa[i] = '5'; 
                break; 
            case 6: 
                stringa[i] = '6'; 
                break; 
            case 7: 
                stringa[i] = '7'; 
                break; 
            case 8: 
                stringa[i] = '8'; 
                break; 
            case 9: 
                stringa[i] = '9'; 
                break;                 
            default: 
                stringa[i] = '0'; 
                break; 
        } 
    } 
    i++; 
    stringa[i] = '\0'; 
    int a; 
    char *conv = (char*) malloc(sizeof(char) * count + 1); 
 
    for(i = count - 1, a = 0; i >= 0; i--, a++){ 
        conv[i] = stringa[a]; 
    } 
    conv[count] = '\0'; 
    return conv; 
}

void copy_string(char *stringa1, char* stringa2){

    int i;
    for(i = 0; *stringa2 != '\0'; stringa1++, stringa2++, i++){
        *stringa1 = *stringa2;
    }
    i++;
    *stringa1++ = '\0';

}

char* concat_string(char* stringa1, char* stringa2){

    int size1 = string_length(stringa1);
    int size2 = string_length(stringa2);

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

