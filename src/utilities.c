#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utilities.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <constants.h>

int string_length(char* string){

    int i;
    for(i = 0; string[i] != '\0'; i++);
    return i;

}

char* from_int_to_string(int numero){ 
    
    int val = numero;
    int cifre = 0;
    do{
        val = val / 10;
        cifre++;
    }while(val != 0);
    char *new_string = (char*) malloc(sizeof(char) * cifre + 1);
    // char *new_string = (char*) my_malloc(KEY_SHM3, 512);
    

    int i = 0;
    for(i = cifre - 1; i >= 0; i-- ){
        new_string[i] = (char) (numero % 10) + '0';
        numero = numero/10;
    }
    new_string[cifre] = '\0';

    
    return new_string; 
}

char* from_unsigned_to_hexa(unsigned numero){ 
    
    unsigned val = numero;
    char *new_string = (char*) malloc(sizeof(char) * 9);
    // char* new_string = (char*) my_malloc(KEY_SHM4, 512);

    int i = 0;
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

void copy_string(char *stringa1, char* stringa2){

    int i;
    for(i = 0; stringa2[i] != '\0'; i++){
        stringa1[i] = stringa2[i];
    }
    stringa1[i] = '\0';

}

char* concat_string(char* stringa1, char* stringa2){

    int size1 = string_length(stringa1);
    int size2 = string_length(stringa2);
    int size = size1 + size2;

    char* new_string = (char*) malloc(sizeof(char) * (size1 + size2 + 1));
    // char* new_string = (char*) my_malloc(KEY_SHM5, 512);

    int i;
    for(i = 0; stringa1[i] != '\0'; i++){
        new_string[i] = stringa1[i];
    }

    for(int j = 0 ; stringa2[j] != '\0'; i++, j++){
        new_string[i] = stringa2[j];
    }
    new_string[i] = '\0';
    
    return new_string;

}

void printing(char *stringa){
    int i = 0;
    for( ; stringa[i] != '\0'; i++);
    i++;
    stringa[i] = '\n';
    i++;
    write(1, stringa, i);
}

// void* my_malloc(int key, int size){
//     int shmid = shmget (key, (size + sizeof(struct Memory)), 0666 | IPC_CREAT);
//     if ( shmid == -1){
//         perror("blabla");
//         exit(1);
//     }
//     struct Memory *s3 = (struct Memory *) shmat (shmid , NULL , 0);
//     if ( s3 == ( void *) -1) {
//         perror("bubu");
//         exit(1);
//     }
//     s3 -> key = key ;
//     s3 -> shmid = shmid ;
//     return s3 -> buf;
// }

// void my_free (void * s3) {
//     struct Memory tmp;
//     //the same as shmget
//     struct Memory *memory = (struct Memory *)(((char *) s3) - (((char *)&tmp.buf) - ((char *)&tmp.key)));
//     int shmid = memory -> shmid ;
//     shmdt (memory);
//     shmctl (shmid, IPC_RMID, NULL);
// }

