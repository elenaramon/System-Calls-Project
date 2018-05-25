#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/padre.h"
#include "../include/logger.h"
#include "../include/types.h"
#include "../include/figlio.h"
#include "../include/utilities.h"
#include "../include/costanti.h"


int lines;
struct shmid_ds shmid_struct;

void padre(char *file_name_input, char *file_name_output){

    /*
        son_logger, figlio: variabili per i pid dei processi figli
        *status_struct: struttura Status
        file_descriptor_*: file descriptor per file passati in input
        shm_size*: dimensione delle zone di memoria condivise
        shm_id*: id delle zone di memoria condivise
        *shm_address*: indirizzi di memoria condivisa a cui attaccarsi
        *shm_write: variabile per la scrittura nella memoria condivisa del file input
        line_counter: variabile per il conteggio delle chiavi (conta il numero di righe del file <plai_encoded>)
    */
    
    pid_t son_logger, son_figlio;
    int file_descriptor_input, file_descriptor_output;
    int shm_size1, shm_size2;

    // Apertura del file di input
    if((file_descriptor_input = open(file_name_input, O_RDONLY, 0777)) == -1){
        perror("PADRE: File descriptor open error");
        exit(1);
    }


    // Apertura del file di output
    if((file_descriptor_output = open(file_name_output, O_WRONLY | O_CREAT, 0777)) == -1){
        perror("PADRE: File descriptor 2 open error");
        exit(1);
    }


    // Calcolo dimensione zona di memoria condivisa
    shm_size1 = sizeof(struct Status) + lseek(file_descriptor_input, 0L, SEEK_END);

    // Creazione e collegamento della zona di memoria condivisa
    void *s1 = attach_segments(KEY_SHM1, shm_size1);

    // Definizione posizione struttura
    struct Status* status = (struct Status*) s1;
    status->id_string = 0;
    status->grandson = 0;

    // Definizione posizione file di input
    char *shm_write1 = (char*)s1 + sizeof(struct Status);
    lines = load_file(shm_write1, file_descriptor_input);

    // Calcolo dimensione seconda zona di memoria condivisa
    shm_size2 = sizeof(char) * 11 * lines;

    // Creazione e collegamento della seconda zona di memoria condivisa
    void *s2 = attach_segments(KEY_SHM2, shm_size2);

    // Creazione del figlio logger
    if((son_logger = fork()) == -1){
        perror("PADRE: Logger son creation error");
        exit(1);
    }
    else if(son_logger == 0){
        // Esecuzione di logger
        logger();
    }
    else{

        // Creazione del figlio figlio
        if((son_figlio = fork()) == -1){
            perror("PADRE: Figlio son creation error");
            exit(1);
        }
        else if(son_figlio == 0){
             // Esecuzione di figlio
            // figlio(shm_size1, line_counter);
            figlio(shm_size1, lines);
        }
        else{
             // Esecuzione del padre
            // Attende la terminazione dei figli
            wait(&son_figlio);
            wait(&son_logger);

            check_keys((char*)(s1 + sizeof(struct Status)), (unsigned*) s2);

            // Salva le chiavi nel file di output
            save_keys((unsigned*) s2, file_descriptor_output);

            // Eliminazione dei segmenti di memoria condivisa
            detach_segments(shm_size1, KEY_SHM1, s1);
            detach_segments(shm_size2, KEY_SHM2, s2);
        }
    }

}

void * attach_segments(int key, int shm_size){

    int shm_id;
    char *shm_address;

    // Creazione della zona di memoria condivisa   
    if(( shm_id =  shmget(key, shm_size, 0666 | IPC_CREAT| IPC_EXCL)) < 0){
        perror("PADRE: Shared memory creation error");
        exit(1);
    }

    // Collegamento zona di memoria condivisa con la memoria del processo
    if((shm_address = shmat(shm_id, NULL, 0)) == (void*) -1){
        perror("PADRE: Shared memory attachment error");
        exit(1);
    }

    return shm_address;

}

void detach_segments(int shm_size, int key, void *shm_address){

    shmdt(shm_address);

    int shm_id;
    // Creazione della zona di memoria condivisa   
    if(( shm_id =  shmget(key, shm_size, 0666)) < 0){
        perror("PADRE: Shared memory creation error");
        exit(1);
    }
 
     if(shmctl(shm_id, IPC_RMID, &shmid_struct) == -1){
            perror("PADRE: Deallocation shared memory error");
            exit(1);
    }

}

int load_file(char* shm_write, int file_descriptor){

    /*
        character: per la lettura un carattere alla volta del file di input
        read_line: per la lettura del file
        counter: conta il numero di righe nel file (corrisponde al numero di chiavi) -> inizializzato a 0
        position: puntatore del file in lettura -> inizializzato al primo carattere del file

    */

    char character;
    int read_line;
    int counter = 0;
    int position = lseek(file_descriptor, 0L, SEEK_SET);

    // scorrimento del file di input un carattere alla volta
    while((read_line = read(file_descriptor, &character, 1)) == 1){
        // controllo il carattere letto per l'incremento del contatore delle righe del file
        if(character == '\n'){
            // copia del carattere nella zona di memoria
            *shm_write++ = character;
            // incremento del numero di righe
            counter++;
        }
        else{
            // copia del carattere nella zona di memoria
            *shm_write++ = character;
        }
    }
    // inserimento di un carattere per il riconoscimento della fine del file
    *shm_write = '\0';

    close(file_descriptor);

    return counter;

}

void check_keys(char *shm_address1, unsigned * shm_address2){

    char* shm_read;

    char clear[512];
    char encoded[512];
    shm_read = shm_address1;

    for(int i = 0; i < lines; i++){
        int j = 0;
        for(shm_read = (shm_read + 1); *shm_read != '>'; shm_read++, j++){
            clear[j] = *shm_read;
        }
        int size = j / 4;
        j = 0;
        for(shm_read = (shm_read + 3); *shm_read != '>'; shm_read++, j++){
            encoded[j] = *shm_read;
        }

        unsigned key = shm_address2[i];
        unsigned *unsigned_clear = (unsigned *) clear;
        unsigned *unsigned_encoded = (unsigned *) encoded;
        int check = 0;
        for(j = 0; j < size ; unsigned_clear++, unsigned_encoded++, j++){
            if((*unsigned_clear ^ key) == *unsigned_encoded){
                check++;
            }
        }   
        shm_read = shm_read + 2;
    }


}

void save_keys(unsigned* shm_address, int file_descriptor){

    unsigned* shm_read;
    int i = 0;
    for(shm_read = shm_address; i < lines; shm_read++, i++){

        char *hexa = from_unsigned_to_hexa(*shm_read);
        char *finale = concat_string("0x", hexa);
        finale = concat_string(finale, "\n");
        write(file_descriptor, finale, string_length(finale));

    }

    //PUO ESSERE SMEPLIFICATO

    close(file_descriptor);

}
