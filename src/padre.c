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
#include "../include/logger.h"
#include "../include/types.h"
#include "../include/figlio.h"

#define KEY_SHM1 75
#define KEY_SHM2 76
#define SIZE 512

void* attach_segments(int key, int shm_size);
void detach_segments(struct shmid_ds shmid_struct, int shm_size, int key);
int load_file(char* shm_write, int file_descriptor_input);
// int check_keys(char *shm_address1, char * shm_address2, int counter);
void save_keys(char *shm_address, int file_descriptor_output);

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


    // apertura del file di lettura input
    /* // AREA DEBUG
        printf("PADRE: Apertura del file %s\n", file_name_input);
    */
    if((file_descriptor_input = open(file_name_input, O_RDONLY, 0777)) == -1){
        perror("PADRE: File descriptor open error");
        exit(1);
    }

    // apertura del file di scrittura output
    /* // AREA DEBUG
        printf("PADRE: Apertura del file %s\n", file_name_output);
    */
    if((file_descriptor_output = open(file_name_output, O_WRONLY | O_CREAT, 0777)) == -1){
        perror("PADRE: File descriptor 2 open error");
        exit(1);
    }


    // calcolo dimensione zona di memoria condivisa
    shm_size1 = sizeof(struct Status) + lseek(file_descriptor_input, 0L, SEEK_END);

    void *s1 =attach_segments(KEY_SHM1, shm_size1);
    struct Status* status = (struct Status*) s1;
    status->id_string = 0;
    status->grandson = 0;

    // // creazione della zona di memoria condivisa
    // shm_id1 = attach_segments(KEY_SHM1, shm_size1);
    // if(shm_id1 < 0){
    //     perror("PADRE: Shared memory 1 creation error");
    //     exit(1);
    // }

    // // attacco il segmento di memoria condivisa appena creato alla zona di memoria del processo
    // /*AREA DEBUG
    //     printf("PADRE: Attacco il segmento\n");
    // */
    // if((shm_address1 = shmat(shm_id1, NULL, 0)) == (void*) -1){
    //     perror("PADRE: Shared memory attachment error");
    //     exit(1);
    // }


    // assegnamento primo indirizzo in scrittura sulla memoria condivisa alla variabile di scrittura
    /* // AREA DEBUG
        printf("PADRE: Assegno l'indirizzo\n");
        */



//CAMBIAMENTO NELLA POSIZIONE DEL FILE: togliere + sizeof(..)

    char *shm_write1 = (char*)s1 + sizeof(struct Status);
    line_counter = load_file(shm_write1, file_descriptor_input);

    // AREA DEBUG
        printf("Il file ha %i righe\n", line_counter);
   

    // dichiarazione della dimensionde della zona di memoria condivisa per la scrittura delle chiavi
    shm_size2 = sizeof(int) * line_counter + line_counter;

    void *s2 =attach_segments(KEY_SHM2, shm_size2);


    // // creazione della zona di memoria condivisa
    // shm_id2 = attach_segments(KEY_SHM2, shm_size2);
    // if(shm_id2 < 0){
    //     perror("PADRE: Shared memory 2 creation error");
    //     exit(1);
    // }    


    // // attacco la zona di memoria condivisa alla zona di memoria del processo
    // /*AREA DEBUG
    //     printf("PADRE: Attacco il segmento 2\n");
    // */
    // if((shm_address2 = shmat(shm_id2, NULL, 0)) == (void *) -1){
    //     perror("PADRE: Shared memory 2 attachment error");
    //     exit(1);
    // }


    if((son_logger = fork()) == -1){
        perror("PADRE: Logger son creation error");
        exit(1);
    }

    if((son_figlio = fork()) == -1){
        perror("PADRE: Figlio son creation error");
        exit(1);
    }
    if(son_logger == 0 && son_figlio == 0){
        // figlio di nessuno
    }
    else if(son_logger == 0 && son_figlio != 0){

        // figlio logger
        /* // AREA DEBUG
            printf("PADRE: Ciao sono il figlio logger %i, mio padre è %i\n", getpid(), getppid());
            printf("PADRE: Sto per entrare nel file logger\n");
          */
        logger();
        /* // AREA DEBUG
            printf("PADRE: Logger: Sono tornato nel file padre\n");
        */
        
       
    }
    else if(son_logger != 0 && son_figlio == 0){
        // figlio figlio
        /* // AREA DEBUG
            printf("PADRE: Ciao sono il figlio figlio %i, mio padre è %i\n", getpid(), getppid());
            printf("PADRE: Sto per entrare nel file figlio\n");
        */
        figlio(shm_size1, line_counter);
        /* // AREA DEBUG
            printf("PADRE: Figlio: Sono tornato nel file padre\n");
        */
    }
    else{
        // padre
        /* // AREA DEBUG
            printf("PADRE: Ciao sono il padre %i\n", getpid());
          */

        wait(NULL);
        wait(NULL);
        //sleep(10);

        save_keys(s2, file_descriptor_output);


        // int correct_keys = check_keys(shm_address1, shm_address2, line_counter);
        // if(correct_keys == 1){
        //     printf("PADRE: Le chiavi trovate sono corrette\n");
        //     save_keys(shm_address2, line_counter, file_descriptor2);
        // }
        // else{
        //     printf("PADRE: Una o più chiavi trovate non sono corrette\n");
        // }

        struct shmid_ds shm_struct;
        detach_segments(shm_struct, shm_size1, KEY_SHM1);
        detach_segments(shm_struct, shm_size2, KEY_SHM2);

    }

}

void * attach_segments(int key, int shm_size){

    int shm_id;
    char *shm_address;

    // creazione della zona di memoria condivisa
   
    if(( shm_id =  shmget(key, shm_size, 0666 | IPC_CREAT| IPC_EXCL)) < 0){
        perror("PADRE: Shared memory creation error");
        exit(1);
    }

    // attacco il segmento di memoria condivisa appena creato alla zona di memoria del processo
    /* // AREA DEBUG
        printf("PADRE: Attacco il segmento\n");
    */
    if((shm_address = shmat(shm_id, NULL, 0)) == (void*) -1){
        perror("PADRE: Shared memory attachment error");
        exit(1);
    }

    // creo la zona di memoria condivisa
    /* // AREA DEBUG
        printf("PADRE: Creo la zona di memoria condivisa\n");
      */   
    return shm_address;

}

void detach_segments(struct shmid_ds shmid_struct, int shm_size, int key){

    int shm_id;

    if(( shm_id =  shmget(key, shm_size, 0666 | IPC_CREAT)) < 0){
        perror("PADRE: Shared memory 1 creation error");
        exit(1);
    }

    // rimozione del segmento di memoria condivisa
    /*AREA DEBUG
        printf("PADRE: Dealloco la zona di memoria condivisa\n");
    */  
     if(shmctl(shm_id, IPC_RMID, &shmid_struct) == -1){
            perror("PADRE: Deallocation shared memory 2 error");
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

    return counter;

}


void save_keys(char* shm_address, int file_descriptor){

    char* shm_read;
    char* hexa;

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

}
