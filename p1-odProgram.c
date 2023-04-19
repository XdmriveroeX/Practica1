#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Constantes para los nombres de los programas
const char *INTERFACE_PROGRAM = "./interface";
const char *SEARCHER_PROGRAM = "./searcher";

// Prototipos de funciones
pid_t create_and_run_child_process(const char *program_name);
void wait_for_child_processes(pid_t *pids, int num_pids);



int main() {
    // Crear procesos hijo para la interfaz y el buscador
    pid_t pid1 = create_and_run_child_process(INTERFACE_PROGRAM);
    pid_t pid2 = create_and_run_child_process(SEARCHER_PROGRAM);

    // Esperar a que ambos procesos hijo terminen
    pid_t child_pids[] = {pid1, pid2};
    wait_for_child_processes(child_pids, 2);

    printf("Both programs have finished executing.\n");

    return 0;
}

pid_t create_and_run_child_process(const char *program_name) {
    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo
        char *args[] = {(char *)program_name, NULL};
        if (execvp(args[0], args) == -1) {
            perror("Error executing child process");
            exit(1);
        }
    } else if (pid < 0) {
        perror("Error forking child process");
        exit(1);
    }

    return pid;
}

void wait_for_child_processes(pid_t *pids, int num_pids) {
    int child_status;

    for (int i = 0; i < num_pids; i++) {
        waitpid(pids[i], &child_status, 0);
    }
}

