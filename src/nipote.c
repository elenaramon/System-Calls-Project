#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "../include/types.h"
#include "../include/nipote.h"
#include "../include/utilities.h"
#include "../include/costanti.h"

/**
 * sem_id: identificatore del semaforo
 * *status: puntatore alla struttura status
 * *s1: puntatore al segmento 1 di memoria condivisa
 * *s2: puntatore al segmento 2 di memoria condivisa
 * msq_id: identificatore della coda di messaggi
 */
int sem_id;
struct Status *status;
void *s1;
void *s2;
int msq_id;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
};

void nipote(int line, int id, void *shm1, void *shm2, int sem){

    s1 = shm1;
    s2 = shm2;
    sem_id = sem;

    /**
     * my_string: identifica la chiave che il processo nipote sta cercando
     * shm_id*: id delle zone di memoria
     */
    int my_string;

    if((msq_id = msgget(KEY_MSG, 0666| IPC_CREAT)) < 0){
        perror("Message queue access error");
        exit(1);
    }

     while(1)   {

        lock(0);

        my_string = status->id_string;

        if(my_string != line){
            status->grandson = id;
            status->id_string = status->id_string + 1;
            
            #if CONDITION != 1

                // PARTE CON I NIPOTI            

                    kill(getppid(), SIGUSR1);

                    lock(1);

                // FINE PARTE CON I NIPOTI

            #else

                // PARTE CON I THREAD

                char *messaggio = concat_string("Il thread ", from_int_to_string(status->grandson));
                messaggio = concat_string(messaggio, " sta analizzando la ");
                messaggio = concat_string(messaggio, from_int_to_string(status->id_string));
                messaggio = concat_string(messaggio, "-esima stringa.");

                printing(messaggio);
                free(messaggio);

                // FINE PARTE CON I THREAD

             #endif
         
            
            unlock(0);

            char *current_line = load_string(line, my_string);
            unsigned key = find_key(current_line);
            save_key(key, my_string);

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
        perror("Semaphore lock operation error");
        exit(1);
    }

}

void unlock(int sem_num){

    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = 1;          
    op.sem_flg = 0;         

    if (semop(sem_id, &op, 1) == -1) { 
        perror("Semaphore unlock operation error");
        exit(1);
    }
    
}

char* load_string(int line, int my_string){

    int current_line = 0;
    char *read = (char*)(s1 + sizeof(struct Status));
    int j;
    for(j = 0; read[j] != '\0'; j++) {
        if(current_line == my_string){
            break;
        }
        else{
            if(read[j] == '\n'){
                current_line++;
            }
        }
    }
    
    read = read + j;

    return read;
}

unsigned find_key(char *read){
    char clear[512];
    char encoded[512];

    int count = 0;
    for(; read[count] != '>'; count++);

    int i;
    int position = 0;
    for(i = 1; i < count; i++, position++){
        clear[position] = read[i];
    }

    position = 0;
    for(i = i + 3; read[i] != '>'; i++, position++){
        encoded[position] = read[i];
    }

    time_t inizio = current_timestamp();
    unsigned key = 0;
    unsigned *unsigned_clear = (unsigned *) clear;
    unsigned *unsigned_encoded = (unsigned *) encoded;
    while((*unsigned_clear^ key) != *unsigned_encoded){
        key++;
    }
    printf("%u\n", key);
    send_timeelapsed(current_timestamp() - inizio);

    return key;
    
}

void save_key(unsigned key, int my_string){

    unsigned *write = (unsigned *) s2;
    *(write + my_string)= key;

}

time_t current_timestamp() {     

    struct timespec ts;     
    clock_gettime(CLOCK_REALTIME, &ts);    
    return ts.tv_sec; 
}

void send_timeelapsed(int time){

    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    char *messaggio = concat_string("chiave trovata in ", from_int_to_string(time));
    copy_string(Msg.text, messaggio);
    Msg.mtype = 2;
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        perror("NIPOTE: Message queue sending error");
        exit(1);
    }
    free(messaggio);

}