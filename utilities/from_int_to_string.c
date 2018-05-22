#include <stdio.h> 
#include <stdlib.h> 
#include "./h/from_int_to_string.h"

 
 
char* from_int_to_string(int numero){ 
 
    // int numero = 1234; 
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