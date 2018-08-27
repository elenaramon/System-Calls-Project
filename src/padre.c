#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include <padre.h>
#include <logger.h>
#include <types.h>
#include <figlio.h>
#include <utilities.h>
#include <constants.h>

/**
 * lines numero di righe del file di input
 * shmid_struct struttura per la gestione della memoria condivisa 
 */
int lines = 0;
struct shmid_ds shmid_struct;
int id1;
int id2;

void padre(char *file_name_input, char *file_name_output){

    /**
     * son_logger, son_figlio identificativi dei processi generati
     * file_descriptor_input, file_descriptor_output descrittori dei file
     * shm_size1, shm_size2 dimensioni delle zone di memoria condivise
     */   


    signal(SIGINT, error_signal);   // ctrl + c
    signal(SIGTSTP, error_signal);  // ctrl + z
	signal(SIGTERM, error_signal);
	signal(SIGQUIT, error_signal);  // ctrl + \  //  
	signal(SIGSEGV, error_signal);
	signal(SIGPIPE, error_signal);


    pid_t son_logger, son_figlio;
    int file_descriptor_input;
    int shm_size1, shm_size2;

    // controllo esistenza file di output, se esiste termino
    if (access(file_name_output, F_OK) == 0) {
        check_error("PADRE: Il file di output esiste già");
    }

    // apertura del file di lettura
    if((file_descriptor_input = open(file_name_input, O_RDONLY, 0777)) == -1){
        check_error("PADRE: Apertura file input");
    }

    /**
     * read_line numero di caratteri letti dal file
     * buffer[SIZE] buffer contenente i caratteri letti dal file
     * position offset dall'inizio del file
     */

    int read_line;
    char buffer[SIZE];
    int position = 0;
    while((read_line = read(file_descriptor_input, &buffer, SIZE)) > 0){
        // ciclo di scorrimento dei caratteri del buffer
        int i;
        for(i = 0; i < read_line; i++, position++){           
            // se buffer è uguale a '>' viene incrementato il numero di linee
            if(buffer[i] == '>'){
                lines++;
                // viene calcolata la nuova posizione in lettura
                i = i + position + 5;
                position = 0;
                if(i > read_line)
                {
                    if(lseek(file_descriptor_input, i - read_line + 1, SEEK_CUR) == -1){
                        check_error("PADRE: Riposizionamento in lettura nel file (conteggio delle righe)");
                    }
                }
            }            
        }       
    }
    if(read_line == -1){
        check_error("PADRE: Lettura del file di input (conteggio delle righe)");
    }

    // calcolo dimensione della prima zona di memoria condivisa
    int end_position = lseek(file_descriptor_input, 0L, SEEK_END);
    if(end_position == -1){
        check_error("PADRE: Calcolo della dimensione del file");
    }
    shm_size1 = sizeof(struct Status) + end_position;
    // creazione e collegamento della zona prima di memoria condivisa
    void *s1 = attach_segments(KEY_SHM1, shm_size1);

    // definizione posizione struttura
    struct Status *status = (struct Status*) s1;
    status->id_string = 0;
    status->grandson = 0;

    // definizione posizione file di input
    char *shm_write1 = (char*)s1 + sizeof(struct Status);

    // caricamento del file nella zona di memoria condivisa
    load_file(shm_write1, file_descriptor_input);

    // viene definita la dimensione della seconda zona di memoria condivisa
    shm_size2 = sizeof(char) * 11 * lines;
    // creazione e collegamento della seconda zona di memoria condivisa
    void *s2 = attach_segments(KEY_SHM2, shm_size2);

    // creazione del figlio logger
    if((son_logger = fork()) == -1){
        check_error("PADRE: Creazione logger");
    }
    else if(son_logger == 0){
        // esecuzione di logger
         logger();
        //execl("./logger", "logger", itos(KEY_MSG), (char *)NULL);
    }
    else{
        // creazione del figlio figlio
        if((son_figlio = fork()) == -1){
            check_error("PADRE: Creazione figlio");
        }
        else if(son_figlio == 0){
             // esecuzione di figlio
            figlio(lines, s1, s2);
        }
        else{
            
            // il padre attende la terminazione di figli
            wait(NULL);
            wait(NULL);

            // controllo delle chiavi trovate per tutta la lunghezza del testo
            check_keys((char*)(s1 + sizeof(struct Status)), (unsigned*) s2);

            // salva le chiavi nel file di output
            save_keys((unsigned*) s2, file_name_output);

            // sliminazione dei segmenti di memoria condivisa
            detach_segments(shm_size1, KEY_SHM1, s1);
            detach_segments(shm_size2, KEY_SHM2, s2);
        }
    }

    exit(0);
}

void *attach_segments(int key, int shm_size){

    /**
     * shm_id identificativo della zona di memoria condivisa
     * shm_address idirizzo della zona di memoria condivisa
     */
    int shm_id;
    char *shm_address;

    // creazione della zona di memoria condivisa   
    if((shm_id =  shmget(key, shm_size, 0666 | IPC_CREAT| IPC_EXCL)) < 0){
        check_error("PADRE: Creazione memoria condivisa");
    }
    // collegamento zona di memoria condivisa con la memoria del processo
    if((shm_address = shmat(shm_id, NULL, 0)) == (void*) -1){
        check_error("PADRE: Attach memoria condivisa");
    }

    if(key == KEY_SHM1){
        id1 = shm_id;
    }
    else{
        id2 = shm_id;
    }

    return shm_address;

}

void detach_segments(int shm_size, int key, void *shm_address){

    // detach della zona di memoria condivisa
    shmdt(shm_address);

    // shm_id identificativo della zona di memoria condivisa
    int shm_id;
    
    // recupero identificativo della zona di memoria condivisa
    if((shm_id =  shmget(key, shm_size, 0666)) < 0){
        check_error("PADRE: Recupero ID memoria condivisa");
    }
 
    // eliminazione della zona di memoria condivisa
    if(shmctl(shm_id, IPC_RMID, &shmid_struct) == -1){
        check_error("PADRE: Deallocazione memoria condivisa");
    }

}

void load_file(char *shm_write, int file_descriptor){

    /**
     * read_line numero di caratteri letti
     * buffer[SIZE] buffer per contenere i caratteri letti
     * totalIndex posizione di memoria
     */
    int read_line;
    char buffer[SIZE];
    int totalIndex = 0;

    // viene impostato il puntatore in lettura del file a 0
    if(lseek(file_descriptor, 0L, SEEK_SET) == -1){
        check_error("PADRE: Riposizionamento nel file (Caricamento)");
    }

    // ciclo di lettura del file
    while((read_line = read(file_descriptor, &buffer, SIZE)) > 0){
        // tutto quello letto viene salvato in memoria
        int i;
        for(i = 0; i < read_line; i++){            
            shm_write[totalIndex++] = buffer[i];           
            
        }       
    }
    if (read_line == -1){
        check_error("PADRE: Lettura dal file di input (Caricamento)");
    }

    // viene chiuso il file
    if(close(file_descriptor) == -1){
        check_error("PADRE: Chiusura file di input");
    }

}

void check_keys(char *shm_address1, unsigned *shm_address2){

    /**
     * clear[SIZE] array per contenere il plain text
     * encoded[SIZE] array per contenere l'encoded text
     */
    char clear[SIZE];
    char encoded[SIZE];

    // scorrimento della zona di memoria
    int i;
    for(i = 0; i < lines; i++){
        int j = 0;
        // viene recuperato il plain text
        for(shm_address1 = (shm_address1 + 1); *shm_address1 != '>'; shm_address1++, j++){
            clear[j] = *shm_address1;            
        }
        // viene impostata la dimensione in unsigned
        int size = j / 4;
        int position = 0;
        // viene recuperato l'encoded text
        for(shm_address1 = (shm_address1 + 3); position < j; shm_address1++, position++){
            encoded[position] = *shm_address1;
        }

        /**
         * key chiave trovata e da controllare
         * unsigned_clear cast in unsigned del plain text
         * unsigned_encoded cast in unsigned dell'encoded text
         * check per il controllo della correttezza della chiave
         */
        unsigned key = shm_address2[i];
        unsigned *unsigned_clear = (unsigned *) clear;
        unsigned *unsigned_encoded = (unsigned *) encoded;
        int check = 0;     
        // ciclo per il controllo   
        for(j = 0; j < size; j++){
            if((*(unsigned_clear + j) ^ key) != *(unsigned_encoded + j)){
                check = 1;
            }
        }   
        if(check == 1){
                printing("Trovata una chiave non corretta.");
        }
        shm_address1 = shm_address1 + 2;
    }

}

void save_keys(unsigned *shm_address, char *file_name_output){
    
    /**
     * hexa stringa esadecimale
     * chiave stringa da salvare sul file
     */
    char *hexa;
    char *chiave;
    int file_descriptor;
    // creazione del file di scrittura
    if((file_descriptor = creat(file_name_output, ((O_RDONLY | O_WRONLY) ^ 0777))) == -1){
        check_error("PADRE: Apertura file output");
    }

    // ciclo per il salvataggio sul file
    int i;
    int write_line;
    for(i = 0; i < lines; i++){
        hexa = utoh(*(shm_address + i));
        chiave = concat("0x", hexa);
        chiave = concat(chiave, "\r\n");
        if((write_line = write(file_descriptor, chiave, length(chiave))) == -1){
            check_error("PADRE: Scrittura sul file di output");
        }

    }

    // liberazione delle memorie allocate
    free(hexa);        
    free(chiave);
    
    // viene chiuso il file
    if(close(file_descriptor) == -1){
        check_error("PADRE: Chiusura file di output");
    }

}
