#include "graph.h"

// Partitioning functions
bool** allocateFinalPartitions(int k, int n);
void freeFinalPartitions(bool** partitions, int k);
void kWayPartition(Graph* g, int k, bool** finalPartitions, double earlyStopThreshold, double balanceMargin);
int calculateTotalCutSize(Graph* g, bool** partitions, int k);
