#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

// Define the Process struct
typedef struct {
    char name[256];
    int priority;
    pid_t pid;
    int runtime;
} Process;

// Define the Queue node
typedef struct Node {
    Process process;
    struct Node* next;
} Node;

// Define the Queue structure
typedef struct {
    Node* front;
    Node* rear;
} Queue;

// Function prototypes
void push(Queue* queue, Process process);
Process pop(Queue* queue);
Process delete_name(Queue* queue, char* name);
Process delete_pid(Queue* queue, int pid);
void execute_process(Process* process);

int main() {
    FILE* file = fopen("processes_q5.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    Queue queue = { NULL, NULL };

    char name[256];
    int priority, runtime;

    // Read the processes from the file
    while (fscanf(file, "%255[^,], %d, %d\n", name, &priority, &runtime) == 3) {
        Process process;
        sprintf(process.name, "%s", name);
        process.priority = priority;
        process.pid = 0;
        process.runtime = runtime;
        push(&queue, process);
    }

    fclose(file);

    // Execute processes with priority 0 first
    while (queue.front != NULL && queue.front->process.priority == 0) {
        Process process = pop(&queue);
        execute_process(&process);
    }

    // Execute remaining processes
    while (queue.front != NULL) {
        Process process = pop(&queue);
        execute_process(&process);
    }

    // Cleanup the queue
    Node* current = queue.front;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}

void push(Queue* queue, Process process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (queue->front == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

Process pop(Queue* queue) {
    if (queue->front == NULL) {
        Process process = { "", 0, 0, 0 };
        return process;
    }

    Node* temp = queue->front;
    Process process = temp->process;
    queue->front = queue->front->next;
    free(temp);

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return process;
}

Process delete_name(Queue* queue, char* name) {
    Node* current = queue->front;
    Node* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->process.name, name) == 0) {
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                queue->front = current->next;
            }

            if (queue->rear == current) {
                queue->rear = prev;
            }

            Process process = current->process;
            free(current);
            return process;
        }

        prev = current;
        current = current->next;
    }

    Process process = { "", 0, 0, 0 };
    return process;
}

Process delete_pid(Queue* queue, int pid) {
    Node* current = queue->front;
    Node* prev = NULL;

    while (current != NULL) {
        if (current->process.pid == pid) {
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                queue->front = current->next;
            }

            if (queue->rear == current) {
                queue->rear = prev;
            }

            Process process = current->process;
            free(current);
            return process;
        }

        prev = current;
        current = current->next;
    }

    Process process = { "", 0, 0, 0 };
    return process;
}

void execute_process(Process* process) {
    // Fork process
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        printf("Executing %s with PID %d\n", process->name, getpid());
        // Execute process
        execlp(process->name, process->name, NULL);
        // If execlp returns, it failed
        perror("Exec failed");
        exit(1);
    } else if (pid > 0) {
                // Parent process
        process->pid = pid;
        sleep(process->runtime);
        kill(pid, SIGINT);
        int status;
        waitpid(pid, &status, 0);
        printf("Process %s with PID %d terminated\n", process->name, pid);
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n",
               process->name, process->priority, process->pid, process->runtime);
    } else {
        perror("Fork failed");
    }
}

