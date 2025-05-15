#include <stdio.h>
#include <string.h>
#include <assert.h>


void print_matrix(int size, int matrix[size][size])
{
    for(int i = 0; i< size; ++i)
    {
        for (int j = 0; j < size; j++)
        {
            int const value = matrix[i][j];
            printf("%3d ", value);
        }

        printf("\n");
    }
}

#define NOF_VERTICES 10
typedef struct Edge
{
    int from;
    int to;
    int weight;
} Edge;


void generate_adjacency_matrix(int nov, Edge const *edge, int adjmx[nov][nov])
{
    assert(nov > 1);
    assert(edge != NULL && edge[0].weight != -1);
    assert(adjmx != NULL);

    int idx = 0;
    while (edge[idx].weight != -1)
    {
        Edge const e = edge[idx++];
        
        assert(e.from >= 0 && e.from < nov);
        assert(e.to >= 0 && e.to < nov);
        assert(e.weight > 0);
        assert(e.from != e.to);
        assert(adjmx[e.from][e.to] == 0);
        
        adjmx[e.from][e.to] = e.weight;
        adjmx[e.to][e.from] = -e.weight;
    }
    

}

int main(void)
{
    int adjmx[NOF_VERTICES][NOF_VERTICES] = { 0 };

    Edge const edges[] = {
        {0, 1, 10},
        {1, 2, 20},
        {2, 3, 30},
        {3, 4, 40},
        {4, 5, 50},
        {5, 6, 60},
        {6, 7, 70},
        {-1, -1, -1},
    };

    generate_adjacency_matrix(NOF_VERTICES, edges, adjmx);
    printf("\n");

    print_matrix(NOF_VERTICES, adjmx);

    return 0;
}