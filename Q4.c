#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigtstp_handler(int signum) {
    printf("\nGot SIGTSTP, suspending child\n");
}

void sigcont_handler(int signum) {
    printf("\nGot SIGCONT, resuming child\n");
}

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        printf("\nChild Process Running\n");
        printf("PID: %d\n", getpid());
        printf("Parent PID: %d\n", getppid());

        // Register signal handlers
        signal(SIGTSTP, sigtstp_handler);
        signal(SIGCONT, sigcont_handler);

        // Print information for 5 seconds
        for (int i = 0; i < 5; i++) {
            printf("Child Process Running %d\n", i+1);
            sleep(1);
        }

        printf("Child Process Suspended\n");
        pause();

        // Print information for another 5 seconds
        for (int i = 0; i < 5; i++) {
            printf("Child Process Running %d\n", i+6);
            sleep(1);
        }

        printf("Child Process Terminating\n");
        exit(0);
    } else {
        // Parent process
        sleep(5); // Wait for child to start
        printf("Sending SIGTSTP to child process with PID %d\n", pid);
        kill(pid, SIGTSTP);
        sleep(10); // Sleep for 10 seconds
        printf("Sending SIGCONT to child process with PID %d\n", pid);
        kill(pid, SIGCONT);

        // Wait for child to terminate
        int status;
        waitpid(pid, &status, 0);
        printf("Child process terminated\n");
    }

    return 0;
}
