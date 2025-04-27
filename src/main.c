#include "graph.h"
#include "io.h"
#include "algorytm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int max_count = 0;

  char *indexes_buffer = malloc(LINE_BUFFER_SIZE);
  char *first_nodes_indexes_buffer = malloc(LINE_BUFFER_SIZE);
  char *groups_buffer = malloc(LINE_BUFFER_SIZE);
  char *first_group_indexes_buffer = malloc(LINE_BUFFER_SIZE);
  read_file("graf1.csrrg", &max_count, indexes_buffer,
            first_nodes_indexes_buffer, groups_buffer,
            first_group_indexes_buffer);

  int *indexes = malloc(sizeof(int) * LINE_BUFFER_SIZE);
  int *first_nodes_indexes = malloc(sizeof(int) * LINE_BUFFER_SIZE);
  int *groups = malloc(sizeof(int) * LINE_BUFFER_SIZE);
  int *first_group_indexes = malloc(sizeof(int) * LINE_BUFFER_SIZE);

  int indexes_count = 0;
  int first_nodes_indexes_count = 0;
  int groups_count = 0;
  int first_group_indexes_count = 0;

  buffer_to_int_array(indexes_buffer, indexes, LINE_BUFFER_SIZE,
                      &indexes_count);
  buffer_to_int_array(first_nodes_indexes_buffer, first_nodes_indexes,
                      LINE_BUFFER_SIZE, &first_nodes_indexes_count);
  buffer_to_int_array(groups_buffer, groups, LINE_BUFFER_SIZE, &groups_count);
  buffer_to_int_array(first_group_indexes_buffer, first_group_indexes,
                      LINE_BUFFER_SIZE, &first_group_indexes_count);
  int n = indexes_count;
  Graph *g = initGraph(n);
  for (int i = 0; i < first_group_indexes_count; i++) {
    int start = first_group_indexes[i];
    int j = start + 1;
    int stop;
    if (i + 1 >= first_group_indexes_count) {
      stop = groups_count;
    } else {
      stop = first_group_indexes[i + 1];
    }
    while (j < stop) {
      addEdge(g, groups[start], groups[j]);
      j++;
    }
  }
  // Create k partitions
  int k = 2;
  bool **partition = allocateFinalPartitions(k, g->vertices);

  // Run k-way partitioning
  kWayPartition(g, k, partition, 0.5,0.1); // Using 0.5% as early stop threshold

  // Calculate final cut size
  int cutSize = calculateTotalCutSize(g, partition, k);
  printf("Total cut size for %d-way partitioning: %d\n", k, cutSize);

  // Free memory when done

  // Run the optimized algorithm
  printf("Running Kernighan-Lin algorithm on sample graph...\n");

      // savePartitionBinary("bintest",g,partition);
      // savePartitionTGF("test.tgf",g,partition);

      freeGraph(g);
  free(indexes_buffer);
  free(first_nodes_indexes_buffer);
  free(groups_buffer);
  free(first_group_indexes_buffer);
  free(indexes);
  free(first_nodes_indexes);
  free(groups);
  free(first_group_indexes);

  // printf("first group index count: %d\n", first_group_indexes_count);
  // printf("index count: %d\n", indexes_count);
  // printf("first nodes index count: %d\n", first_nodes_indexes_count);
  // printf(" group  count: %d\n", groups_count);
  // printf(" max  count: %d\n", max_count);

  printf("\n");
}

