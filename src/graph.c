#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
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

// Helper functions for partitioning
static int calculateCutSize(Graph* g, bool* partition) {
    int cutSize = 0;
    for (int i = 0; i < g->vertices; i++) {
        if (partition[i]) {
            Edge* edge = g->adjList[i];
            while (edge) {
                if (!partition[edge->dest]) cutSize++;
                edge = edge->next;
            }
        }
    }
    return cutSize;

}

static void fastKL(Graph* g, bool* partition, double earlyStopThreshold) {
    typedef struct { int a, b, gain; } VertexPair;
    int n = g->vertices;
    
    // Allocate memory
    int *externalEdges = (int*)malloc(n * sizeof(int));
    int *internalEdges = (int*)malloc(n * sizeof(int));
    bool *locked = (bool*)malloc(n * sizeof(bool));
    VertexPair *swaps = (VertexPair*)malloc((n/2) * sizeof(VertexPair));
    bool *bestPartition = (bool*)malloc(n * sizeof(bool));
    int *improvementHistory = (int*)calloc(3, sizeof(int));
    int *partA = (int*)malloc(n * sizeof(int));
    int *partB = (int*)malloc(n * sizeof(int));
    
    if (!externalEdges || !internalEdges || !locked || !swaps || !bestPartition || 
        !improvementHistory || !partA || !partB) {
        fprintf(stderr, "Memory allocation failed\n"); exit(1);
    }

    int initialCutSize = calculateCutSize(g, partition);
    int currentCutSize = initialCutSize;
    int bestCutSize = initialCutSize;
    printf("Initial cut size: %d edges\n", initialCutSize);
    memcpy(bestPartition, partition, n * sizeof(bool));
    int passCount = 0, historyIndex = 0, passesWithoutImprovement = 0;
    bool improved;

    do {
        improved = false;
        passCount++;
        memset(locked, 0, n * sizeof(bool));

        // Build partition lists
        int countA = 0, countB = 0;
        for (int i = 0; i < n; i++) {
            if (partition[i]) partA[countA++] = i;
            else partB[countB++] = i;
        }

        // Calculate edge counts
        for (int i = 0; i < n; i++) {
            internalEdges[i] = externalEdges[i] = 0;
            bool inA = partition[i];
            Edge* edge = g->adjList[i];
            while (edge) {
                if (partition[edge->dest] == inA) internalEdges[i]++;
                else externalEdges[i]++;
                edge = edge->next;
            }
        }

        // Prepare for swaps
        int swapCount = 0, bestGain = 0, bestSwapCount = 0, cumulativeGain = 0;
        int maxCandidates = n > 30000 ? 25 : (n > 10000 ? 50 : 100);
        int maxSwaps = n > 10000 ? n/10 : n/4;
        
        // Create active vertex arrays
        int *activeA = (int*)malloc(countA * sizeof(int));
        int *activeB = (int*)malloc(countB * sizeof(int));
        int *gainA = (int*)malloc(countA * sizeof(int));
        int *gainB = (int*)malloc(countB * sizeof(int));
        
        if (!activeA || !activeB || !gainA || !gainB) {
            fprintf(stderr, "Memory allocation failed\n"); exit(1);
        }
        
        // Initialize active sets
        memcpy(activeA, partA, countA * sizeof(int));
        memcpy(activeB, partB, countB * sizeof(int));
        int activeSizeA = countA, activeSizeB = countB;
        
        for (int i = 0; i < countA; i++) gainA[i] = externalEdges[partA[i]] - internalEdges[partA[i]];
        for (int i = 0; i < countB; i++) gainB[i] = externalEdges[partB[i]] - internalEdges[partB[i]];

        // Main KL loop
        for (int swapsTried = 0; swapsTried < maxSwaps && activeSizeA > 0 && activeSizeB > 0; swapsTried++) {
            // Sort candidates periodically
            if (swapsTried % 10 == 1 || n < 10000) {
                // Sort partA candidates by gain

                for (int i = 0; i < maxCandidates && i < activeSizeA; i++) {
                    int maxIdx = i;
                    for (int j = i+1; j < activeSizeA; j++)
                        if (gainA[j] > gainA[maxIdx]) maxIdx = j;
                    if (maxIdx != i) {
                        int tv = activeA[i], tg = gainA[i];
                        activeA[i] = activeA[maxIdx]; gainA[i] = gainA[maxIdx];
                        activeA[maxIdx] = tv; gainA[maxIdx] = tg;
                    }
                }










                
                // Sort partB candidates by gain
                for (int i = 0; i < maxCandidates && i < activeSizeB; i++) {
                    int maxIdx = i;
                    for (int j = i+1; j < activeSizeB; j++)
                        if (gainB[j] > gainB[maxIdx]) maxIdx = j;
                    if (maxIdx != i) {
                        int tv = activeB[i], tg = gainB[i];
                        activeB[i] = activeB[maxIdx]; gainB[i] = gainB[maxIdx];
                        activeB[maxIdx] = tv; gainB[maxIdx] = tg;
                    }
                }
            }

            // Find best pair to swap
            int maxGain = INT_MIN, bestA = -1, bestB = -1, bestAIdx = -1, bestBIdx = -1;
            int checkA = maxCandidates < activeSizeA ? maxCandidates : activeSizeA;
            int checkB = maxCandidates < activeSizeB ? maxCandidates : activeSizeB;
            
            for (int ai = 0; ai < checkA; ai++) {
                int a = activeA[ai];
                if (locked[a]) continue;
                int Da = gainA[ai];
                if (Da <= -3 && maxGain > 0) continue;

                for (int bi = 0; bi < checkB; bi++) {
                    int b = activeB[bi];
                    if (locked[b]) continue;
                    int Db = gainB[bi];
                    if (Da + Db <= -3 && maxGain > 0) continue;

                    int cost = g->adjMatrix[a][b] ? 1 : 0;
                    int gain = Da + Db - 2 * cost;
                    if (gain > maxGain) {
                        maxGain = gain;
                        bestA = a; bestB = b;
                        bestAIdx = ai; bestBIdx = bi;
                    }
                }
            }

            // Early termination conditions
            if (bestA == -1 || bestB == -1) break;
            if (maxGain <= 0 && swapsTried > 10) break;

            // Lock and record swap
            locked[bestA] = locked[bestB] = true;
            swaps[swapCount].a = bestA;
            swaps[swapCount].b = bestB;
            swaps[swapCount].gain = maxGain;
            swapCount++;

            // Update active sets
            if (bestAIdx < activeSizeA - 1) {
                activeA[bestAIdx] = activeA[activeSizeA-1];
                gainA[bestAIdx] = gainA[activeSizeA-1];
            }
            activeSizeA--;
            
            if (bestBIdx < activeSizeB - 1) {
                activeB[bestBIdx] = activeB[activeSizeB-1];
                gainB[bestBIdx] = gainB[activeSizeB-1];
            }
            activeSizeB--;

            // Update neighbor gains for bestA
            Edge* edge = g->adjList[bestA];
            while (edge) {
                int v = edge->dest;
                if (!locked[v]) {
                    bool vInA = partition[v];
                    if (vInA) {
                        for (int i = 0; i < activeSizeA; i++) {
                            if (activeA[i] == v) {
                                gainA[i] += partition[bestA] ? 2 : -2;
                                break;
                            }
                        }
                    } else {
                        for (int i = 0; i < activeSizeB; i++) {
                            if (activeB[i] == v) {
                                gainB[i] += partition[bestA] ? -2 : 2;
                                break;
                            }
                        }
                    }
                }
                edge = edge->next;
            }

            // Update neighbor gains for bestB
            edge = g->adjList[bestB];
            while (edge) {
                int v = edge->dest;
                if (!locked[v]) {
                    bool vInA = partition[v];
                    if (vInA) {
                        for (int i = 0; i < activeSizeA; i++) {
                            if (activeA[i] == v) {
                                gainA[i] += partition[bestB] ? 2 : -2;
                                break;
                            }
                        }
                    } else {
                        for (int i = 0; i < activeSizeB; i++) {
                            if (activeB[i] == v) {
                                gainB[i] += partition[bestB] ? -2 : 2;
                                break;
                            }
                        }
                    }
                }
                edge = edge->next;
            }

            // Track gains
            cumulativeGain += maxGain;
            if (cumulativeGain > bestGain) {
                bestGain = cumulativeGain;
                bestSwapCount = swapCount;
            }

            // Early stop if gain declining
            if (cumulativeGain < bestGain - 10 && swapCount > 20) break;
        }

        free(activeA); free(activeB); free(gainA); free(gainB);

        // Apply best swaps if beneficial
        if (bestGain > 0 && bestSwapCount > 0) {
            improved = true;
            for (int i = 0; i < bestSwapCount; i++) {
                partition[swaps[i].a] = false;
                partition[swaps[i].b] = true;
            }

            int newCutSize = calculateCutSize(g, partition);
            int improvement = currentCutSize - newCutSize;
        printf("Pass %d: Improved by %d edges\n", passCount, improvement);
            
      

            // Track improvement history
            improvementHistory[historyIndex] = improvement;
            historyIndex = (historyIndex + 1) % 3;
            
            // Save best partition
            if (newCutSize < bestCutSize) {
                bestCutSize = newCutSize;
                memcpy(bestPartition, partition, n * sizeof(bool));
            }
            
            currentCutSize = newCutSize;
            double improvePct = (double)improvement / currentCutSize * 100.0;
            
            // Early termination checks
            if (improvePct < earlyStopThreshold) {
                passesWithoutImprovement++;
                if (passesWithoutImprovement >= 2) break;
            } else {
                passesWithoutImprovement = 0;
            }
            
            double avgImprovement = 0;
            int validHistory = passCount < 3 ? passCount : 3;
            for (int i = 0; i < validHistory; i++) avgImprovement += improvementHistory[i];
            avgImprovement /= validHistory;
            
            if ((passCount >= 3 && avgImprovement < initialCutSize * 0.0005) || 
                (improvement < 5 && passCount > 2)) {
                break;
            }
        } else {
            break;
        }
    } while (improved);

    // Use best partition if better
    if (bestCutSize < currentCutSize) {
        memcpy(partition, bestPartition, n * sizeof(bool));
    }

    free(externalEdges); free(internalEdges); free(locked); free(swaps);
    free(bestPartition); free(improvementHistory); free(partA); free(partB);
}

static void createInitialPartition(int* vertices, int size, int targetSize, double margin, bool* partition) {
    // Fisher-Yates shuffle
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = vertices[i]; vertices[i] = vertices[j]; vertices[j] = temp;
    }
    
    // Add variance within margin
    int variance = (int)(targetSize * margin / 100.0 * 0.5);
    int actualSize = targetSize;
    
    if (variance > 0) {
        int randomVar = (rand() % (2 * variance + 1)) - variance;
        actualSize = targetSize + randomVar;
        if (actualSize > size) actualSize = size;
        if (actualSize < 1) actualSize = 1;
    }
    
    // Assign vertices
    for (int i = 0; i < actualSize; i++) partition[vertices[i]] = true;
}

// Functions used by main.c
bool** allocateFinalPartitions(int k, int n) {
    bool** partitions = (bool**)malloc(k * sizeof(bool*));
    if (!partitions) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    
    for (int i = 0; i < k; i++) {
        partitions[i] = (bool*)calloc(n, sizeof(bool));
        if (!partitions[i]) {
            for (int j = 0; j < i; j++) free(partitions[j]);
            free(partitions); exit(1);
        }
    }
    return partitions;
}

void freeFinalPartitions(bool** partitions, int k) {
    for (int i = 0; i < k; i++) free(partitions[i]);
    free(partitions);
}

int calculateTotalCutSize(Graph* g, bool** partitions, int k) {
    int cutSize = 0, n = g->vertices;
    
    for (int i = 0; i < n; i++) {
        int partI = -1;
        for (int p = 0; p < k; p++) {
            if (partitions[p][i]) { partI = p; break; }
        }
        
        if (partI == -1) continue;
        
        Edge* edge = g->adjList[i];
        while (edge) {
            int j = edge->dest;
            if (i < j) {  // Count each edge once
                int partJ = -1;
                for (int p = 0; p < k; p++) {
                    if (partitions[p][j]) { partJ = p; break; }
                }
                
                if (partJ != -1 && partI != partJ) cutSize++;
            }
            edge = edge->next;
        }
    }
    return cutSize;
}

void kWayPartition(Graph* g, int k, bool** finalPartitions, double earlyStopThreshold, double balanceMargin) {
    int n = g->vertices;
    printf("Starting %d-way partitioning for %d vertices (margin: %.1f%%)\n", k, n, balanceMargin);
    
    srand(time(NULL));
    int idealSize = n / k;
    int minSize = (int)(idealSize * (100.0 - balanceMargin) / 100.0);
    int maxSize = (int)(idealSize * (100.0 + balanceMargin) / 100.0);
    
    if (minSize < 1) minSize = 1;
    if (maxSize > n) maxSize = n;
    
    int* vertexPartition = (int*)malloc(n * sizeof(int));
    if (!vertexPartition) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    memset(vertexPartition, 0, n * sizeof(int));
    
    // k-1 bisections needed for k partitions
    for (int bisect = 0; bisect < k-1; bisect++) {
        // Find largest partition to bisect
        int* partSizes = (int*)calloc(k, sizeof(int));
        for (int i = 0; i < n; i++) partSizes[vertexPartition[i]]++;
        
        int maxPartSize = -1, partToBisect = -1;
        for (int p = 0; p < k; p++) {
            if (partSizes[p] > maxPartSize) {
                maxPartSize = partSizes[p];
                partToBisect = p;
            }
        }
        
        int newPartID = bisect + 1;
        int remainingPartitions = k - bisect - 1;
        
        // Collect vertices in partition
        int* verticesInPart = (int*)malloc(maxPartSize * sizeof(int));
        if (!verticesInPart) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
        
        int idx = 0;
        for (int i = 0; i < n; i++)
            if (vertexPartition[i] == partToBisect) verticesInPart[idx++] = i;
        
        int newPartSize = (remainingPartitions == 1) ? 
                          (maxPartSize / 2) : idealSize;
        
        // Create temporary partition
        bool* tempPartition = (bool*)calloc(n, sizeof(bool));
        if (!tempPartition) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
        
        createInitialPartition(verticesInPart, maxPartSize, newPartSize, balanceMargin, tempPartition);
        
        // Create subgraph
        Graph* subgraph = initGraph(maxPartSize);
        int* origToSub = (int*)malloc(n * sizeof(int));
        int* subToOrig = (int*)malloc(maxPartSize * sizeof(int));
        
        // Map vertices
        for (int i = 0; i < maxPartSize; i++) {
            int origV = verticesInPart[i];
            origToSub[origV] = i;
            subToOrig[i] = origV;
        }
        
        // Add edges
        for (int i = 0; i < maxPartSize; i++) {
            Edge* edge = g->adjList[verticesInPart[i]];
            while (edge) {
                int origNeighbor = edge->dest;
                for (int j = 0; j < maxPartSize; j++) {
                    if (verticesInPart[j] == origNeighbor) {
                        addEdge(subgraph, i, j);
                        break;
                    }
                }
                edge = edge->next;
            }
        }
        
        // Create subgraph partition
        bool* subPartition = (bool*)calloc(maxPartSize, sizeof(bool));
        if (!subPartition) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
        
        for (int i = 0; i < maxPartSize; i++)
            subPartition[i] = tempPartition[subToOrig[i]];
        
        // Run KL on subgraph
        fastKL(subgraph, subPartition, earlyStopThreshold);
        
        // Map result back
        for (int i = 0; i < maxPartSize; i++)
            tempPartition[subToOrig[i]] = subPartition[i];
        
        // Apply partition
        int countAssigned = 0;
        for (int i = 0; i < n; i++) {
            if (vertexPartition[i] == partToBisect && tempPartition[i]) {
                vertexPartition[i] = newPartID;
                countAssigned++;
            }
        }
        
        // Balance if needed
        if (countAssigned < minSize) {
            int needToMove = minSize - countAssigned;
            for (int i = 0; i < n && needToMove > 0; i++) {
                if (vertexPartition[i] == partToBisect) {
                    vertexPartition[i] = newPartID;
                    needToMove--; countAssigned++;
                }
            }
        } else if (countAssigned > maxSize) {
            int needToMove = countAssigned - maxSize;
            for (int i = 0; i < n && needToMove > 0; i++) {
                if (vertexPartition[i] == newPartID) {
                    vertexPartition[i] = partToBisect;
                    needToMove--; countAssigned--;
                }
            }
        }
        
        printf("Created partition %d with %d vertices\n", newPartID, countAssigned);
        
        // Check if enough vertices remain for future partitions
        int remainingSize = partSizes[partToBisect] - countAssigned;
        int minRemainingNeeded = minSize * (remainingPartitions - 1);
        
        if (remainingSize < minRemainingNeeded && countAssigned > (minRemainingNeeded - remainingSize)) {
            int needToMove = minRemainingNeeded - remainingSize;
            int moved = 0;
            for (int i = 0; i < n && moved < needToMove; i++) {
                if (vertexPartition[i] == newPartID) {
                    vertexPartition[i] = partToBisect;
                    moved++;
                }
            }
            countAssigned -= moved;
        }
        
        free(verticesInPart); free(tempPartition); free(subPartition);
        free(origToSub); free(subToOrig); free(partSizes);
        freeGraph(subgraph);
    }
    
    // Map partitioning to output format
    for (int p = 0; p < k; p++)
        for (int v = 0; v < n; v++)
            finalPartitions[p][v] = (vertexPartition[v] == p);
    
    // Calculate final metrics
    int* finalSizes = (int*)calloc(k, sizeof(int));
    int minFinalSize = n, maxFinalSize = 0;
    
    for (int p = 0; p < k; p++) {
        for (int v = 0; v < n; v++)
            if (finalPartitions[p][v]) finalSizes[p]++;
        
        printf("Partition %d: %d vertices\n", p, finalSizes[p]);
        if (finalSizes[p] < minFinalSize) minFinalSize = finalSizes[p];
        if (finalSizes[p] > maxFinalSize) maxFinalSize = finalSizes[p];
    }
    
    double imbalance = 100.0 * (maxFinalSize - minFinalSize) / (double)(n/k);
    printf("Balance: Min=%d, Max=%d, Ideal=%d, Imbalance=%.2f%%\n", 
           minFinalSize, maxFinalSize, n/k, imbalance);
    
    int totalCut = calculateTotalCutSize(g, finalPartitions, k);
    printf("Total cut size: %d\n", totalCut);
    
    free(vertexPartition); free(finalSizes);
}
