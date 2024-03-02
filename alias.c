#include "shell.h"

#define MAX_ALIASES 100
#define ALIAS_SIZE 1024

/* Structure to store aliases */
typedef struct {
    char name[ALIAS_SIZE];
    char value[ALIAS_SIZE];
} Alias;

Alias aliases[MAX_ALIASES];
int num_aliases = 0;

void handle_alias(char *arg) {
    if (arg == NULL) {
        // Print all aliases
        for (int i = 0; i < num_aliases; i++) {
            printf("%s='%s'\n", aliases[i].name, aliases[i].value);
        }
    } else {
        // Check if argument contains '='
        char *equals = strchr(arg, '=');
        if (equals != NULL) {
            // Extract alias name and value
            *equals = '\0';
            char *name = arg;
            char *value = equals + 1;

            // Check if alias already exists
            for (int i = 0; i < num_aliases; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    strcpy(aliases[i].value, value);
                    return;
                }
            }

            // Add new alias
            if (num_aliases < MAX_ALIASES) {
                strcpy(aliases[num_aliases].name, name);
                strcpy(aliases[num_aliases].value, value);
                num_aliases++;
            } else {
                fprintf(stderr, "Too many aliases\n");
            }
        } else {
            // Print specific alias
            for (int i = 0; i < num_aliases; i++) {
                if (strcmp(aliases[i].name, arg) == 0) {
                    printf("%s='%s'\n", aliases[i].name, aliases[i].value);
                    return;
                }
            }
            fprintf(stderr, "alias: %s: not found\n", arg);
        }
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
            } else if (strcmp(token, "alias") == 0) {
                char *arg = strtok(NULL, "");
                handle_alias(arg);
            }
        }

        // Free memory allocated for input
        free(input);
    }

    return EXIT_SUCCESS;
}
