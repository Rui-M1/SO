#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
int main()
{

    srand(time(NULL));

    int size = 5;
    int matrix[5][5] = {
        {0, 23, 10, 4, 1},
        {23, 0, 9, 5, 4},
        {10, 9, 0, 8, 2},
        {4, 5, 8, 0, 11},
        {1, 4, 2, 11, 0},
    };
    int path[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 10; i++)
    {
        swap(size, path);
        int dist = distance(size, path, matrix);
        printf("Distance: %d\n", dist);
    }
}
