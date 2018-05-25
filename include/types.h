/// @file
/// @defgroup types Types
/// @{

/**
 * @brief struttura per identificare la chiave (id_string) su cui un nipote (grandson) sta lavorando
 */
struct Status{
    /// @param grandson identificatore del nipote 
    int grandson;
    /// @param id_string identificatore della stringa
    int id_string;
};

/**
 * @brief struttura per la composizione dei messaggi
 */
struct Message{
    /// @param mtype tipo di messaggio che viene inviato/ricevuto
    long mtype;
    /// @param text contiene il testo del messaggio
    char text[128];
};

/// @}
