/// @file
/// @defgroup padre Padre
/// @{

/**
 * @brief wrapper del processo padre
 * @param file_name_input nome del file di input
 * @param file_name_output nome del file di output
 */
void padre(char *file_name_input, char *file_name_output);

/**
 * @brief crea un segmento di memoria condivisa
 * @param key chiave identifiativa del segmento di memoria
 * @param shm_size dimensione del segmento di memoria
 * @return puntatore alla zona di memoria condivisa
 */
void* attach_segments(int key, int shm_size);

/**
 * @brief elimina il segmento di memoria condivisa
 * @param shm_size dimensione della zona di memoria condivisa
 * @param key chiave identificativa del segmento di memoria
 * @param shm_address indirizzo di memoria condivisa
 */
void detach_segments(int shm_size, int key, void *shm_address);

/**
 * @brief carica il file di input nel segmento di memoria s1
 * @param shm_write puntatore alla zona di memoria condivisa che conterrà il file
 * @param file_descriptor_input file descriptor del file da leggere
 * @return numero di righe del file
 */
int load_file(char* shm_write, int file_descriptor_input);

/**
 * @brief controlla che le chiavi trovate siano corrette per tutta la lunghezza delle stringhe
 * @param shm_address1 puntatore alla zona di memoria condivisa s1
 * @param shm_address2 puntatore alla zona di memoria condivisa s2
 */
void check_keys(char *shm_address1, unsigned * shm_address2);

/**
 * @brief salva le chiavi sul file di output
 * @param shm_address puntatore alla zona di memoria s2
 * @param file_descriptor_output file descriptor del file di output
 */
void save_keys(unsigned *shm_address, int file_descriptor_output);

/// @}