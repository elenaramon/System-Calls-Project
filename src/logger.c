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


    sleep(1);


    end_signal = polling_receive(msq_id);


    if(end_signal == 1){

        if((msgctl(msq_id, IPC_RMID, &buf)) == -1){
            perror("LOGGER: Deallocation message queue error");
            exit(1);
        }
    }

}


int polling_receive(int msq_id){

    struct Message Msg;
    int size;
    int message_size;
    int controll = 0;

    do{
        size = sizeof(Msg) - sizeof(long);
        if(msgrcv(msq_id, &Msg, size, 0, 0) == -1){
            perror("Error");
            exit(1);
        }

        message_size = sizeof(Msg.text);
        printing(Msg.text);

        if(Msg.mtype == 1){

            controll = 1;
        }


        if(msgctl(msq_id, IPC_STAT, &buf) == -1){
            perror("LOGGER: Errore msgctl");
            exit(1);
        }
        
    }while((controll == 1 && ((int)buf.msg_qnum) > 0) || (controll == 0));

    return controll;
      
}