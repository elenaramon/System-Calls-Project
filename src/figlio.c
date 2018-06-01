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
#include <figlio.h>
#include <types.h>
#include <nipote.h>
#include <utilities.h>
#include <constants.h>

//librearia per i thread
#include <pthread.h>

struct Status* status;
int msq_id;
int sem_id;
void *s1;
void *s2;
int line;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
};


void figlio(int lines, void *shm1, void *shm2){

    line = lines;
    s1 = shm1;
    s2 =shm2;

    union semun sem_arg;
    int id;

    status = (struct Status*) s1;

    signal(SIGUSR1, status_updated);

    if((sem_id = semget(KEY_SEM, 2, (0666 | IPC_CREAT | IPC_EXCL))) < 0){
        perror("FIGLIO: Semaphore creation error");
        exit(1);
    }

    sem_arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {
        perror ("FIGLIO: Semaphore initialization");
        exit(1);
    }    
    sem_arg.val = 0;
    if (semctl(sem_id, 1, SETVAL, sem_arg) == -1) {
        perror ("FIGLIO: Semaphore initialization");
        exit(1);
    }   

    if((msq_id = msgget(KEY_MSG, 0666| IPC_CREAT)) < 0){
        perror("FIGLIO: Message queue access error");
        exit(1);
    }


    #if CONDITION != 1

        // PARTE CON I NIPOTI

        pid_t son_nipote1, son_nipote2;
        struct Params param[2];

        if((son_nipote1 = fork()) == -1){
            perror("FIGLIO: Nipote1 creation error");
            exit(1);
        }
        else if(son_nipote1 == 0){
            param[0].s1 = s1;
            param[0].s2 = s2;
            param[0].id = 1;
            param[0].sem = sem_id;
            param[0].line = line;
            nipote((void *) &param[0]);
        }
        else{
            if((son_nipote2 = fork()) == -1){
                perror("FIGLIO: Nipote2 creation error");
                exit(1);
            }
            else if(son_nipote2 == 0){
                param[1].s1 = s1;
                param[1].s2 = s2;
                param[1].id = 2;
                param[1].sem = sem_id;
                param[1].line = line;
                nipote((void *) &param[1]);
            }
            else{
                wait(&son_nipote1);
                wait(&son_nipote2);
        
                send_terminate();

                sem_arg.val = 0;
                if(semctl(sem_id, 0, IPC_RMID, sem_arg) == -1){
                    perror("FIGLIO: Remove semaphore error");
                    exit(1);
                }  
            }
        }

        // FINE PARTE CON I NIPOTI

    #else

        // PARTE CON I THREAD

        pthread_t threads[NUM];
        int rc, t;
        for(t = 0; t < NUM; t++){

            struct Params param[NUM];
            param[t].s1 = s1;
            param[t].s2 = s2;
            param[t].id = (t + 1);
            param[t].sem = sem_id;
            param[t].line = line;

            rc = pthread_create(&threads[t], NULL, nipote, (void *) &param[t]);
            if(rc){
                perror("error\n");
                exit(1);
            }
        }
        for(t = 0; t < NUM; t++){
            pthread_join(threads[t], NULL);
        }  

        send_terminate();

        sem_arg.val = 0;
        if(semctl(sem_id, 0, IPC_RMID, sem_arg) == -1){
            perror("FIGLIO: Remove semaphore error");
            exit(1);
        }     

        // FINE PARTE CON I THREAD

    #endif


}


void status_updated(int s){

    if(s == SIGUSR1){

        char *messaggio = concat_string("Il nipote ", from_int_to_string(status->grandson));
        messaggio = concat_string(messaggio, " sta analizzando la ");
        messaggio = concat_string(messaggio, from_int_to_string(status->id_string));
        messaggio = concat_string(messaggio, "-esima stringa.");

        printing(messaggio);

        free(messaggio);
        unlock(1);
    }

}


void send_terminate(){

    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    char *messaggio = "ricerca conclusa";
    copy_string(Msg.text, messaggio);
    Msg.mtype = 1;
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        perror("FIGLIO: Message queue sending error");
        exit(1);
    }

}