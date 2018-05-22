#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "../include/types.h"
#include "../include/nipote.h"

#define KEY_SEM 78
#define KEY_SHM 75
#define KEY_SHM2 76


char* load_string(int line, int my_string);
void lock();
void unlock();
char* find_key();
void send_timeelapsed();
// int save_key();    -> ritorna se il salvataggio è andato a buon fine
void save_key(char * key, int my_string);

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
} sem_arg;

int sem_id;

struct Status *status;

void *s1;

void *s2;

void nipote(int shm_size, int line){

    int my_string;
    int shm_id;
    int shm_id2;

    if((sem_id = semget(KEY_SEM, 0, (0666 | IPC_CREAT))) < 0){
        printf("NIPOTE: %i", getpid());
        perror("Semaphore creation error");
        exit(1);
    }

    if((shm_id = shmget(KEY_SHM, shm_size, 0666 | IPC_CREAT)) < 0){
        printf("NIPOTE: %i", getpid());
        perror("Shared memory access error");
        exit(1);
    }

    if((s1 = shmat(shm_id, NULL, 0)) == (void*) -1){
        printf("NIPOTE: %i", getpid());
        perror("Shared memory attachement error");
        exit(1);
    }

    int size2 = sizeof(int) * line + line;

    if((shm_id2 = shmget(KEY_SHM2, size2, 0666 | IPC_CREAT)) < 0){
        printf("NIPOTE: %i", getpid());
        perror("Shared memory access error");
        exit(1);
    }

    if((s2 = shmat(shm_id2, NULL, 0)) == (void*) -1){
        printf("NIPOTE: %i", getpid());
        perror("Shared memory attachement error");
        exit(1);
    }

    status = (struct Status*) s1;

    // Prova ad accedere al semaforo
    /* // AREA DEBUG
        printf("NIPOTE %i: Provo ad accedere al semaforo\n", getpid());
     */


     while(1)   {
        lock();

        my_string = status->id_string;

        if(my_string != line){
            status->grandson = getpid();
            status->id_string = status->id_string + 1;
            kill(getppid(), SIGUSR1);
            sleep(1);
            
            unlock();

            char* stringa = load_string(line, my_string);

            printf("%s\n", stringa);

            char* key = (char*) malloc(sizeof(int));

            // char* key = find_key();
            int i = 0;
            switch(i){
                case 0:
                    key = "zero";
                    break;
                case 1:
                    key = "nove";
                    break;
                case 2:
                    key = "otto";
                    break;
                case 3:
                    key = "sette";
                    break;
            }
            save_key(key, my_string);
            
        }
        else{
            unlock();
            break;
        }
     }

}

void lock(){

    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;

    if (semop(sem_id, &op, 1) == -1) {  
        printf("NIPOTE: %i", getpid());
        perror("Semaphore lock operation error");
        exit(1);
    }
    /* // AREA DEBUG
        printf("NIPOTE %i: Ho avuto accesso alla risorsa condivisa\n", getpid());
     */
}

void unlock(){

    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;          
    op.sem_flg = 0;         

    if (semop(sem_id, &op, 1) == -1) { 
        printf("NIPOTE: %i", getpid());
        perror("Semaphore unlock operation error");
        exit(1);
    }
    /* // AREA DEBUG
        printf("NIPOTE %i: Ho sbloccato la risorsa condivisa\n", getpid());
     */
}

char* load_string(int line, int my_string){

    /* // AREA DEBUG
        printf("Sto per leggere la stringa %i\n", my_string);
     */
    int count = 0;
    char *buffer = (char*) malloc(sizeof(char*) * 1030);
    int i;
    char *read = (char*)s1 + sizeof(struct Status);

    for(read = (char*)s1 + sizeof(struct Status); *read != '\0'; read++){

        if(count == my_string){
            break;
        }
        else{
            if(*read == '\n'){
                count++;
            }
        }
    }
    for(i = 0; *read != '\n' && *read != '\0'; i++, read++){
        buffer[i] = *read;
    }  
    return buffer;

}

void save_key(char* key, int my_string){

    

}