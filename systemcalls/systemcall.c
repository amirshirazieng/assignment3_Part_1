#include "systemcalls.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h> 


bool do_system(const char *cmd)
{
    int result = system(cmd);

    if (result == 0) {
        return true; // Command executed successfully
    } else {
        return false; // Command execution failed
    }
}

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++) {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("Fork failed");
        return false;
    } else if (child_pid == 0) {
        
        execv(command[0], command);
        perror("Execv failed"); // If execv returns, there was an error
        exit(1);
    } else {
        
        int status;
        waitpid(child_pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }

    va_end(args);
}



bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++) {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("Fork failed");
        return false;
    } else if (child_pid == 0) {
        // This code executes in the child process
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            perror("Open failed");
            exit(1);
        }
        dup2(fd, STDOUT_FILENO); // Redirect stdout to the output file
        close(fd);

        execv(command[0], command);
        perror("Execv failed"); // If execv returns, there was an error
        exit(1);
    } else {
        // This code executes in the parent process
        int status;
        waitpid(child_pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }

    va_end(args);
}




