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
#include <time.h>
#include "../include/types.h"
#include "../include/nipote.h"
#include "../include/utilities.h"
#include "../include/costanti.h"


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

void nipote(int shm_size, int line, int id){

    int my_string;
    int shm_id;
    int shm_id2;
    union semun sem_arg;

    if((msq_id = msgget(KEY_MSG, 0666| IPC_CREAT)) < 0){
        perror("Message queue access error");
        exit(1);
    }

    if((sem_id = semget(KEY_SEM, 0, (0666 | IPC_CREAT))) < 0){
        perror("Semaphore creation error");
        exit(1);
    }

    if((shm_id = shmget(KEY_SHM1, shm_size, 0666 | IPC_CREAT)) < 0){
        perror("Shared memory access error");
        exit(1);
    }

    if((s1 = shmat(shm_id, NULL, 0)) == (void*) -1){
        perror("Shared memory attachement error");
        exit(1);
    }

    int size2 = sizeof(int) * line + line;

    if((shm_id2 = shmget(KEY_SHM2, size2, 0666 | IPC_CREAT)) < 0){
        perror("Shared memory access error");
        exit(1);
    }

    if((s2 = shmat(shm_id2, NULL, 0)) == (void*) -1){
        perror("Shared memory attachement error");
        exit(1);
    }


     while(1)   {
        lock(0);

        my_string = status->id_string;

        if(my_string != line){
            status->grandson = id;
            status->id_string = status->id_string + 1;

            kill(getppid(), SIGUSR1);
            lock(1);
            
            unlock(0);

            load_string(line, my_string);
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

void load_string(int line, int my_string){

    char clear[512];
    char encoded[512];
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
    
    int count = j;
    for(; read[count] != '>'; count++);

    int i;
    int position = 0;
    for(i = j + 1; i < count; i++, position++){
        clear[position] = read[i];
    }

    position = 0;
    for(i = i + 3; read[i] != '>'; i++, position++){
        encoded[position] = read[i];
    }
    
    find_key(clear, encoded, my_string);

}

void find_key(char *clear, char *encoded, int my_string){
    time_t inizio = current_timestamp();
    unsigned key = 0;
    unsigned *unsigned_clear = (unsigned *) clear;
    unsigned *unsigned_encoded = (unsigned *) encoded;
    while((*unsigned_clear^ key) != *unsigned_encoded){
        key++;
    }

    send_timeelapsed(current_timestamp() - inizio);

    save_key(key, my_string);
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

}