#include "graph.h"
#include "io.h"
#include <stdio.h>
void add_edge_int(Graph* g, int a, int b) {
    char str1[20];
    char str2[20];
    sprintf(str1, "%d", a);
    sprintf(str2, "%d", b);
    add_edge(g, str1, str2);
}

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

  Graph *g = create_graph();
  for (int i = 0; i + 1 < first_nodes_indexes_count; i++) {
    int start = first_group_indexes[i];
    int j = start+1;
    int stop = first_group_indexes[i + 1];
    while (j < stop) {
      add_edge_int(g, groups[start], groups[j]);
      printf("%d -> %d ||", groups[start], groups[j]);
      j++;
    }
    printf("\n\n");
  }
      fm_algorithm(g);
    
    printf("Final partition:\n");
    for (int i = 0; i < g->vertex_count; i++) {
        printf("%s: %s\n", g->vertices[i]->name, g->partition[i] ? "Right" : "Left");
    }
    
    free_graph(g);
    return 0;

 // printf("first group index count: %d\n", first_group_indexes_count);
 // printf("index count: %d\n", indexes_count);
 // printf("first nodes index count: %d\n", first_nodes_indexes_count);
 // printf(" group  count: %d\n", groups_count);
 // printf(" max  count: %d\n", max_count);

  printf("\n");
}
