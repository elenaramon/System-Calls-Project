/// @file
/// @defgroup costanti Costanti
/// @{

#ifndef KEY_SHM1
/**
 * @brief chiave della sezione di memoria condivisa s1
 */
#define KEY_SHM1 75
#endif

#ifndef KEY_SHM2
/**
 * @brief chiave della sezione di memoria condivisa s2
 */
#define KEY_SHM2 76
#endif

#ifndef KEY_MSG
/**
 * @brief chiave della coda di messaggi condivisa
 */
#define KEY_MSG 77
#endif

#ifndef KEY_SEM
/**
 * @brief chiave dell'array di semafori
 */
#define KEY_SEM 78
#endif

#ifndef SIZE
/**
 * @brief lunghezza massima di una riga
 */
#define SIZE 512
#endif

#ifndef STRUCT_PARAMS
/**
 * @brief struttura per il passaggio dei parametri al wrapper nipote
 */
struct Params{
    /// @param s1 puntatore alla zona di memoria s1
    void *s1;
    /// @param s2 puntatore alla zona di memoria s2
    void *s2;
    /// @param line numero di chiavi da trovare
    int line;
    /// @param id identificatore del processo nipote o del thread    
    int id;
    /// @param sem identificatore del semaforo
    int sem;
};
#endif

#ifndef NUM
/**
 * @brief numero di thread da generare
 */
#define NUM 2
#endif

#ifndef CONDITION
/**
 * @brief variabile per la scelta tra thread e processi, valore di default 0
 * se CONDITION vale 0 (o un valore diverso da 1) viene compilata ed eseguita la parte con i processi
 * se CONDITION vale 1 viene compilata ed eseguita la parte con i thread
 */
#define CONDITION 0
#endif

/// @}
