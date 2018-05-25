/// @file
/// @defgroup logger Logger
/// @{

/**
 * @brief wrapper del processo logger
 */
void logger();

/**
 * @brief scarica la coda di messaggi e la stampa su stdout
 * @param msq_id identificativo della coda di messaggi
 * @return (1) se è stato letto un messaggio di terminazione
 */
int polling_receive(int msq_id);

/// @}