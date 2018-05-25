/// @file
/// @defgroup utilities Utilities
/// @{

/**
 * @brief calcola la lunghezza di una stringa
 * @param stringa puntatore alla stringa di cui calcolare la lunghezza
 * @return il numero di caratter
 */
int string_length(char* stringa);

/**
 * @brief converte un intero in stringa
 * @param num il numero da convertire
 * @return il numero convertito in stringa
 */
char* from_int_to_string(int num);

/**
 * @brief converte un unsigned in esadecimale
 * @param numero l'unsigned da convertire
 * @return il numero convertito in stringa
 */
char* from_unsigned_to_hexa(unsigned numero);

/**
 * @brief copia una stringa dentro un'altra
 * @param stringa1 la stringa in cui copiare 
 * @param stringa2 la stringa da copiare
 * @return stringa2 copiata in stringa1
 */
void copy_string(char *stringa1, char* stringa2);

/**
 * @brief concatena due stringhe
 * @param stringa1 la prima stringa che comparirà in quella finale
 * @param stringa2 la seconda stringa che comparirà in quella finale
 * @return la conatenazione di stringa2 a stringa1
 */
char* concat_string(char* stringa1, char* stringa2);

/**
 * @brief stampa su stdout una stringa con new line
 * @param stringa la stringa da stampare
 */
void printing(char *stringa);

/// @}