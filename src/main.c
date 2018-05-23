#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "../include/padre.h"
#include "../include/types.h"


int main(int argc, char* argv[]){

    // controllo dei parametri passati da linea di comando
    if(argc != 3){
        perror("Insert:\nkey_finder <file_plain_ancoded> <file_key_destination>\n");
        exit(1);
    }
    
    padre(argv[1], argv[2]);  

    return 0;
}

