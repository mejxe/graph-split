#include "graph.h"
#include "io.h"
#include "algorytm.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {


char** inputFile = (char**)malloc(sizeof(char*)*64);
char** outputFile = (char**)malloc(sizeof(char*)*64);
int* parts = malloc(sizeof(int));
int* margin = malloc(sizeof(int));
char** format = (char**)malloc(sizeof(char*)*64);

  parseArgs(argc,argv,inputFile, outputFile,parts,margin,format);



  int max_count = 0;

  char *indexes_buffer = malloc(LINE_BUFFER_SIZE);
  char *first_nodes_indexes_buffer = malloc(LINE_BUFFER_SIZE);
  char *groups_buffer = malloc(LINE_BUFFER_SIZE);
  char *first_group_indexes_buffer = malloc(LINE_BUFFER_SIZE);
  readFile(*inputFile, &max_count, indexes_buffer,
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

  bufferToIntArray(indexes_buffer, indexes, LINE_BUFFER_SIZE,
                      &indexes_count);
  bufferToIntArray(first_nodes_indexes_buffer, first_nodes_indexes,
                      LINE_BUFFER_SIZE, &first_nodes_indexes_count);
  bufferToIntArray(groups_buffer, groups, LINE_BUFFER_SIZE, &groups_count);
  bufferToIntArray(first_group_indexes_buffer, first_group_indexes,
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
  int k = *parts;
  bool **partition = allocateFinalPartitions(k, g->vertices);

  // Run k-way partitioning
  kWayPartition(g, k, partition, 0.5,*margin); // Using 0.5% as early stop threshold

  // Calculate final cut size
  int cutSize = calculateTotalCutSize(g, partition, k);
  printf("Total cut size for %d-way partitioning: %d\n", k, cutSize);


  if(strcmp(*format,"txt")== 0){
     saveKWayPartitionTGF(*outputFile,g,partition,k,*inputFile);
  } 
  else if(strcmp(*format,"bin")== 0){
    saveKWayPartitionBinary(*outputFile,g,partition,k,*inputFile);
  }   

  
  
  
  freeGraph(g);
  free(indexes_buffer);
  free(first_nodes_indexes_buffer);
  free(groups_buffer);
  free(first_group_indexes_buffer);
  free(indexes);
  free(first_nodes_indexes);
  free(groups);
  free(first_group_indexes);


  free(inputFile);
  free(outputFile);
  free(parts);
  free(margin);
  free(format);


  printf("\n");
}

