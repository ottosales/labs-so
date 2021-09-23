#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE_STRING 1024

int main() {
    pid_t childPIDs[2]; // para cada um dos child pids
    int fd[3][2];
    srand(time(NULL));

    if (pipe(fd[0]) < 0) {
        printf("error creating pipe 0\n");
        return 1;
    }

    if (pipe(fd[1]) < 0) {
        printf("error creating pipe 1\n");
        return 1;
    }

    if (pipe(fd[2]) < 0) {
        printf("error creating pipe 2\n");
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        childPIDs[i] = fork(); // b
        if (childPIDs[i] == -1) {
            printf("error creating child process %d\n", i);
            return 1;
        }
        if (childPIDs[i] == 0 && i == 0) {
            close(fd[1][0]); // reading end of pipe c1 - c2
            close(fd[0][1]); // writting end of pipe parent - c1
            close(fd[2][0]); // reading end of pipe c2 - parent
            close(fd[2][1]); // writting end of pipe c2 - parent
            
            int x = 0;
            char message[SIZE_STRING + 1] = {0};
            
            read(fd[0][0], message, SIZE_STRING + 1);
            read(fd[0][0], &x, sizeof(int));
            
            close(fd[0][0]); // reading end of pipe parent - c1 (not needed anymore)

            printf("[child %d] x = %d\n", i, x);
            printf("[child %d] %s\n", i, message);

            int arraySize = 1 + rand() % 10;
            int array[arraySize];
            for (int j = 0; j < arraySize; j++) {
                array[j] = 1 + rand() % x;
            }
            printf("[child %d] ", i);
            fflush(stdout);
            for (int j = 0; j < arraySize; j++) {
                printf("%d ", array[j]);
                fflush(stdout);
            }
            printf("\n");

            write(fd[1][1], &arraySize, sizeof(int));
            write(fd[1][1], array, arraySize * sizeof(int));

            close(fd[1][1]); // writting end of pipe c1 - c2 (not needed anymore)

            exit(0);
        } else if (childPIDs[i] == 0 && i == 1) {
            close(fd[0][0]); // reading end of pipe parent - c1
            close(fd[0][1]); // writting end of pipe parent - c1
            close(fd[1][1]); // writting end of pipe c1 - c2
            close(fd[2][0]); // reading end of pipe c2 - parent
            
            int arraySize = 0;
            read(fd[1][0], &arraySize, sizeof(int));
            printf("[child %d] array size received: %d\n", i, arraySize);

            int array[arraySize];
            read(fd[1][0], array, arraySize * sizeof(int));
            printf("[child %d] ", i);
            fflush(stdout);
            for (int j = 0; j < arraySize; j++) {
                printf("%d ", array[j]);
                fflush(stdout);
            }
            printf("\n");

            close(fd[1][0]); // reading end of pipe c1 - c2 (not needed anymore)

            int sum = 0;

            for (int j = 0; j < arraySize; j++) {
                sum += array[j];
            }

            printf("[child %d] sum: %d\n", i, sum);
            write(fd[2][1], &sum, sizeof(int));
            close(fd[2][1]); // writting end of pipe c2 - parent (not needed anymore)

            exit(0);
        }
    }

    close(fd[0][0]); // reading end of pipe parent - c1
    close(fd[1][0]); // reading end of pipe c1 - c2
    close(fd[1][1]); // writting end of pipe c1 - c2
    close(fd[2][1]); // writting end of pipe c2 - parent

    int x = 0;
    char message[SIZE_STRING + 1] = {0};
    snprintf(message, SIZE_STRING + 1, "Meu filho, crie e envie para o seu irmão um array de números inteiros com valores randômicos entre 1 e o valor enviado anteriormente. O tamanho do array também deve ser randômico, na faixa de 1 a 10");
    printf("[parent] x value goes here: ");
    scanf("%d", &x);
    fflush(stdout);

    write(fd[0][1], message, SIZE_STRING + 1);
    write(fd[0][1], &x, sizeof(int));

    close(fd[0][1]);

    int sum = 0;    
    read(fd[2][0], &sum, sizeof(int));
    printf("[parent] sum value: %d\n", sum);

    close(fd[2][0]);

    wait(NULL);

    // fifo to ping to ufes.br
    if (mkfifo("PipePing.txt", 0666) == -1) {
        if (errno != EEXIST) {
            printf("error creating fifo :(\n");
            return 1;
        }
    }
    pid_t pid = fork();
    if (pid == -1) {
        printf("error creating fifo child process");
        return 1;
    }
    if (pid == 0) { // child 3
        int fifofd = open("PipePing.txt", O_WRONLY);
        printf("[child fifo] executing command ping\n");
        dup2(fifofd, STDOUT_FILENO);
        execlp("ping", "ping", "ufes.br", "-c", "5", NULL);
    }

    wait(NULL);

    FILE* fifofd = fopen("PipePing.txt", "r");

    char line[SIZE_STRING + 1];
    printf("[parent] read from fifo:\n");
    while (fgets(line, sizeof(line), fifofd) != NULL) {
        printf("%s", line);
    }
    fclose(fifofd);

    return 0;
}
