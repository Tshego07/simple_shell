#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

/* Function prototypes */
char *custom_getline(void);
void handle_exit(char *arg);
void handle_setenv(char *arg1, char *arg2);
void handle_unsetenv(char *arg);

#endif /* SHELL_H */
