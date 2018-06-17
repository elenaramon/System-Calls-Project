/// @file
/// @defgroup utilities Utilities
/// @{

/**
 * @brief calcola la lunghezza di una stringa
 * @param stringa puntatore alla stringa di cui calcolare la lunghezza
 * @return il numero di caratteri
 */
int length(char *stringa);

/**
 * @brief converte un intero in stringa
 * @param num il numero da convertire
 * @return il numero convertito in stringa
 */
char *itos(int num);

/**
 * @brief converte un unsigned in esadecimale
 * @param numero l'unsigned da convertire
 * @return il numero convertito in esadecimale contenuto in una stringa
 */
char *utoh(unsigned numero);

/**
 * @brief copia una stringa dentro un'altra
 * @param stringa1 la stringa in cui copiare 
 * @param stringa2 la stringa da copiare
 */
void copy(char *stringa1, char *stringa2);

/**
 * @brief concatena due stringhe
 * @param stringa1 la prima stringa che comparirà in quella finale
 * @param stringa2 la seconda stringa che comparirà in quella finale
 * @return la conatenazione di stringa2 a stringa1
 */
char *concat(char *stringa1, char *stringa2);

/**
 * @brief stampa su stdout una stringa con new line
 * @param stringa la stringa da stampare
 */
void printing(char *stringa);

/**
 * @brief handler per i vari segnali di terminazione
 * @param sig segnale ricevuto
 */
void error_signal(int sig);

/**
 * @brief rimuove le risorse condivise create quando il programma termina in modo non regolare
 */
void remove_resources();

/**
 * @brief controllo errore della system call
 * @param message messaggio di errore
 */
void check_error(char *message);

/**
 * @brief funzione che stampa a video il messaggio di errore
 * @param message messaggio di errore
 */
void syserr(char *message);

/// @}