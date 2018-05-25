
void padre(char *file_name_input, char *file_name_output);
void* attach_segments(int key, int shm_size);
void detach_segments(struct shmid_ds shmid_struct, int shm_size, int key);
int load_file(char* shm_write, int file_descriptor_input);
void check_keys(char *shm_address1, unsigned * shm_address2, int counter);
void save_keys(unsigned *shm_address, int file_descriptor_output, int lines);
