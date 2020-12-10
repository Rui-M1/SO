#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

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

int main()
{
	int dist1 = 0;
	int dist2 = 0;

	sem_unlink("openMem2");
	sem_unlink("closeMem2");
	sem_t *sem1 = sem_open("openMem2", O_CREAT, 0644, 0);
	sem_t *sem2 = sem_open("closeMem2", O_CREAT, 0644, 1);

	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	void *shmem = mmap(NULL, 64, protection, visibility, 0, 0);

	int numProcess;
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

	for(int i = 0; i < numProcess; i++)
	{
		int pid = fork();
		char *num;

		if(pid == 0)
		{
			sem_wait(sem2);
			swap(size, path);
			dist1 = distance(size, path, matrix);
			sem_post(sem1);
		}
		else
		{
			sem_wait(sem1);
			swap(size, path);
			dist2 = distance(size, path, matrix);
			sem_post(sem2);
		}

		if(shmem == NULL)
		{
			if(dist1 > dist2)
			{
				sprintf(num, "%d", dist2);
				strcpy(shmem, num);
			}
			else
			{
				sprintf(num, "%d", dist1);
				strcpy(shmem, num);
			}
		}
		else
		{
			int shmemValue = atoi((char*) shmem);

			if(dist1 > dist2 && shmemValue > dist2 && shmemValue > dist1)
			{
				sprintf(num, "%d", dist2);
				strcpy(shmem, num);
			}
			else if(dist1 < dist2 && shmemValue > dist2 && shmemValue > dist1)
			{
				sprintf(num, "%d", dist1);
				strcpy(shmem, num);
			}
		}

		printf("Distancia: %s\n",(char*) shmem);
	}

	sem_close(sem1);
	sem_close(sem2);

	return (EXIT_SUCCESS);
}
