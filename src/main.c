#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "../include/padre.h"
#include "../include/types.h"


int main(int argc, char* argv[]){

    // controllo dei parametri passati da linea di comando
    if(argc != 3){
        printf("Insert:\nkey_finder <file_plain_ancoded> <file_key_destination>\n");
        exit(1);
    }
    /*AREA DEBUG
        printf("MAIN: Starting father, pid: %i\n", getpid());
    */
    padre(argv[1], argv[2]);  

    return 0;
}

