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

#define KEY_SHM 75
#define KEY_SHM2 76
#define KEY_MSQ 77
#define KEY_SEM 78


char* load_string(int line, int my_string);
void lock(int sem_num);
void unlock(int sem_num);
char* find_key();
void send_timeelapsed();
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
int msq_id;

void nipote(int shm_size, int line){

    int my_string;
    int shm_id;
    int shm_id2;

    if((msq_id = msgget(KEY_MSQ, 0666| IPC_CREAT)) < 0){
        printf("NIPOTE: %i", getpid());
        perror("Message queue access error");
        exit(1);
    }

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
        lock(0);

        my_string = status->id_string;

        if(my_string != line){
            status->grandson = getpid();
            status->id_string = status->id_string + 1;
            kill(getppid(), SIGUSR1);
            sleep(1);
            
            unlock(0);

            char* stringa = load_string(line, my_string);

            char* key = (char*) malloc(sizeof(int));

            // char* key = find_key();

            switch(my_string){
                case 0:
                    key = "abcd";
                    break;
                case 1:
                    key = "efgh";
                    break;
                case 2:
                    key = "iklm";
                    break;
                case 3:
                    key = "nopq";
                    break;
            }

            lock(1);
            save_key(key, my_string);
            unlock(1);
            
            send_timeelapsed();
            
        }
        else{
            unlock(0);
            break;
        }
     }

}

void lock(int sem_num){

    struct sembuf op;

    op.sem_num = sem_num;
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

void unlock(int sem_num){

    struct sembuf op;
    op.sem_num = sem_num;
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
    char *read;
    //  = (char*)s1 + sizeof(struct Status);

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

    char *write;
    int count = 0;
    //  = (char*) s2;
    for(write = (char*)s2; *write != '\0'; write++){
        if(count == my_string){
            break;
        }
        else
        {
            if(*write == '\n'){
                count++;
            }
        }
    }
    for( ; *key != '\0'; write++, key++){
        *write = *key;
    }
    *write++ = '\n';

}

void send_timeelapsed(){

    /* // AREA DEBUG
    printf("NIPOTE %i: Invio il messaggio\n", getpid());
     */
    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    // SE CAMBI MESSGGIO CAMBIA ANCHE LA DIMENSIONE DELLA WRITE IN LOGGER
    strcpy(Msg.text, "chiave trovata in BLA BLA\0");
    Msg.mtype = 2;
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        perror("FIGLIO: Message queue sending error");
        exit(1);
    }

}