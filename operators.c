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
    char *dir = arg;

    // If no argument provided, change to the home directory
    if (dir == NULL || strcmp(dir, "~") == 0) {
        dir = getenv("HOME");
    }

    if (chdir(dir) == -1) {
        perror("cd");
    } else {
        // Update the PWD environment variable
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            setenv("PWD", cwd, 1);
        } else {
            perror("getcwd");
        }
    }
}

void execute_command(char *command) {
    char *token;
    char *delim = " ;&";
    int status = 0;
    int execute_next = 1;

    token = strtok(command, delim);

    while (token != NULL) {
        if (strcmp(token, "&&") == 0) {
            execute_next = (status == 0);
        } else if (strcmp(token, "||") == 0) {
            execute_next = (status != 0);
        } else {
            pid_t pid = fork();

            if (pid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) { // Child process
                if (execlp(token, token, NULL) == -1) {
                    perror(token);
                    exit(EXIT_FAILURE);
                }
            } else { // Parent process
                waitpid(pid, &status, 0);
            }
        }

        if (!execute_next) {
            break;
        }

        token = strtok(NULL, delim);
    }
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
            } else {
                execute_command(input);
            }
        }

        //
