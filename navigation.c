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

void handle_setenv(char *arg1, char *arg2) {
    if (arg1 == NULL || arg2 == NULL) {
        fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
        return;
    }

    if (setenv(arg1, arg2, 1) == -1) {
        perror("setenv");
    }
}

void handle_unsetenv(char *arg) {
    if (arg == NULL) {
        fprintf(stderr, "Usage: unsetenv VARIABLE\n");
        return;
    }

    if (unsetenv(arg) == -1) {
        perror("unsetenv");
    }
}

void handle_cd(char *arg) {
    char *new_dir;

    if (arg == NULL || strcmp(arg, "~") == 0) {
        new_dir = getenv("HOME");
    } else if (strcmp(arg, "-") == 0) {
        new_dir = getenv("OLDPWD");
    } else {
        new_dir = arg;
    }

    if (new_dir == NULL) {
        fprintf(stderr, "cd: No home directory\n");
        return;
    }

    // Save current directory for 'cd -'
    char *old_dir = getcwd(NULL, 0);
    if (old_dir == NULL) {
        perror("getcwd");
        return;
    }

    // Change directory
    if (chdir(new_dir) == -1) {
        perror("chdir");
        free(old_dir);
        return;
    }

    // Update PWD and OLDPWD environment variables
    if (setenv("OLDPWD", old_dir, 1) == -1 || setenv("PWD", new_dir, 1) == -1) {
        perror("setenv");
        free(old_dir);
        return;
    }

    free(old_dir);
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
        if (token != NULL) {
            if (strcmp(token, "exit") == 0) {
                token = strtok(NULL, " ");
                handle_exit(token);
            } else if (strcmp(token, "setenv") == 0) {
                char *arg1 = strtok(NULL, " ");
                char *arg2 = strtok(NULL, " ");
                handle_setenv(arg1, arg2);
            } else if (strcmp(token, "unsetenv") == 0) {
                char *arg = strtok(NULL, " ");
                handle_unsetenv(arg);
            } else if (strcmp(token, "cd") == 0) {
                char *arg = strtok(NULL, " ");
                handle_cd(arg);
            }
        }

        // Free memory allocated for input
        free(input);
    }

    return EXIT_SUCCESS;
}
