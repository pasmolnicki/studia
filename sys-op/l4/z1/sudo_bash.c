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
    // gcc -o sb sudo_bash.c
    // sudo chown root:root sb
    // sudo chmod u+s sb
    // id
    
    pid_t pid = fork();
    if (pid == 0) {
        // -p for preserve privileges
        execlp("bash", "bash", "-p", NULL);

        // This will only executed if exec fails
        if (errno == ENOENT) {
            fprintf(stderr, "command not found: %s\n", "bash");
        } else {
            fprintf(stderr, "exec error: %s\n", strerror(errno));
        }

        exit(1);
    } else if (pid > 0) {
        while (wait(NULL) > 0);
    } else {
        perror("sudo_bash: fork failed\n");
    }

    printf("Exiting sudo_bash\n");

    return 0;
}