#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

int distance(int size, int path[size], int matrix[size][size])
{
	int dist = 0;
	for(int i = 0; i < size - 1; i++)
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

int* createShmem(int size)
{
	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_SHARED | MAP_ANONYMOUS;
	return mmap(NULL, sizeof(int) * size, protection, visibility, 0, 0);
}

int main()
{
	int dist1 = 0;
	int dist2 = 0;

    bool update = false;

	sem_unlink("calculate");
	sem_unlink("memory");
    sem_unlink("newWay");
	sem_t *sem1 = sem_open("calculate", O_CREAT, 0644, 0);
	sem_t *sem2 = sem_open("memory", O_CREAT, 0644, 1);
    sem_t *sem3 = sem_open("newWay", O_CREAT, 0644, 0);

	int* shmem = createShmem(64);

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

    int shortWay = 0;

	for(int i = 0; i < numProcess; i++)
	{
		int pid = fork();

		if(pid == 0)
		{
            sem_wait(sem2);
			swap(size, path);
			dist1 = distance(size, path, matrix);

			if(shortWay == 0 || shortWay > dist1)
            {
                sem_post(sem3);
                sem_wait(sem1);
                shmem[0] = dist1;
                shortWay = dist1;
                update = false;
                sem_post(sem2);
            }
            else
                sem_post(sem2);
		}
		else
		{
            sem_wait(sem3);
            update = true;
            sem_post(sem1);
		}
	}

	printf("A Melhor Solução é: %d\n", shmem[0]);

	sem_close(sem1);
	sem_close(sem2);

	return (EXIT_SUCCESS);
}
