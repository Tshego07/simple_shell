#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h> /* Include string.h for strcspn */

#define BUFFER_SIZE 1024

int main(void) {
    char buffer[BUFFER_SIZE];
    char *args[] = {NULL}; /* For now, we don't handle command arguments */
    pid_t pid; 

    while (1) {
        printf("#cisfun$ "); /* Display the prompt */
        fflush(stdout); /* Flush stdout to ensure prompt is displayed */

        /* Read input from the user */
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            printf("\n"); /* Print newline for Ctrl+D (EOF) */
            break; /* Exit loop on EOF */
        }

        /* Replace newline character with null terminator */
        buffer[strcspn(buffer, "\n")] = '\0';

        /* Fork a child process */
        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            /* Child process: execute the command */
            if (execve(buffer, args, NULL) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        } else {
            /* Parent process: wait for the child to terminate */
            wait(NULL);
        }
    }

    return EXIT_SUCCESS;
}
