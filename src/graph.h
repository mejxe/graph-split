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

// Partitioning functions
bool** allocateFinalPartitions(int k, int n);
void freeFinalPartitions(bool** partitions, int k);
void kWayPartition(Graph* g, int k, bool** finalPartitions, double earlyStopThreshold, double balanceMargin);
int calculateTotalCutSize(Graph* g, bool** partitions, int k);

#endif /* GRAPH_H */
