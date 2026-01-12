#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Configuration
#define NUM_THREADS 8
#define NUM_ITERATIONS 10000
#define MAX_ALLOC_SIZE 4096

typedef struct {
    int thread_id;
} thread_arg_t;

void* worker(void* arg) {
    int id = ((thread_arg_t*)arg)->thread_id;
    printf("Thread %d starting...\n", id);

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        // 1. Pick a random size
        size_t size = (rand() % MAX_ALLOC_SIZE) + 1;

        // 2. Allocate
        volatile char* ptr = (char*)malloc(size);
        if (!ptr) {
            printf("Thread %d: malloc failed!\n", id);
            exit(1);
        }
        // printf("Thread %d: Allocated %zu bytes.\n", id, size);

        // 3. Touch memory (force the page to actually be mapped)
        ptr[0] = (char)i;
        ptr[size - 1] = (char)i;

        // 4. Free
        free((void*)ptr);
    }

    printf("Thread %d finished %d allocations.\n", id, NUM_ITERATIONS);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    printf("Starting Stress Test: %d Threads, %d Iterations each.\n", NUM_THREADS, NUM_ITERATIONS);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads completed successfully.\n");
    return 0;
}
