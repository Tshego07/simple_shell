#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 1024

char *concat_path(char *dir, char *cmd) {
    char *result = malloc(strlen(dir) + strlen(cmd) + 2);
    if (result == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(result, dir);
    strcat(result, "/");
    strcat(result, cmd);
    return result;
}

void print_environment() {
    extern char **environ;
    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }
}

int main(void) {
    char buffer[BUFFER_SIZE];
    char *args[BUFFER_SIZE / 2 + 1];
    pid_t pid;

    while (1) {
        printf("$ ");
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
        args[i] = NULL;

        // Check if command is 'exit'
        if (strcmp(args[0], "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }

        // Check if command is 'env'
        if (strcmp(args[0], "env") == 0) {
            print_environment();
            continue;
        }

        // Check if command exists in PATH
        char *path = getenv("PATH");
        char *path_copy = strdup(path); // Make a copy to avoid modifying original
        token = strtok(path_copy, ":");
        while (token != NULL) {
            char *cmd_path = concat_path(token, args[0]);
            if (access(cmd_path, X_OK) == 0) {
                // Command found, execute it
                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                if (pid == 0) {
                    if (execve(cmd_path, args, NULL) == -1) {
                        perror("execve");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    wait(NULL);
                }
                free(cmd_path);
                break;
            }
            free(cmd_path);
            token = strtok(NULL, ":");
        }
        free(path_copy);

        // If command not found in PATH
        if (token == NULL) {
            printf("Command not found\n");
        }
    }

    return EXIT_SUCCESS;
}
