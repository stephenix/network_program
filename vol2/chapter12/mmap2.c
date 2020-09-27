#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

struct shared {
    sem_t mutex;
    int count;
};

int main() {
    int fd = 0, i = 0, nloop = 10;
    int zero = 0;
    char file[] = "tmp.txt";
    //sem_t *mutex = NULL;
    struct shared *ptr = NULL;
    printf("parent addr:%llu\n", &ptr->mutex);

    fd = open(file, O_RDWR | O_CREAT, 0644);
    write(fd, &zero, sizeof(int));
    ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    // initialize semaphore that is shared between process
    sem_init(&ptr->mutex, 1, 1);
    setbuf(stdout, NULL);

    if (fork() == 0) {
        for (i = 0; i < nloop; i ++) {
            printf("child addr:%llu\n", &ptr->mutex);
            sem_wait(&ptr->mutex);
            printf("child: %d\n", ptr->count ++);
            sem_post(&ptr->mutex);
        }
        exit(0);
    }
    for (i = 0; i < nloop; i ++) {
        sem_wait(&ptr->mutex);
        printf("parent: %d\n", ptr->count ++);
        sem_post(&ptr->mutex);
    }
    return 0;
}
