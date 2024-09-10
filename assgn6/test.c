#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Thread function
void* func(void* args) {
    // Print thread ID
    printf("Thread ID: %lu\n", (unsigned long)pthread_self());

    // Print the argument passed to the thread
    printf("Thread Argument: %s\n", (char*)args);

    // Allocate memory dynamically for return value
    int* result = malloc(sizeof(int));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        pthread_exit(NULL);
    }
    *result = 10;
    
    return result;
}

int main() {
    pthread_t tid;

    char* junk = "This is nukes";
    int ret = pthread_create(&tid, NULL, func, junk);
    if (ret != 0) {
        fprintf(stderr, "Failed to create thread: %d\n", ret);
        return 1;
    }

    printf("Main Thread ID: %lu\n", (unsigned long)pthread_self());
    
    void* res;
    ret = pthread_join(tid, &res);
    if (ret != 0) {
        fprintf(stderr, "Failed to join thread: %d\n", ret);
        return 1;
    }

    printf("Returned value from thread: %d\n", *((int*)res));

    free(res);

    return 0;
}
