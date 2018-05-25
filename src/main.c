#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "../include/padre.h"
#include "../include/types.h"
#include "../include/utilities.h"


int main(int argc, char* argv[]){

    // controllo dei parametri passati da linea di comando
    if(argc != 3){
        char *messaggio = "Insert:\nkey_finder <file_plain_ancoded> <file_key_destination>";
        write(1, messaggio, string_length(messaggio));
        exit(0);
    }
    
    padre(argv[1], argv[2]);  

    return 0;
}

