/// @file
/// @defgroup figlio Figlio
/// @{

/**
 * @brief wrapper del processo figlio
 * @param line numero di righe del file di input
 * @param s1 puntatore alla zona di memoria contenente il file di input e la struttura Status
 * @param s2 puntatore alla zona di memoria per contenere le chiavi trovate
 */
void figlio(int line, void *s1, void *s2);

/**
 * @brief signal handler del segnale SIGUSR1
 * @param sig segnale ricevuto
 */
void status_updated(int sig);

/**
 * @brief deposita il messaggio di terminazione nella coda di messaggi del processo logger
 */
void send_terminate();

/// @}




