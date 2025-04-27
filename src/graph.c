#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include "graph.h"







Graph* initGraph(int n) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }

    g->vertices = n;
    g->adjList = (Edge**)malloc(n * sizeof(Edge*));
    g->adjMatrix = (int**)malloc(n * sizeof(int*));
    g->totalEdgeWeights = (int*)calloc(n, sizeof(int));
    
    if (!g->adjList || !g->adjMatrix || !g->totalEdgeWeights) {
        if (g->adjList) free(g->adjList);
        if (g->adjMatrix) free(g->adjMatrix);
        if (g->totalEdgeWeights) free(g->totalEdgeWeights);
        free(g); exit(1);
    }

    for (int i = 0; i < n; i++) {
        g->adjList[i] = NULL;
        g->adjMatrix[i] = (int*)calloc(n, sizeof(int));
        if (!g->adjMatrix[i]) {
            for (int j = 0; j < i; j++) free(g->adjMatrix[j]);
            free(g->adjMatrix); free(g->totalEdgeWeights); 
            free(g->adjList); free(g); exit(1);
        }
    }
    return g;
}

void addEdge(Graph* g, int src, int dest) {
    // Update matrix
    g->adjMatrix[src][dest] = g->adjMatrix[dest][src] = 1;

    // Add edge src -> dest
    Edge* e1 = (Edge*)malloc(sizeof(Edge));
    if (!e1) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    e1->dest = dest; e1->next = g->adjList[src]; g->adjList[src] = e1;

    // Add edge dest -> src
    Edge* e2 = (Edge*)malloc(sizeof(Edge));
    if (!e2) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    e2->dest = src; e2->next = g->adjList[dest]; g->adjList[dest] = e2;

    // Update weights
    g->totalEdgeWeights[src]++; g->totalEdgeWeights[dest]++;
}

void freeGraph(Graph* g) {
    if (!g) return;
    for (int i = 0; i < g->vertices; i++) {
        Edge* current = g->adjList[i];
        while (current) {
            Edge* temp = current;
            current = current->next;
            free(temp);
        }
        free(g->adjMatrix[i]);
    }
    free(g->adjList); free(g->adjMatrix);
    free(g->totalEdgeWeights); free(g);
}


