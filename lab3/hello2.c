#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* printHello(void* data) {
    pthread_detach(pthread_self());
    printf("Hello from new thread (%u) - got %u\n", (int)pthread_self(), (int)data);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int rc;
    pthread_t thread_id;
    int t = 11;

    rc = pthread_create(&thread_id, NULL, printHello, (void*)t);

    if (rc) {
        printf("\nFailed to create thread! return code from pthread_create is %d\n", rc);
        exit(1);
    }

    printf("I am thread (%u) and I created a new thread (%u)\n", (int)pthread_self(), (int)thread_id);

    pthread_exit(NULL);
}
