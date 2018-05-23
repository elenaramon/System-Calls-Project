#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "../include/padre.h"
#include "../include/logger.h"
#include "../include/types.h"
#include "../include/figlio.h"
#include "../include/utilities.h"

#define KEY_SHM1 75
#define KEY_SHM2 76
#define SIZE 512


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
    int shm_id1, shm_id2;
    int line_counter;


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
    line_counter = load_file(shm_write1, file_descriptor_input);

    // Calcolo dimensione seconda zona di memoria condivisa
    shm_size2 = sizeof(char) * 11 * line_counter;

    // Creazione e collegamento della seconda zona di memoria condivisa
    void *s2 = attach_segments(KEY_SHM2, shm_size2);

    // Creazione del figlio logger
    if((son_logger = fork()) == -1){
        perror("PADRE: Logger son creation error");
        exit(1);
    }

    // Creazione del figlio figlio
    if((son_figlio = fork()) == -1){
        perror("PADRE: Figlio son creation error");
        exit(1);
    }
    if(son_logger == 0 && son_figlio == 0){
        // Figlio di nessuno
        exit(0);
    }
    else if(son_logger == 0 && son_figlio != 0){
        // Esecuzione di logger

        logger();
       
    }
    else if(son_logger != 0 && son_figlio == 0){
        // Esecuzione di figlio

        figlio(shm_size1, line_counter);

    }
    else{
        // Esecuzione del padre

        // Attende la terminazione dei figli
        wait(NULL);
        wait(NULL);

        // Salva le chiavi nel file di output
        save_keys(s2, file_descriptor_output);


        // int correct_keys = check_keys(shm_address1, shm_address2, line_counter);
        // if(correct_keys == 1){
        //     printf("PADRE: Le chiavi trovate sono corrette\n");
        //     save_keys(shm_address2, line_counter, file_descriptor2);
        // }
        // else{
        //     printf("PADRE: Una o più chiavi trovate non sono corrette\n");
        // }

        // Eliminazione dei segmenti di memoria condivisa
        struct shmid_ds shm_struct;
        detach_segments(shm_struct, shm_size1, KEY_SHM1);
        detach_segments(shm_struct, shm_size2, KEY_SHM2);

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

void detach_segments(struct shmid_ds shmid_struct, int shm_size, int key){

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


void save_keys(char* shm_address, int file_descriptor){

    char* shm_read;

    for(shm_read = shm_address; *shm_read != '\0'; shm_read++){

        if(*shm_read != '\n'){
            unsigned int numero = (unsigned int) *shm_read;
            unsigned int val;
            char stringa[2];
            int i;

            for(i = 0; numero != 0; i++){

                val = numero % 16;
                numero = numero / 16;
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
                    case 10:
                        stringa[i] = 'A';
                        break;
                    case 11:
                        stringa[i] = 'B';
                        break;
                    case 12:
                        stringa[i] = 'C';
                        break;
                    case 13:
                        stringa[i] = 'D';
                        break;
                    case 14:
                        stringa[i] = 'E';
                        break;
                    case 15:
                        stringa[i] = 'F';
                        break;          
                    default:
                        stringa[i] = '0';
                        break;
                }
            }
            char conv[2];
            int a;
            for(a = 0; i > 0; i--, a++){
                conv[i - 1] = stringa[a];
            }
            if(write(file_descriptor, conv, 2) == -1){
                perror("Write error");
                exit(1);
            }
        }
        else{        
            if(write(file_descriptor, shm_read, 1) == -1){
                perror("Write error");
                exit(1);
            }
        }
        
    }

    close(file_descriptor);

}
