#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <sys/wait.h>

int distance(int size, int path[size], int matrix[size][size])
{
    int dist = 0;
    for (int i = 0; i < size - 1; i++)
    {
        int curr = path[i] - 1;
        int next = path[i + 1] - 1;
        dist += matrix[curr][next];
    }
    int last = path[size - 1] - 1;
    int first = path[0] - 1;
    dist += matrix[last][first];
    return dist;
}

void swap(int size, int path[size])
{
    int a = rand() % size;
    int b = rand() % size;
    int tmp = path[a];
    path[a] = path[b];
    path[b] = tmp;
}

int *createShmem(int size)
{
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    return mmap(NULL, sizeof(int) * size, protection, visibility, 0, 0);
}

int main()
{
    int dist1 = 0;
    int dist2 = 0;
    int i = 0;

    sem_unlink("openMem");
    sem_unlink("closeMem");
    sem_t *sem1 = sem_open("openMem", O_CREAT, 0644, 1);
    sem_t *sem2 = sem_open("closeMem", O_CREAT, 0644, 0);

    int *shmem = createShmem(64);

    int numProcess;
    printf("Introduza o número de processos que deseja criar: ");
    scanf("%d", &numProcess);

    srand(time(NULL));

    int size = 5;

    int matrix[5][5] =
        {
            {0, 23, 10, 4, 1},
            {23, 0, 9, 5, 4},
            {10, 9, 0, 8, 2},
            {4, 5, 8, 0, 11},
            {1, 4, 2, 11, 0},
        };

    int path[] = {1, 2, 3, 4, 5};

    int pid = fork();
    char num[258];

    for (int i = 0; i < numProcess; i++)
    {

        if (pid == 0)
        {
            swap(size, path);
            dist1 = distance(size, path, matrix);
            sem_wait(sem1);
            if (shmem[0] == 0 || shmem[0] > dist1)
                shmem[0] = dist1;
            sem_post(sem2);
        }
        else
        {
            swap(size, path);
            dist2 = distance(size, path, matrix);
            sem_wait(sem2);
            if (shmem[0] == 0 || shmem[0] > dist2)
                shmem[0] = dist2;
            sem_post(sem1);
        }
    }
    sem_close(sem1);
    sem_close(sem2);

    printf("Distancia: %d\n", shmem[0]);
    sleep(2);

    return (EXIT_SUCCESS);
    
}
