

void figlio(int shm_size, int line);
void status_update(int s);
void send_terminate();

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short int* array;
    struct seminfo *__buf;
};

