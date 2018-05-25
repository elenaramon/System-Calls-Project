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
#include "../include/figlio.h"
#include "../include/types.h"
#include "../include/nipote.h"
#include "../include/utilities.h"
#include "../include/costanti.h"

struct Status* status;
int msq_id;
int sem_id;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
};

void figlio(int shm_size, int line){

    int shm_id;
    void * s1;
    pid_t son_nipote1, son_nipote2;
    union semun sem_arg;
    int id;

    // Accedo alla zona di memoria
    if((shm_id = shmget(KEY_SHM1, shm_size, 0666)) < 0){
        perror("Opening shared memory error");
        exit(1);
    }
    // Mi attacco alla zona di memoria
    if((s1 = shmat(shm_id, NULL, 0)) == (void *)-1){
        perror("FIGLIO: Shared memory attachment error");
        exit(1);
    }
    status = (struct Status*) s1;

    // Mi registro per ricevere il segnale SIGUSR1
    signal(SIGUSR1, status_update);

    // Creo i semafori di mutua esclusione
    if((sem_id = semget(KEY_SEM, 3, (0666 | IPC_CREAT | IPC_EXCL))) < 0){
        perror("FIGLIO: Semaphore creation error");
        exit(1);
    }

    // Inizializzo il semaforo 0 a 1
    sem_arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {
        perror ("FIGLIO: Semaphore initialization");
        exit(1);
    }    
    // Inizializzo il semaforo 1 a 0
    sem_arg.val = 0;
    if (semctl(sem_id, 1, SETVAL, sem_arg) == -1) {
        perror ("FIGLIO: Semaphore initialization");
        exit(1);
    }   

    // Accedo alla coda di messaggi
    if((msq_id = msgget(KEY_MSG, 0666| IPC_CREAT)) < 0){
        perror("FIGLIO: Message queue access error");
        exit(1);
    }

    // Creo il nipote 1
    if((son_nipote1 = fork()) == -1){
        perror("FIGLIO: Nipote1 creation error");
        exit(1);
    }

    // Creo il nipote 2
    if((son_nipote2 = fork()) == -1){
        perror("FIGLIO: Nipote2 creation error");
        exit(1);
    }

    if(son_nipote1 == 0 & son_nipote2 == 0){
        // figlio di nessuno
        exit(0);
    }
    else if(son_nipote1 == 0 && son_nipote2 != 0){
        // Esecuzione nipote 1
        id = 1;
        nipote(shm_size, line, id);
        

    }
    else if(son_nipote1 != 0 && son_nipote2 == 0){
        // Esecuzione nipote 2
        id = 2;
        nipote(shm_size, line, id);
        

    }
    else{
        // Esecuzione del padre

        // Attende la terminazione dei figli
        wait(&son_nipote1);
        wait(&son_nipote2);
 
        // Manda un messaggio di terminazione al processo logger
        send_terminate();

        // Elimina i semafori
        sem_arg.val = 0;
        if(semctl(sem_id, 0, IPC_RMID, sem_arg) == -1){
            perror("FIGLIO: Remove semaphore error");
            exit(1);
        }  
    }
}

void status_update(int s){


    if(s == SIGUSR1){

        char *messaggio = concat_string("Il nipote ", from_int_to_string(status->grandson));
        messaggio = concat_string(messaggio, " sta analizzando la ");
        messaggio = concat_string(messaggio, from_int_to_string(status->id_string));
        messaggio = concat_string(messaggio, "-esima stringa.");

        int size = string_length(messaggio);

        printing(messaggio);
    }
    unlock(1);

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