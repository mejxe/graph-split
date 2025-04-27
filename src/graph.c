#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
Graph* create_graph(int max_count, int* indexes, int* first_nodes_indexes, int* groups, int* first_group_indexes, int first_node_indexes_count) {
    Graph* graf = malloc(sizeof(*graf));
    graf->max_width = max_count;
    GraphNode **matrix = malloc(sizeof(GraphNode*)*(first_node_indexes_count));
    int k = 0;
    for (int i = 0; i < first_node_indexes_count; i++) {
        GraphNode *row = (GraphNode*) malloc(sizeof(GraphNode)*max_count);
        for (int j = 0; j < max_count; j++) {
            GraphNode node;
            node.node = -1;
            if (j == indexes[k]) {
                node.node = k;
                k++;
            }
            row[j] = node;

        }
        matrix[i] = row;
    }
    graf->graph_array = matrix;
            
    return graf;
}
void display_graph(Graph* graph, int size_x, int size_y) {
    for (int i = 0; i<size_y; i++) {
        for (int j = 0; j<size_x; j++) {
        printf("%d ",graph->graph_array[i][j].node);
        }
        printf("\n");
    }
}

