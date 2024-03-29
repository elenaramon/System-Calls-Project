#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <figlio.h>
#include <types.h>
#include <nipote.h>
#include <utilities.h>
#include <constants.h>

/**
 * status puntatore alla struttura Status
 * msq_id identificativo della coda di messaggi
 * sem_id identificativo dell'array di semafori
 * s1 puntatore alla zona di memoria condivisa s1
 * s2 puntatore alla zona di memoria condivisa s2
 * line numero di righe nel file
 * semun struttura per settare i semafori
 */
struct Status *status;
int msq_id;
int sem_id;
void *s1;
void *s2;
int line;
int pipe_descriptors[2];

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};


void figlio(int lines, void *shm1, void *shm2){

    line = lines;
    s1 = shm1;
    s2 =shm2;

    union semun sem_arg;

    status = (struct Status*) s1;

    // creo la pipe per la comunicazione tra figlio e nipoti/threads
    if(pipe(pipe_descriptors) == -1){
        check_error("FIGLIO: Creazione pipe");
    }

    // imposto il non bloccaggio in fase di lettura e scrittura nella pipe
    fcntl(pipe_descriptors[0], F_SETFL, O_NONBLOCK);
    fcntl(pipe_descriptors[1], F_SETFL, O_NONBLOCK);

    // il figlio si registra per catturare il segnale dei nipoti
    signal(SIGUSR1, status_updated);

    // creazione del semaforo
    if((sem_id = semget(KEY_SEM, 1, (0666 | IPC_CREAT | IPC_EXCL))) < 0){
        check_error("FIGLIO: Creazione semaforo");
    }

    // il semaforo 0 viene impostato ad 1
    sem_arg.val = 1;
    if (semctl(sem_id, 0, SETVAL, sem_arg) == -1) {    
        check_error("FIGLIO: Inizializzazione semaforo");    
    }      

    // viene creata la coda di messaggi
    if((msq_id = msgget(KEY_MSG, 0666 | IPC_CREAT)) < 0){        
        check_error("FIGLIO: Accesso coda di messaggi");
    }


    // in fase di compilazione in base al valore di CONDITION viene scelta quale parte compilare
    #if CONDITION != 1

        // PARTE CON I NIPOTI

        /**
         * son_nipote1, son_nipote2 identificativi dei nipoti
         * param[2] array per passare i dati ai nipoti
         */
        pid_t son_nipote1, son_nipote2;
        struct Params param[2];

        // creazione del nipote1
        if((son_nipote1 = fork()) == -1){
            check_error("FIGLIO: Creazione nipote1");
        }
        else if(son_nipote1 == 0){
            // vengono inseriti i dati nella struttura e viene chiamato il wrapper del nipote
            param[0].s1 = s1;
            param[0].s2 = s2;
            param[0].id = 1;
            param[0].sem = sem_id;
            param[0].line = line;
            param[0].pipe = pipe_descriptors[1];
            nipote((void *) &param[0]);
        }
        else{
            // creazione nipote2
            if((son_nipote2 = fork()) == -1){
                check_error("FIGLIO: Creazione nipote2");
            }
            else if(son_nipote2 == 0){
                // vengono inseriti i dati nella struttura e viene chiamato il wrapper del nipote
                param[1].s1 = s1;
                param[1].s2 = s2;
                param[1].id = 2;
                param[1].sem = sem_id;
                param[1].line = line;
                param[1].pipe = pipe_descriptors[1];
                nipote((void *) &param[1]);
            }
            else{
                // il padre attende la terminazione dei due figli
                wait(NULL);
                wait(NULL);
        
                // viene mandato il messaggio "ricerca conclusa" sulla coda di messaggi
                send_terminate();

                // viene eliminato l'array di semafori
                sem_arg.val = 0;
                if(semctl(sem_id, 0, IPC_RMID, sem_arg) == -1){
                    check_error("FIGLIO: Rimozione semaforo");
                }  
            }
        }

        exit(0);
        // FINE PARTE CON I NIPOTI

    #else

        // PARTE CON I THREAD

        /**
         * threads[NUM] array per gli identificativi dei thread
         * param[NUM] strutture per il passaggio dei parametri ai thread
         * creation_return controllo del ritorno per la creazione del thread
         * counter contatore per la creazione dei thread
         */
        pthread_t threads[NUM];
        struct Params param[NUM];
        int creation_return, counter;
        for(counter = 0; counter < NUM; counter++){

            // vengono inseriti i dati nella struttura da passare al wrapper di nipote
            param[counter].s1 = s1;
            param[counter].s2 = s2;
            param[counter].id = (counter + 1);
            param[counter].sem = sem_id;
            param[counter].line = line;
            param[counter].pipe = pipe_descriptors[1];

            creation_return = pthread_create(&threads[counter], NULL, nipote, (void *) &param[counter]);
            if(creation_return){
                check_error("FIGLIO: Creazione thread");
            }
        }

        // viene settata l'attesa da parte del padre dei thread creati
        for(counter = 0; counter < NUM; counter++){
            pthread_join(threads[counter], NULL);
        }  

        // il figlio manda il messaggio "ricerca conclusa" sulla coda di messaggi
        send_terminate();

        // viene eliminato l'array di semafori
        sem_arg.val = 0;
        if(semctl(sem_id, 0, IPC_RMID, sem_arg) == -1){
            check_error("FIGLIO: Rimozione semaforo");
        }     

        exit(0);
        // FINE PARTE CON I THREAD

    #endif


}


void status_updated(int sig){

    // si controlla se il segnale è quello che handler deve gestire
    if(sig == SIGUSR1){

        char buffer[512];
        int read_line;
        int write_line;
        // leggo dalla pipe
        while((read_line = read(pipe_descriptors[0], buffer, 512)) > 0) {
            if((write_line = write(1, buffer, read_line)) == -1){
                check_error("FIGLIO: Scrittura su STDOUT");
            }
        }   

        if(read_line == -1){
            check_error("FIGLIO: Lettura dalla pipe");
        }
    }

}


void send_terminate(){

    // viene creata la struttura del messaggio
    struct Message Msg;
    // viene settata la misura del messaggio
    int size = sizeof(Msg) - sizeof(long);
    char *messaggio = "ricerca conclusa";
    // viene copiato il messaggio nel campo testo della struttura
    copy(Msg.text, messaggio);
    // viene settato il tipo di messaggio a 1
    Msg.mtype = 1;
    // invio del messaggio
    if((msgsnd(msq_id, &Msg, size, 0)) == -1){
        check_error("FIGLIO: Invio messaggio sulla coda di messaggi");
    }

}