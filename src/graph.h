
#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>



// Edge structure for adjacency list representation
typedef struct Edge {
    int dest;
    struct Edge* next;
} Edge;

// Graph structure
typedef struct {
    int vertices;
    Edge** adjList;
    int** adjMatrix;
    int* totalEdgeWeights;
} Graph;

// Core graph functions
Graph* initGraph(int n);
void addEdge(Graph* g, int src, int dest);
void freeGraph(Graph* g);
#endif /* GRAPH_H */

