#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREADS 20

pthread_t thread_id[MAX_THREADS];

void* printHello(void* data) {
    printf("Hello from new thread (%u) - I was created in iteration %d\n", (int)pthread_self(), (int)data);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("parameter needed >:^(\n");
        return 1;
    }

    int nThreads = atoi(argv[1]);
    if (nThreads > MAX_THREADS) nThreads = MAX_THREADS;
    if (nThreads < 1) nThreads = 1;

    for (int i = 0; i < nThreads; i++) {
        int rc = pthread_create(&thread_id[i], NULL, printHello, (void*)i);

        if (rc) {
            printf("\nFailed to create thread! return code from pthread_create is %d\n", rc);
            exit(1);
        }

        sleep(1);
        printf("I am thread (%u) and I created a new thread (%u)\n", (int)pthread_self(), (int)thread_id[i]);
    }

    pthread_exit(NULL);
}