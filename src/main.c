#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <padre.h>
#include <types.h>
#include <utilities.h>


int main(int argc, char* argv[]){

    // controlla che vengano passati almeno tre paramatri
    if(argc != 3){
        printing("Inserisci:\nkey_finder <file_plain_encoded> <file_key_destination>");
        exit(0);
    }
    
    // viene chiamato il wrapper del processo padre
    padre(argv[1], argv[2]);  

    return 0;
}

