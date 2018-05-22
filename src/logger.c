#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include "../include/logger.h"
#include "../include/types.h"

#define KEY_MSG 77

int polling_receive(int msq_id, struct msqid_ds buf);



struct Status *status;




void logger(){

    /*
        msg_id: identificatore della coda di messaggi
        *buf: struttura coda di messaggi
        end_signal: usato per la terminazione del processo e la chiusura della coda di messaggi
    */
    int msq_id;
    struct msqid_ds buf;
    int end_signal = 0;

    /* // AREA DEBUG
        printf("LOGGER: Creazione della coda di messaggi\n");
    */
    if((msq_id = msgget(KEY_MSG, (0666 | IPC_CREAT))) < 0){
        perror("LOGGER: Message queue creation error");
        exit(1);
    }

    /*// AREA DEBUG
        printf("LOGGER: logger %i\n", msq_id);

        printf("LOGGER: Dorme un secondo\n");
    */

    sleep(1);
    /* // AREA DEBUG
        printf("LOGGER: Si è svegliato\n");
     */


    end_signal = polling_receive(msq_id, buf);

    /* // AREA DEBUG
        sleep(5);
     */

    if(end_signal == 1){
        /* // AREA DEBUG
            printf("LOGGER: Eliminiamo la coda di messaggi\n");
        */
        if((msgctl(msq_id, IPC_RMID, &buf)) == -1){
            perror("LOGGER: Deallocation message queue error");
            exit(1);
        }
    }

    /* // AREA DEBUG
        printf("LOGGER: Coda eliminata, il processo logger termina\n");
    */
   

}


int polling_receive(int msq_id, struct msqid_ds buf){

    struct Message Msg;
    int size;
    int message_size;
    int controll = 0;

    do{
        /* // AREA DEBUG
            printf("LOGGER: Ci sono dei messaggi\n");
        */
        size = sizeof(Msg) - sizeof(long);
        msgrcv(msq_id, &Msg, size, 0, 0);

        message_size = sizeof(Msg.text);
        write(1, Msg.text, message_size);

        if(Msg.mtype == 1){
            /* // AREA DEBUG
                printf("LOGGER: Un messaggio è di fine\n");
            */
            controll = 1;
        }
        printf("\n");


        if(msgctl(msq_id, IPC_STAT, &buf) == -1){
            perror("LOGGER: Errore msgctl");
            exit(1);
        }
        /* // AREA DEBUG
        else{
            printf("LOGGER: Tutto ok, %d\n", (int)buf.msg_qnum);
        } */
        
    }while((controll == 1 && ((int)buf.msg_qnum) > 0) || (controll == 0));

    return controll;
      
}