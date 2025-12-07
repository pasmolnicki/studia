#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>


int main() {
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process, run bash with sudo privileges - because file
        // has setuid bit set
        execlp("bash", "bash", NULL);

        // This will only executed if exec fails
        if (errno == ENOENT) {
            fprintf(stderr, "lsh: command not found: %s\n", "bash");
        } else {
            fprintf(stderr, "lsh: exec error: %s\n", strerror(errno));
        }

        exit(1);
    } else if (pid > 0) {
        while (waitpid(pid, NULL, 0) > 0);
    } else {
        perror("sudo_bash: fork failed\n");
    }

    return 0;
}