#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "../include/logger.h"
#include "../include/types.h"
#include "../include/utilities.h"
#include "../include/costanti.h"

/**
 * *status: puntatore alla struttura status
 * buf: struttura per la gestione delle code di messaggi definita in <sys/msg.h>
 */
struct Status *status;
struct msqid_ds buf;

void logger(){

    /*
        msg_id: identificatore della coda di messaggi
        end_signal: usato per la terminazione del processo e la chiusura della coda di messaggi
    */
    int msq_id;
    int end_signal = 0;

    if((msq_id = msgget(KEY_MSG, (0666 | IPC_CREAT))) < 0){
        perror("LOGGER: Message queue creation error");
        exit(1);
    }

    while(end_signal != 1){
        sleep(1);
        end_signal = polling_receive(msq_id);
    }

    if((msgctl(msq_id, IPC_RMID, &buf)) == -1){
        perror("LOGGER: Deallocation message queue error");
        exit(1);
    }

}


int polling_receive(int msq_id){
    struct Message Msg;
    int size;
    int message_size;
    int controll = 0;

    size = sizeof(Msg) - sizeof(long);
    while ((msgrcv(msq_id, &Msg, size, 0, IPC_NOWAIT)) > 0) {
        if (Msg.mtype == 1) {
            controll = 1;
        }
        printing(Msg.text);
    }

    return controll;
      
}