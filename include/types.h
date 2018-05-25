/// @file
/// @defgroup types
/// @{

/**
 * @brief struttura per identificare la chiave (id_string) su cui un nipote (grandson) sta lavorando
 */
struct Status{
    int grandson;
    int id_string;
};

/**
 * @brief struttura per la composizione dei messaggi
 */
struct Message{
    long mtype;
    char text[128];
};

/// @}
