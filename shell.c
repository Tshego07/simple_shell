#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(void) {
    char buffer[BUFFER_SIZE];
    char *args[BUFFER_SIZE / 2 + 1]; // Add space for arguments (+1 for NULL)
    pid_t pid;

    while (1) {
        printf("#cisfun$ ");
        fflush(stdout);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            printf("\n");
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

        // Tokenize input into arguments
        char *token = strtok(buffer, " ");
        int i = 0;
        while (token != NULL && i < BUFFER_SIZE / 2) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL; // Mark end of argument list with NULL

        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process: execute the command
            if (execve(args[0], args, NULL) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process: wait for the child to terminate
            wait(NULL);
        }
    }

    return EXIT_SUCCESS;
}
