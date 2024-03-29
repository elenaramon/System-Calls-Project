/// @file
/// @defgroup nipote Nipote
/// @{

/**
 * @brief wrapper del processo nipote
 * @param params puntatore alla struttura contenente i parametri per nipote
 */
void *nipote(void *params);

/**
 * @brief legge la stringa dal segmento s1
 * @param my_string stringa da caricare
 * @return puntatore alla linea interessata
 */
char *load_string(int my_string);

/**
 * @brief esegue il decremento del valore del semaforo
 * @param sem_num numero del semaforo
 */
void lock(int sem_num);

/**
 * @brief esegue l'incremento del valore del semaforo
 * @param sem_num numero del semaforo
 */
void unlock(int sem_num);

/**
 * @brief trova la chiave
 * @param current_line la stringa contenente plain e encoded text 
 * @return la chiave trovata
 */
unsigned find_key(char *current_line);

/**
 * @brief deposita il messaggio "chiave trovata in (secondi)" sulla coda dei messaggi
 * @param time durata della ricerca della chiave
 */
void send_timeelapsed(int time);

/**
 * @brief salva la chiave nel segmento di memoria s2
 * @param key chiave trovata
 * @param my_string numero della chiave trovata
 */
void save_key(unsigned key, int my_string);

/// @}