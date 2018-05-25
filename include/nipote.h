/// @file
/// @defgroup nipote Nipote
/// @{

/**
 * @brief wrapper del processo nipote
 * @param shm_size dimensione del segmento s1 di memoria condivisa
 * @param line numero di chiavi da trovare
 * @param id identificativo del processo figlio
 */
void nipote(int shm_size, int line, int id);

/**
 * @brief legge la stringa dal segmento s1
 * @param line numero di righe del file
 * @param my_string stringa da caricare
 */
void load_string(int line, int my_string);

/**
 * @brief blocca l'accesso esclusivo alla struttura Status
 * @param sem_num numero del semaforo
 */
void lock(int sem_num);

/**
 * @brief sblocca l'accesso esclusivo alla struttura Status
 * @param sem_num numero del semaforo
 */
void unlock(int sem_num);

/**
 * @brief trova la chiave
 * @param clear testo in chiaro
 * @param encoded testo cifrato
 * @param my_string stringa che stiamo analizzando
 */
void find_key(char *clear, char *encoded, int my_string);

/**
 * @brief deposita il messaggio "chiave trovata in (secondi)" sulla coda dei messaggi
 * @param time durata della ricerca della chiava
 */
void send_timeelapsed(int time);

/**
 * @brief salva la chiave nel segmento di memoria s2
 * @param key chiava trovata
 * @param my_string numero della chiava trovata
 */
void save_key(unsigned key, int my_string);

/**
 * @brief calcola l'ora della chiamata alla funzione in secondi
 * @return l'ora attuale in secondi
 */
time_t current_timestamp();

/// @}