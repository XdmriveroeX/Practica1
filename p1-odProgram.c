#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

// Function prototypes
void execute_interface(void);
void execute_searcher(void);
void wait_for_children(pid_t pid1, pid_t pid2);
void execute_create_indexed_file(void);

int main() {
    struct stat buffer;

    // Check if the indexed_file.bin doesn't exist
    if (stat("index_file.bin", &buffer) != 0) {
        execute_create_indexed_file();
    }

    int child_status;
    pid_t pid1 = fork();

    if (pid1 == 0) {
        // Child process for interface
        execute_interface();
    } else if (pid1 > 0) {
        pid_t pid2 = fork();

        if (pid2 == 0) {
            // Child process for searcher
            execute_searcher();
        } else if (pid2 > 0) {
            // Parent process waits for both child processes to finish
            wait_for_children(pid1, pid2);
            printf("Both programs have finished executing.\n");
        } else {
            perror("Error forking searcher process");
            exit(1);
        }
    } else {
        perror("Error forking interface process");
        exit(1);
    }

    return 0;
}

// Function implementations

void execute_interface() {
    char *args[] = {"./interface", NULL};
    if (execvp(args[0], args) == -1) {
        perror("Error executing interface");
        exit(1);
    }
}

void execute_searcher() {
    char *args[] = {"./searcher", NULL};
    if (execvp(args[0], args) == -1) {
        perror("Error executing searcher");
        exit(1);
    }
}

void wait_for_children(pid_t pid1, pid_t pid2) {
    int child_status;
    waitpid(pid1, &child_status, 0);
    waitpid(pid2, &child_status, 0);
}

void execute_create_indexed_file() {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process for create_indexed_file
        char *args[] = {"./create_indexed_file", NULL};
        if (execvp(args[0], args) == -1) {
            perror("Error executing create_indexed_file");
            exit(1);
        }
    } else if (pid > 0) {
        // Parent process waits for child process to finish
        int child_status;
        waitpid(pid, &child_status, 0);
    } else {
        perror("Error forking create_indexed_file process");
        exit(1);
    }
}
