#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

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

char **tokenize_input(char *input) {
    char **tokens = NULL;
    char *token = strtok(input, " \t\r\n\a");
    int index = 0;

    while (token != NULL) {
        tokens = realloc(tokens, (index + 1) * sizeof(char *));
        if (tokens == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        tokens[index] = strdup(token);
        if (tokens[index] == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        index++;
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens = realloc(tokens, (index + 1) * sizeof(char *));
    if (tokens == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    tokens[index] = NULL;
    return tokens;
}

int main(void) {
    char *input;
    char **tokens;

    while (1) {
        printf("$ ");
        fflush(stdout);

        input = custom_getline();
        if (input == NULL) {
            printf("\n");
            break;
        }

        tokens = tokenize_input(input);

        int i = 0;
        while (tokens[i] != NULL) {
            printf("Token %d: %s\n", i, tokens[i]);
            free(tokens[i]);
            i++;
        }
        free(tokens);

        // Free memory allocated for input
        free(input);
    }

    return EXIT_SUCCESS;
}
