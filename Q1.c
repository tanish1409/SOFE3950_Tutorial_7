#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 256

typedef struct proc {
    char name[LENGTH];
    int priority;
    int pid;
    int runtime;
} Proc;

typedef struct node {
    Proc process;
    struct node *next;
} Node;

typedef struct {
    Node *front;
    Node *rear;
} Queue;

void push(Queue *queue, Proc process) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->process = process;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = newNode;
    } else {
        queue->rear->next = newNode;
    }
    queue->rear = newNode;
}

void printQueue(Queue *queue) {
    Node *current = queue->front;
    while (current != NULL) {
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n",
               current->process.name, current->process.priority,
               current->process.pid, current->process.runtime);
        current = current->next;
    }
}

int main() {
    FILE *file = fopen("processes.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Failed to open file\n");
        return 1;
    }

    Queue queue = { .front = NULL, .rear = NULL };
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        Proc process;
        sscanf(line, "%[^,],%d,%d,%d", process.name, &process.priority,
               &process.pid, &process.runtime);
        push(&queue, process);
    }

    fclose(file);

    printQueue(&queue);

    return 0;
}
