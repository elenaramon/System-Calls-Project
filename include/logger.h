/// @file
/// @defgroup logger
/// @{

/**
 * @brief wrapper del processo logger
 */
void logger();

/**
 * @brief scarica la coda di messaggi e la stampa su stdout
 * @param msq_id identificativo della coda di messaggi
 * @param msqid_ds buf struttura contenuta nella libreria <sys/msg.h>
 * @return (1) se è stato letto un messaggio di terminazione
 */
int polling_receive(int msq_id);

/// @}