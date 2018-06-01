/// @file
/// @defgroup figlio Figlio
/// @{

/**
 * @brief wrapper del processo figlio
 * @param shm_size dimensione della zona di memoria condivisa contenente il file di input
 * @param line numero di righe del file di input
 */
void figlio(int line, void *s1, void *s2);

/**
 * @brief signal handler del segnale SIGUSR1
 * @param s sengale ricevuto
 */
void status_updated(int s);

/**
 * @brief deposita il messaggio di terminazione nella coda di messaggi del processo logger
 */
void send_terminate();

/**
 * @brief starting routine per i thread
 * @param threadID id del thread
 */
void *StartThread(void *threadID);

/// @}




