#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

int main() {
    int fd = 0, i = 0, nloop = 10;
    int zero = 0;
    char file[] = "tmp.txt";
    int *ptr = NULL;
    sem_t *mutex = NULL;

    fd = open(file, O_RDWR | O_CREAT, 0644);
    write(fd, &zero, sizeof(int));
    ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    mutex = sem_open("name_sem", O_RDWR | O_CREAT, 0644, 1);
    sem_unlink("name_sem");
    setbuf(stdout, NULL);

    if (fork() == 0) {
        for (i = 0; i < nloop; i ++) {
            sem_wait(mutex);
            printf("child: %d\n", (*ptr) ++);
            sem_post(mutex);
        }
        exit(0);
    }
    for (i = 0; i < nloop; i ++) {
        sem_wait(mutex);
        printf("parent: %d\n", (*ptr) ++);
        sem_post(mutex);
    }
    return 0;
}
