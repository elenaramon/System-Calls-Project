#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utilities.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <constants.h>

int length(char *string){

    int i;
    // scorrimento della stringa fino al carattere di terminazione
    for(i = 0; string[i] != '\0'; i++);
    return i;

}

char *itos(int numero){ 
    
    int val = numero;
    int cifre = 0;
    // conteggio del numero di cifre
    do{
        val = val / 10;
        cifre++;
    }while(val != 0);

    // allocazione dello spazio necessario per la memorizzazione della stringa
    char *new_string = (char*) malloc(sizeof(char) * cifre + 1);    

    int i = 0;
    // ciclo di conversione dei numeri
    for(i = cifre - 1; i >= 0; i-- ){
        new_string[i] = (char) (numero % 10) + '0';
        numero = numero/10;
    }
    new_string[cifre] = '\0';
    
    return new_string; 

}

char *utoh(unsigned numero){ 
    
    unsigned val = numero;
    char *new_string = (char*) malloc(sizeof(char) * 9);

    int i = 0;
    // ciclo di conversione dei numeri
    for(i = 7; i >= 0; i-- ){
        if((numero % 16) <= 9){
            new_string[i] = (char) (numero % 16) + '0';
        }
        else{
            new_string[i] = (char) ((numero % 16) - 10) + 'A';
        }
        numero = numero/16;
    }
    new_string[i] = '\0';
    
    return new_string; 

}

void copy(char *stringa1, char *stringa2){

    int i;
    // ciclo di copia della stringa
    for(i = 0; stringa2[i] != '\0'; i++){
        stringa1[i] = stringa2[i];
    }
    stringa1[i] = '\0';

}

char *concat(char *stringa1, char *stringa2){

    int size = length(stringa1) + length(stringa2) + 1;

    char *new_string = (char*) malloc(sizeof(char) * (size));

    int i;
    // copia della prima stringa in quella nuova
    for(i = 0; stringa1[i] != '\0'; i++){
        new_string[i] = stringa1[i];
    }
    // copia della seconda stringa in quella nuova
    int j;
    for(j = 0 ; stringa2[j] != '\0'; i++, j++){
        new_string[i] = stringa2[j];
    }
    new_string[i] = '\0';
    
    return new_string;

}

void printing(char *stringa){

    char *message = concat(stringa, "\n");
    write(1, message, length(message));

    free(message);

}

