#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

char *custom_getline() {
    static char buffer[BUFFER_SIZE];
    static int position = 0;
    static int bytes_read = 0;

    char *line = NULL;
    int length = 0;

    while (1) {
        // Read more characters if buffer is empty
        if (position == bytes_read) {
            bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if (bytes_read <= 0) {
                return NULL; // Return NULL on EOF or error
            }
            position = 0;
        }

        // Find newline character or end of buffer
        while (position < bytes_read && buffer[position] != '\n') {
            position++;
            length++;
        }

        // Allocate memory for the line
        line = realloc(line, length + 1);
        if (line == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        // Copy characters from buffer to line
        int i = 0;
        while (i < length && buffer[position - length + i] != '\n') {
            line[i] = buffer[position - length + i];
            i++;
        }
        line[i] = '\0'; // Null-terminate the line

        // Update position for next iteration
        position++;

        // If newline character found, return the line
        if (i < length || buffer[position - 1] == '\n') {
            return line;
        }
    }
}

void handle_exit(char *arg) {
    if (arg == NULL) {
        exit(EXIT_SUCCESS); // No argument provided, exit with success status
    }

    // Convert argument to integer
    char *endptr;
    errno = 0;
    long status = strtol(arg, &endptr, 10);

    // Check for conversion errors or remaining characters after the number
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Invalid exit status: %s\n", arg);
        exit(EXIT_FAILURE);
    }

    // Exit with the provided status
    exit((int)status);
}

int main(void) {
    char *input;

    while (1) {
        printf("$ ");
        fflush(stdout);

        input = custom_getline();
        if (input == NULL) {
            printf("\n");
            break;
        }

        // Tokenize input to handle built-in commands
        char *token = strtok(input, " ");
        if (token != NULL && strcmp(token, "exit") == 0) {
            token = strtok(NULL, " ");
            handle_exit(token);
        }

        // Free memory allocated for input
        free(input);
    }

    return EXIT_SUCCESS;
}
