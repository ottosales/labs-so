#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc , char * argv []) {
    int n;

    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        if(!fork()) {
            printf("processo %d, filho de %d\n", getpid(), getppid());
            return 0;
        }
    }
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    return 0;
}