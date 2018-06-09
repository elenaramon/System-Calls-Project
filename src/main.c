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
        char *messaggio = "Insert:\nkey_finder <file_plain_ancoded> <file_key_destination>";
        write(1, messaggio, length(messaggio));
        exit(0);
    }
    
    // viene chiamato il wrapper del processo padre
    padre(argv[1], argv[2]);  

    return 0;
}

