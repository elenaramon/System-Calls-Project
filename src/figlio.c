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
#include "../include/figlio.h"
#include "../include/types.h"
#include "../include/nipote.h"
#include "../include/utilities.h"

#define KEY_SHM 75
#define KEY_MSG 77
#define KEY_SEM 78

struct Status* status;
int msq_id;

void figlio(int shm_size, int line){

    int shm_id;
    void * s1;
    pid_t son_nipote1, son_nipote2;
    union semun sem_arg;
    int sem_id;

    // Accedo alla zona di memoria
    if((shm_id = shmget(KEY_SHM, shm_size, 0666)) < 0){
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
    if((sem_id = semget(KEY_SEM, 2, (0666 | IPC_CREAT | IPC_EXCL))) < 0){
        perror("FIGLIO: Semaphore creation error");
        exit(1);
    }

    // Inizializzo i semafori a 1
    sem_arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {
        perror ("FIGLIO: Semaphore initialization");
        exit(1);
    }    
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
        nipote(shm_size, line);
        
        exit(0);
    }
    else if(son_nipote1 != 0 && son_nipote2 == 0){
        // Esecuzione nipote 2
        nipote(shm_size, line);
        
        exit(0);
    }
    else{
        // Esecuzione del padre

        // Attende la terminazione dei figli
        wait(NULL);
        wait(NULL);
 
        // Manda un messaggio di terminazione al processo logger
        send_terminate(msq_id);

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

        char *messaggio_pt1 = concat_string("Il nipote ", from_int_to_string(status->grandson));
        char *messaggio_pt2 = concat_string(messaggio_pt1, " sta analizzando la ");
        char *messaggio_pt3 = concat_string(messaggio_pt2, from_int_to_string(status->id_string));
        char *messaggio_pt4 = concat_string(messaggio_pt3, "-esima stringa.");

        // printf("FIGLIO: grandson %i - ", status->grandson);
        // printf("FIGLIO: id_string %i\n", status->id_string);

        int size = string_length(messaggio_pt4);

        write(1, messaggio_pt4, size);
    }

}

void send_terminate(){


    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    // SE CAMBI MESSGGIO CAMBIA ANCHE LA DIMENSIONE DELLA WRITE IN LOGGER
    char *messaggio = "ricerca conclusa";
    copy_string(Msg.text, messaggio);
    // strcpy(Msg.text, "ricerca conclusa\0");
    Msg.mtype = 1;
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        perror("FIGLIO: Message queue sending error");
        exit(1);
    }

}