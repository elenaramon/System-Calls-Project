#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <logger.h>
#include <types.h>
#include <utilities.h>
#include <constants.h>

/** buf struttura per la gestione delle code di messaggi
 *  msq_id identificativo della coda di messaggi
 */
struct msqid_ds buf;
int msq_id;

void sig_alrm(int sig){

    // controllo che il segnale ricevuto sia SIGALARM
    if(sig == SIGALRM){
        return; 
    }

}

/*
int main(int argc, char *argv[]){
    if(argc != 2){
        printing("Manca chiave");
        exit(0);
    }

	logger(atoi(argv[1]));
	exit(0);
}
*/


void logger(){

    /**
     * end_signal segnale per terminare la lettura della coda
     */
    int end_signal = 0;

    // creazione della coda di messaggi
    if((msq_id = msgget(KEY_MSG, (0666 | IPC_CREAT))) < 0){
        check_error("LOGGER: Creazione coda di messaggi");
    }

    // il processo logger si registra per catturare l'allarme
    signal(SIGALRM, sig_alrm);

    // ciclo per la lettura della coda di messaggi, continua fino a che non viene letto un messaggio di tipo 1
    while(end_signal != 1){

            // viene impostato un allarme a 1 secondo per il polling
            alarm(1);
            // il processo attende di ricevere il segnale di alarm
            pause();
            // l'allarme viene messo a 0 per evitare che venga visualizzato un messaggio di errore
            alarm(0);

            // richiamo della funzione per la lettura della coda
            end_signal = polling_receive(msq_id);
        }

    // la coda viene eliminata
    if((msgctl(msq_id, IPC_RMID, &buf)) == -1){
        check_error("LOGGER: Deallocazione coda di messaggi");
    }

    exit(0);

}

int polling_receive(int msq_id){

    /**
     * Msg struttura per i messaggi
     * size dimensione del messaggio
     * controll per la verifica della lettura di un messaggio di tipo 1
     */
    struct Message Msg;
    int size;
    int controll = 0;
    int read_message;
    size = sizeof(Msg) - sizeof(long);
    // viene letto un messaggio dalla coda FIFO
    while ((read_message = msgrcv(msq_id, &Msg, size, 0, IPC_NOWAIT)) > 0) {
        // controllo del tipo di messaggio
        if (Msg.mtype == 1) {
            controll = 1;
        }
        // il messagio viene stampato su stdout
        printing(Msg.text);
    }
    if(read_message == -1 && errno != ENOMSG){
        check_error("LOGGER: Lettura dalla coda di messaggi");
    }

    return controll;
      
}
