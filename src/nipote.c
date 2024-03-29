#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <pthread.h>

#include <types.h>
#include <nipote.h>
#include <utilities.h>
#include <constants.h>
#include <errno.h>

/**
 * sem_id identificativo dell'array di semafori
 * status puntatore alla struttura status
 * s1 puntatore alla prima zona di memoria condivisa
 * s2 puntatore alla seconda zona di memoria condivisa
 * msq_id identificativo della coda di messaggi
 */
int sem_id;
struct Status *status;
void *s1;
void *s2;
int msq_id;

time_t current_timestamp() {     
    
    struct timeval timer;
    gettimeofday(&timer, NULL);
    return timer.tv_sec; 

}

void *nipote(void *params){

    // struttura dei parametri passati
    struct Params *prm = (struct Params*) params;
    // stringa analizzata
    int my_string;

    // recupero dei parametri
    s1 = prm->s1;
    s2 = prm->s2;
    sem_id = prm->sem;    

    // accesso alla coda di messaggi
    if((msq_id = msgget(KEY_MSG, 0666)) < 0){
        check_error("Accesso coda di messaggi");
    }

    // decremento del semaforo 0
    lock(0);

    // ciclo per il calcolo delle chiavi
    while((my_string = status->id_string) < prm->line)   {

            // settaggio parametri struttura status
            status->grandson = prm->id;
            status->id_string = status->id_string + 1;

            // l'identificativo del viene convertito in stringa
            char *id =  itos(status->grandson);
            // concatenazione della prima parte della stringa            
            char *messaggio = concat("Il nipote ", id);   
            // concatenazione della seconda parte della stringa
            messaggio = concat(messaggio, " sta analizzando la ");        
            // il numero di stringa in fase di analisi viene convertita in stringa
            char *string = itos(status->id_string);
            // concatenazione delle ultime parti della stringa
            messaggio = concat(messaggio, string);   
            messaggio = concat(messaggio, "-esima stringa.\n");

            int write_line;
            if((write_line = write(prm->pipe, messaggio, length(messaggio))) == -1){
                check_error("NIPOTE: Scrittura nella pipe");
            }
            
            #if CONDITION != 1

                // PARTE CON I NIPOTI            
                    
                    // invio del segnale al padre
                    kill(getppid(), SIGUSR1);

                // FINE PARTE CON I NIPOTI

            #else

                // PARTE CON I THREAD          

                    // invio del segnale al padre
                    kill(getpid(), SIGUSR1);

                // FINE PARTE CON I THREAD

            #endif

            // vengono liberate le zone di memoria
            free(id);
            free(string);
            free(messaggio);
            unlock(0);
            // caricamento della stringa da analizzare
            char *current_line = load_string(my_string);
            // ricerca della chiave
            unsigned key = find_key(current_line);
            // salvataggio della chiave
            save_key(key, my_string);

     }


    // incremento del semaforo 0 
    unlock(0);
    #if CONDITION != 1
        exit(0);
    #else
        pthread_exit(0);
    #endif
}

void lock(int sem_num){

    // struttura per le operazioni sui semafori
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = -1;
    op.sem_flg = 0;

    // decremento del semaforo
    if (semop(sem_id, &op, 1) == -1){
        check_error("Lock semaforo");
    }

}

void unlock(int sem_num){

    // struttura per le operazioni sui semafori
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = 1;          
    op.sem_flg = 0;         

    // incremento del semaforo
    if (semop(sem_id, &op, 1) == -1) { 
        check_error("Unlock semaforo");    
    }
    
}

char *load_string(int my_string){

    /**
     * current_line per il conteggio del numero di linee
     * read per puntare alle stringhe da plain-encoded
     * j per spostarsi nella memoria
     */
    int current_line = 0;
    char *read = (char*)(s1 + sizeof(struct Status));
    int j;
    for(j = 0; current_line != my_string ; j++) {
        if(read[j] == '>'){
            // incremento il numero di righe
            current_line++;                 
            // la prossia stringa si trova due volte la lunghezza letta (plain ed encoded) + 4 (;<>\n) avanti
            read = read + j + j + 4;
            // azzero il contatore dei caratteri
            j = 0;                      
        }
    }

    return read;
}

unsigned find_key(char *read){

    /**
     * clear[512] per contenere il plain
     * encoded[512] per contenere l'encoded
     * i per spostarsi nella stringa in input
     * position per contare il numero di caratteri letti (plain)
     */
    char clear[512];
    char encoded[512];
    int i;
    int position = 0;
    // lettura della stringa plain e conteggio del numero di caratteri
    for(i = 1; read[i] != '>'; i++, position++){
        clear[position] = read[i];
    }
    int j = 0;
    // lettura della stringa encoded della stessa lunghezza di plain
    for(i = i + 3; j < position; i++, j++){
        encoded[j] = read[i];
    }

    /**
     * key chiave da trovare
     * unsigned_clear clear convertito per lo XOR
     * unsigned_encoded encoded convertito per lo XOR
     */
    unsigned key = 0;
    unsigned *unsigned_clear = (unsigned *) clear;
    unsigned *unsigned_encoded = (unsigned *) encoded;
    // calcolo del tempo di inizio
    time_t inizio = current_timestamp();
    // ciclo per la ricerca della chiave
    while((*unsigned_clear^ key) != *unsigned_encoded){
        key++;
    }

    // invio del messaggio "chiave trovata in" con il tempo impiegato
    send_timeelapsed(current_timestamp() - inizio);

    return key;
    
}

void save_key(unsigned key, int my_string){

    // accesso alla zona di memoria per il salvataggio della chiave nella posizione corretta
    unsigned *write = (unsigned *) s2;
    *(write + my_string)= key;

}

void send_timeelapsed(int time) {

    /**
     * Msg struttura del messaggio
     * size dimensione del messaggio
     * tempo stringa che contiene il tempo impiegato convertito in stringa
     * messaggio messaggio da inviare
     */
    struct Message Msg;
    int size = sizeof(Msg) - sizeof(long);
    char *tempo = itos(time);
    char *messaggio = concat("chiave trovata in ", tempo);

    copy(Msg.text, messaggio);
    // impostazione del tipo di messaggio
    Msg.mtype = 2;
    // invio del messaggio sulla coda
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        check_error("Invio messaggio sulla coda di messaggi");
    }

    // liberazione delle zone di memoria allocate
    free(tempo);
    free(messaggio);

}