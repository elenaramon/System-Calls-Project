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
#include "../include/figlio.h"
#include "../include/types.h"
#include "../include/nipote.h"

#define KEY_SHM 75
#define KEY_MSG 77
#define KEY_SEM 78

void status_update(int s);
void send_terminate(int msq_id);

struct Status* status;

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
};


void figlio(int shm_size, int line){

    int shm_id;
    void * s1;


    /* // AREA DEBUG
        printf("FIGLIO: Creo la zona di memoria condivisa\n");
     */
    if((shm_id = shmget(KEY_SHM, shm_size, 0666)) < 0){
        perror("Opening shared memory error");
        exit(1);
    }

    // attacco il segmento di memoria condivisa appena creato alla zona di memoria del processo
    /* // AREA DEBUG
        printf("FIGLIO: Attacco il segmento\n");
    */
    if((s1 = shmat(shm_id, NULL, 0)) == (void *)-1){
        perror("FIGLIO: Shared memory attachment error");
        exit(1);
    }
    status = (struct Status*) s1;

    signal(SIGUSR1, status_update);

    pid_t son_nipote1, son_nipote2;
    union semun sem_arg;
    int sem_id;

    /*
    // AREA DEBUG
        printf("FIGLIO: Creazione del semaforo\n");
    */

    if((sem_id = semget(KEY_SEM, 2, (0666 | IPC_CREAT | IPC_EXCL))) < 0){
        perror("FIGLIO: Semaphore creation error");
        exit(1);
    }

    // inizializzazione di semafori a 1 : MUTEX
   /*  // AREA DEBUG
        printf("FIGLIO: Settiamo il semaforo a 1, mutua esclusione\n");
     */
    sem_arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {
        perror ("Semaforo PADRE non inizializzato.\n");
        exit(1);
    }    
    if (semctl(sem_id, 1, SETVAL, sem_arg) == -1) {
        perror ("Semaforo PADRE non inizializzato.\n");
        exit(1);
    }  
        

    /* 
    // AREA DEBUG
      printf("FIGLIO: Accesso alla coda di messagi\n");
    */

    int msq_id;
    if((msq_id = msgget(KEY_MSG, 0666| IPC_CREAT)) < 0){
        perror("FIGLIO: Message queue access error");
        exit(1);
    }


    /* 
    // AREA DEBUG
        printf("FIGLIO: ID coda: %i\n", msq_id);
     */


    if((son_nipote1 = fork()) == -1){
        perror("FIGLIO: Nipote1 creation error");
        exit(1);
    }

    if((son_nipote2 = fork()) == -1){
        perror("FIGLIO: Nipote2 creation error");
        exit(1);
    }

    if(son_nipote1 == 0 & son_nipote2 == 0){
        // figlio di nessuno
        exit(0);
    }
    else if(son_nipote1 == 0 && son_nipote2 != 0){
        
        // esecuzione son_nipote1
        /* // AREA DEBUG
            printf("FIGLIO:  Ciao sono il figlio nipote1 %i, mio padre è %i\n", getpid(), getppid());
         */
        nipote(shm_size, line);
        
        exit(0);
    }
    else if(son_nipote1 != 0 && son_nipote2 == 0){
        
        // esecuzione son_nipote2
        /* // AREA DEBUG
            printf("FIGLIO:  Ciao sono il figlio nipote2 %i, mio padre è %i\n", getpid(), getppid());
         */
        nipote(shm_size, line);
        
        exit(0);
    }
    else{
        /* 
        // esecuzione del padre
        // AREA DEBUG
            printf("FIGLIO: Ciao sono il padre %i, mio padre è %i\n", getpid(), getppid());
         */
        wait(NULL);
        wait(NULL);
        /* 
        // AREA DEBUG
            printf("FIGLIO: I figli sono terminati mando il messaggio a logger\n");
        */
            
        send_terminate(msq_id);
        /*
        // AREA DEBUG
            printf("FIGLIO: Dormo 5 secondi\n");

        sleep(5);
        // AREA DEBUG
            printf("FIGLIO: Mi sono svegliato\n");

        // AREA DEBUG
            printf("FIGLIO: Rimuovo il semaforo\n");
        */

        sem_arg.val = 0;
        if(semctl(sem_id, 0, IPC_RMID, sem_arg) == -1){
            perror("FIGLIO: Remove semaphore error");
            exit(1);
        }    
        /* 
        // AREA DEBUG
            printf("FIGLIO: Semaforo rimosso, termino\n")   ;
        */
    }


}

void status_update(int s){

    if(s == SIGUSR1){

        printf("FIGLIO: grandson %i - ", status->grandson);
        printf("FIGLIO: id_string %i\n", status->id_string);

        // write(1, "HELLO\n", 7);
    }

}

void send_terminate(int msq_id){

    /* // AREA DEBUG
        printf("FIGLIO: Invio il messaggio a logger\n");
    */

    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    // SE CAMBI MESSGGIO CAMBIA ANCHE LA DIMENSIONE DELLA WRITE IN LOGGER
    strcpy(Msg.text, "ricerca conclusa\0");
    Msg.mtype = 1;
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        perror("FIGLIO: Message queue sending error");
        exit(1);
    }

    /* // AREA DEBUG
        printf("FIGLIO: Messaggio inviato\n");
    */
}