#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct SharedMemoryData {
    int sourceid;
    int dstid;
    int hod;
    float mean_travel_time;
    int ready; // 0: Not ready, 1: Ready
} SharedMemoryData;

// Prototipos de funciones
void print_menu();
void initialize_shared_data(SharedMemoryData *shared_data);
int detach_shared_memory(SharedMemoryData *shared_data);

int main() {
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("Error creating shared memory key");
        return 1;
    }

    int shmid = shmget(key, sizeof(SharedMemoryData), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Error creating shared memory segment");
        return 1;
    }

    SharedMemoryData *shared_data = (SharedMemoryData *)shmat(shmid, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("Error attaching shared memory segment");
        return 1;
    }

    initialize_shared_data(shared_data);

    int option;
    while (1) {
        print_menu();
        printf("Ingrese la opción: ");
        scanf("%d", &option);

        switch (option) {
            case 1:
                printf("Ingrese ID del origen: ");
                scanf("%d", &shared_data->sourceid);
                break;
            case 2:
                printf("Ingrese ID del destino: ");
                scanf("%d", &shared_data->dstid);
                break;
            case 3:
                printf("Ingrese hora del día: ");
                scanf("%d", &shared_data->hod);
                break;
            case 4:
                shared_data->ready = 1;
                while (shared_data->ready == 1);

                if (shared_data->mean_travel_time >= 0){
                    printf("Tiempo de viaje medio: %.1f\n", shared_data->mean_travel_time);
                } else printf("Tiempo de Viaje medio: N/A\n");
                
                break;
            case 5:
                initialize_shared_data(shared_data);
                shared_data->ready = 1;
                goto exit_loop;
            default:
                printf("Opción inválida, por favor ingrese un número entre 1 y 5.\n");
        }
    }

exit_loop:
    if (detach_shared_memory(shared_data) == -1) {
        return 1;
    }

    return 0;
}

// Prints the menu options for the user

void print_menu() {
    printf("Bienvenido\n");
    printf("1. Ingresar origen\n");
    printf("2. Ingresar destino\n");
    printf("3. Ingresar hora\n");
    printf("4. Buscar tiempo de viaje medio\n");
    printf("5. Salir\n");
}

// Initializes shared memory data with default values

void initialize_shared_data(SharedMemoryData *shared_data) {
    shared_data->sourceid = -1;
    shared_data->dstid = -1;
    shared_data->hod = -1;
    shared_data->ready = 0;
}

// Detaches the shared memory segment

int detach_shared_memory(SharedMemoryData *shared_data) {
    if (shmdt(shared_data) == -1) {
        perror("Error detaching shared memory segment");
        return -1;
    }
    return 0;
}
