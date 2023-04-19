#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

typedef struct IndexNode {
    long position;
    struct IndexNode *next;
} IndexNode;

typedef struct SharedMemoryData {
    int sourceid;
    int dstid;
    int hod;
    float mean_travel_time;
    int ready; // 0: Not ready, 1: Ready
} SharedMemoryData;

#define TABLE_SIZE 1160

//Prototipos de funciones
int hash_function(int sourceid);
IndexNode *load_index_entry(const char *index_filename, int sourceid);
float search_mean_travel_time(const char *csv_filename, const char *index_filename, int sourceid, int dstid, int hod, double *elapsed_time);
int initialize_shared_memory(key_t key, int *shmid, SharedMemoryData **shared_data);
int detach_and_remove_shared_memory(int shmid, SharedMemoryData *shared_data);

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid;
    SharedMemoryData *shared_data;

    if (initialize_shared_memory(key, &shmid, &shared_data) == -1) {
        return 1;
    }

    while (1) {
        while (shared_data->ready == 0);

        if (shared_data->sourceid == -1 && shared_data->dstid == -1 && shared_data->hod == -1) {
            break;
        }

        double elapsed_time;
        float mean_travel_time = search_mean_travel_time("uber_movement.csv", "index_file.bin", shared_data->sourceid, shared_data->dstid, shared_data->hod, &elapsed_time);
        printf("Tiempo de búsqueda: %.6f segundos\n", elapsed_time);

        shared_data->mean_travel_time = mean_travel_time;
        shared_data->ready = 0;
    }

    if (detach_and_remove_shared_memory(shmid, shared_data) == -1) {
        return 1;
    }

    return 0;
}

int hash_function(int sourceid) {
    return sourceid % TABLE_SIZE;
}

IndexNode *load_index_entry(const char *index_filename, int sourceid) {
    FILE *index_file = fopen(index_filename, "rb");
    if (index_file == NULL) {
        perror("Error opening index file");
        return NULL;
    }

    int index = hash_function(sourceid);
    IndexNode *head = NULL;

    while (!feof(index_file)) {
        int current_index;
        long position;

        fread(&current_index, sizeof(int), 1, index_file);
        fread(&position, sizeof(long), 1, index_file);

        if (current_index == index) {
            IndexNode *node = (IndexNode *)malloc(sizeof(IndexNode));
            node->position = position;
            node->next = head;
            head = node;
        }
    }

    fclose(index_file);
    return head;
}

float search_mean_travel_time(const char *csv_filename, const char *index_filename, int sourceid, int dstid, int hod, double *elapsed_time) {
    clock_t start_time = clock();

    FILE *csv_file = fopen(csv_filename, "r");
    if (csv_file == NULL) {
        perror("Error opening CSV file");
        return -1;
    }

    IndexNode *index_entry = load_index_entry(index_filename, sourceid);
    if (index_entry == NULL) {
        fclose(csv_file);
        return -1;
    }

    float mean_travel_time = -1;
    IndexNode *current = index_entry;

    while (current != NULL) {
        fseek(csv_file, current->position, SEEK_SET);
        char line[256];
        fgets(line, sizeof(line), csv_file);
        int current_sourceid, current_dstid, current_hod;
        float current_mean_travel_time;
        sscanf(line, "%d,%d,%d,%f", &current_sourceid, &current_dstid, &current_hod, &current_mean_travel_time);

        if (current_sourceid == sourceid && current_dstid == dstid && current_hod == hod) {
            mean_travel_time = current_mean_travel_time;
            break;
        }

        current = current->next;
    }

    while (index_entry != NULL) {
        IndexNode *temp = index_entry;
        index_entry = index_entry->next;
        free(temp);
    }

    fclose(csv_file);

    clock_t end_time = clock();
    *elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    return mean_travel_time;
}

int initialize_shared_memory(key_t key, int *shmid, SharedMemoryData **shared_data) {
    *shmid = shmget(key, sizeof(SharedMemoryData), 0666 | IPC_CREAT);
    if (*shmid == -1) {
        perror("Error creating shared memory segment");
        return -1;
    }

    *shared_data = (SharedMemoryData *)shmat(*shmid, NULL, 0);

    if (*shared_data == (void *)-1) {
        perror("Error attaching shared memory segment");
        return -1;
    }

    (*shared_data)->ready = 0;
    return 0;
}

int detach_and_remove_shared_memory(int shmid, SharedMemoryData *shared_data) {
    if (shmdt(shared_data) == -1) {
        perror("Error detaching shared memory segment");
        return -1;
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Error removing shared memory segment");
        return -1;
    }

    return 0;
}


    
    
