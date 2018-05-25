/// @file
/// @defgroup figlio Figlio
/// @{

/**
 * @brief wrapper del processo figlio
 * @param shm_size dimensione della zona di memoria condivisa contenente il file di input
 * @param line numero di righe del file di input
 */
void figlio(int shm_size, int line);

/**
 * @brief signal handler del segnale SIGUSR1
 * @param s sengale ricevuto
 */
void status_update(int s);

/**
 * @brief deposita il messaggio di terminazione nella coda di messaggi del processo logger
 */
void send_terminate();

/// @}


