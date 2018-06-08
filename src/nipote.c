#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <types.h>
#include <nipote.h>
#include <utilities.h>
#include <constants.h>

#include <pthread.h>

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

void *nipote(void *params){

    struct Params *prm = (struct Params*) params;

    s1 = prm->s1;
    s2 = prm->s2;
    sem_id = prm->sem;

    /**
     * my_string: identifica la chiave che il processo nipote sta cercando
     * shm_id*: id delle zone di memoria
     */
    int my_string;

    if((msq_id = msgget(KEY_MSG, 0666)) < 0){
        perror("Message queue access error");
        exit(1);
    }

    lock(0);

    // my_string = status->id_string;

     while((my_string = status->id_string) < prm->line)   {

        
            // my_string = status->id_string;

            status->grandson = prm->id;
            status->id_string = status->id_string + 1;
            
            #if CONDITION != 1

                // PARTE CON I NIPOTI            

                    kill(getppid(), SIGUSR1);

                    lock(1);

                // FINE PARTE CON I NIPOTI

            #else

                // PARTE CON I THREAD

                kill(getpid(), SIGUSR1);
                lock(1);

                // FINE PARTE CON I THREAD

             #endif
         
            
            unlock(0);

            char *current_line = load_string(prm->line, my_string);
            unsigned key = find_key(current_line);
            save_key(key, my_string);

     }
     unlock(0);

}

void lock(int sem_num){

    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = -1;
    op.sem_flg = 0;

    // if (semop(sem_id, &op, 1) == -1) {  
    //     perror("Semaphore lock operation error");
    //     exit(1);
    // }
    while(semop(sem_id, &op, 1) == -1){}

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
    for(j = 0; current_line != my_string ; j++) {
             if(read[j] == '>'){
                current_line++;                 
                read = read + j + j + 4;
                j = 0;                      
            }
    }


    return read;
}

unsigned find_key(char *read){
    char clear[512];
    char encoded[512];
    int i;
    int position = 0;
    for(i = 1; read[i] != '>'; i++, position++){
        clear[position] = read[i];
    }
    int j = 0;
    for(i = i + 3; j < position; i++, j++){
        encoded[j] = read[i];
    }

    unsigned key = 0;
    unsigned *unsigned_clear = (unsigned *) clear;
    unsigned *unsigned_encoded = (unsigned *) encoded;
    time_t inizio = current_timestamp();
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
    struct timeval timer;
    gettimeofday(&timer, NULL);
    return timer.tv_sec; 
}

void send_timeelapsed(int time) {

    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    char *tempo = from_int_to_string(time);
    char *messaggio = concat_string("chiave trovata in ", tempo);
    copy_string(Msg.text, messaggio);
    Msg.mtype = 2;
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        perror("Message queue sending error");
        exit(1);
    }

    free(tempo);
    // tempo = NULL;
    free(messaggio);
    // my_free(messaggio);
    // for (int i = 0; i < string_length(tempo); i++) {
    //     tempo[i] = '\0';
    // }

    
    // my_free(tempo);
    // messaggio = NULL;
    // tempo = NULL;

}