#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define NAME_SIZE 256
#define MAX_PROCESSES 10

// Process struct
typedef struct {
    char name[NAME_SIZE];
    int priority;
    pid_t pid;
    int runtime;
} Process;

// Linked list node
typedef struct Node {
    Process process;
    struct Node* next;
} Node;

// Queue struct
typedef struct {
    Node* front;
    Node* rear;
    int size;
} Queue;

// Function prototypes
void push(Queue* queue, Process process);
Process pop(Queue* queue);
void delete_name(Queue* queue, const char* name);
void delete_pid(Queue* queue, pid_t pid);
void execute_process(Process* process);

int main() {
    FILE* file = fopen("processes_q5.txt", "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Queue queue = { .front = NULL, .rear = NULL, .size = 0 };

    char name[NAME_SIZE];
    int priority, runtime;
    while (fscanf(file, "%255[^,],%d,%d\n", name, &priority, &runtime) == 3) {
        Process process = { .priority = priority, .runtime = runtime, .pid = 0 };
        strcpy(process.name, name);
        push(&queue, process);
    }
    fclose(file);

    Node* current = queue.front;
    while (current != NULL) {
        if (current->process.priority == 0) {
            Process process = pop(&queue);
            execute_process(&process);
            waitpid(process.pid, NULL, 0);
            printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", process.name, process.priority, process.pid, process.runtime);
        }
        current = current->next;
    }

    while (queue.size > 0) {
        Process process = pop(&queue);
        execute_process(&process);
        waitpid(process.pid, NULL, 0);
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n", process.name, process.priority, process.pid, process.runtime);
    }

    return 0;
}

void push(Queue* queue, Process process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (queue->size == 0) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->size++;
}

Process pop(Queue* queue) {
    if (queue->size == 0) {
        fprintf(stderr, "Queue is empty\n");
        exit(EXIT_FAILURE);
    }

    Node* frontNode = queue->front;
    Process process = frontNode->process;

    if (queue->size == 1) {
        queue->front = NULL;
        queue->rear = NULL;
    } else {
        queue->front = frontNode->next;
    }

    free(frontNode);
    queue->size--;

    return process;
}

void delete_name(Queue* queue, const char* name) {
    Node* current = queue->front;
    Node* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->process.name, name) == 0) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }

            if (current == queue->rear) {
                queue->rear = prev;
            }

            queue->size--;
            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}

void delete_pid(Queue* queue, pid_t pid) {
    Node* current = queue->front;
    Node* prev = NULL;

    while (current != NULL) {
        if (current->process.pid == pid) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }

            if (current == queue->rear) {
                queue->rear = prev;
            }

            queue->size--;
            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}

void execute_process(Process* process) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        char runtime_str[10];
        sprintf(runtime_str, "%d", process->runtime);
        execl("./process", "./process", process->name, runtime_str, NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        process->pid = pid;
    }
}
