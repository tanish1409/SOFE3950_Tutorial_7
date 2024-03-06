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

Proc pop(Queue *queue) {
    if (queue->front == NULL) {
        fprintf(stderr, "Error: Queue is empty\n");
        exit(EXIT_FAILURE);
    }

    Node *temp = queue->front;
    Proc process = temp->process;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    return process;
}

Proc *delete_name(Queue *queue, const char *name) {
    Node *current = queue->front;
    Node *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->process.name, name) == 0) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }
            if (queue->rear == current) {
                queue->rear = prev;
            }
            return &current->process;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
}

Proc *delete_pid(Queue *queue, int pid) {
    Node *current = queue->front;
    Node *prev = NULL;

    while (current != NULL) {
        if (current->process.pid == pid) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }
            if (queue->rear == current) {
                queue->rear = prev;
            }
            return &current->process;
        }
        prev = current;
        current = current->next;
    }

    return NULL;
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

    // Delete process named "emacs"
    Proc *deleted_proc = delete_name(&queue, "emacs");
    if (deleted_proc != NULL) {
        printf("Deleted process named emacs\n");
    } else {
        printf("Process named emacs not found\n");
    }

    // Delete process with pid 12235
    deleted_proc = delete_pid(&queue, 12235);
    if (deleted_proc != NULL) {
        printf("Deleted process with pid 12235\n");
    } else {
        printf("Process with pid 12235 not found\n");
    }

    // Print remaining processes
    printf("Remaining processes:\n");
    printQueue(&queue);

    // Pop remaining processes
    printf("Popping remaining processes:\n");
    while (queue.front != NULL) {
        Proc process = pop(&queue);
        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n",
               process.name, process.priority, process.pid, process.runtime);
    }

    return 0;
}
