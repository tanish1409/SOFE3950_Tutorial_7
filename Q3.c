#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigint_handler(int signum) {
    printf("\nReceived SIGINT, terminating child process\n");
    exit(0);
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

        // Register SIGINT handler
        signal(SIGINT, sigint_handler);

        // Print information for 5 seconds
        for (int i = 0; i < 5; i++) {
            printf("Child Process Running %d\n", i+1);
            sleep(1);
        }

        printf("Child Process Terminating\n");
        exit(0);
    } else {
        // Parent process
        sleep(5); // Wait for child to start
        printf("Sending SIGINT to child process with PID %d\n", pid);
        kill(pid, SIGINT);
        wait(NULL); // Wait for child to terminate
        printf("Child process terminated\n");
    }

    return 0;
}
