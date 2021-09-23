
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* printHello(void* data) {
    int myData = (int)data;

    pthread_detach(pthread_self());
    printf("Hello from new thread - got %d\n", myData);
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

    printf("Created new thread (%u)\n", thread_id);

    pthread_exit(NULL);
}