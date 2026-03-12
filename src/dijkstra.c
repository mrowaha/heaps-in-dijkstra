#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "mm.h"
#include "dijkstra.h"
#include "bin_heap.h"
#include "binom_heap.h"
#include "fib_heap.h"


// alias to dbl max for readability
#define INF DBL_MAX

#define DIJKSTRA_INIT(csr, source) \
    N = (csr)->num_of_rows; \
    dist = new_distances_vector(N, source);

static double* new_distances_vector(int N, int source) {
    double *dist = (double *)malloc(N * sizeof(double));
    for (int i = 0; i < N; i++) dist[i] = INF;
    // minimum distance to source vertex to itself is 0
    dist[source] = 0.0;
    return dist;
}

static double *dijkstra_bin(struct csrdata *csr, int source) {
    int N; double* dist;
    DIJKSTRA_INIT(csr, source);

    // the binary min-heap will store (vertex, distance) pairs, keyed by distance
    BinHeap *h = bin_heap_create(N);
    bin_heap_insert(h, source, 0.0);

    while (!bin_heap_is_empty(h)) {
        BinHeapNode u = bin_heap_extract_min(h);
        int v = u.vertex;

        // If the extracted distance is greater than the current known distance, it's a stale entry and we skip it
        if (u.key > dist[v]) continue;

        // this loops through all the outgoing edges of v
        // please see the README.md for details on the CSR format
        for (int j = csr->row_ptr[v]; j < csr->row_ptr[v + 1]; j++) {
            int w = csr->col_idx[j]; // destination vertex of the edge
            double weight = csr->values[j];

            // relaxation step: if we found a shorter path to w through v, update dist[w] and the heap
            if (dist[v] + weight < dist[w]) {
                dist[w] = dist[v] + weight;
                // update priority of w in the heap, or insert if it's not there
                if (h->pos[w] == -1)
                    bin_heap_insert(h, w, dist[w]);
                else
                    bin_heap_decrease_key(h, w, dist[w]);
            }
        }
    }

    bin_heap_free(h);
    return dist;
}

static double *dijkstra_binom(struct csrdata *csr, int source) {
    int N; double* dist;
    DIJKSTRA_INIT(csr, source);

    // book-keeping indexed by vertex
    BinomNode **node_ref = (BinomNode **)calloc(N, sizeof(BinomNode *));
    BinomHeap *h = binom_heap_create();

    // insert source
    node_ref[source] = binom_heap_insert(h, source, 0.0);

    while (!binom_heap_is_empty(h)) {

        BinomNode *u_node = binom_heap_extract_min(h);
        int v = u_node->vertex;
        node_ref[v] = NULL;   // no longer in heap
        free(u_node);

        for (int j = csr->row_ptr[v]; j < csr->row_ptr[v + 1]; j++) {
            int w = csr->col_idx[j]; double weight =  csr->values[j];

            if (dist[v] + weight < dist[w]) {
                dist[w] = dist[v] + weight;
                if (node_ref[w] == NULL) {
                    // not yet in heap
                    node_ref[w] = binom_heap_insert(h, w, dist[w]);
                } else {
                    // already in heap so decrease key
                    binom_heap_decrease_key(h, node_ref[w], dist[w]);
                }
            }
        }
    }

    free(node_ref);
    binom_heap_free(h);
    return dist;
}

static double *dijkstra_fib(struct csrdata *csr, int source) {
    int N; double* dist;
    DIJKSTRA_INIT(csr, source);
 
    FibNode **node_ref = (FibNode **)calloc(N, sizeof(FibNode *));
    FibHeap *h = fib_heap_create();
    node_ref[source] = fib_heap_insert(h, source, 0.0);

    while (!fib_heap_is_empty(h)) {
        FibNode *u_node = fib_heap_extract_min(h);
        int v = u_node->vertex;
        node_ref[v] = NULL;
        free(u_node);

        for (int j = csr->row_ptr[v]; j < csr->row_ptr[v + 1]; j++) {
            int w = csr->col_idx[j];
            double weight = csr->values ? csr->values[j] : 1.0;
            if (dist[v] + weight < dist[w]) {
                dist[w] = dist[v] + weight;
                if (!node_ref[w])
                    node_ref[w] = fib_heap_insert(h, w, dist[w]);
                else
                    fib_heap_decrease_key(h, node_ref[w], dist[w]);
            }
        }
    }

    free(node_ref);
    fib_heap_free(h);
    return dist;
}

double *dijkstra(struct csrdata *csr, int source, int heap_type) {
  if (csr->values == NULL) {
    fprintf(stderr, "Unweighted-directed graphs not supported");
    return NULL;
  }
    switch (heap_type) {
        case HEAP_BINARY: return dijkstra_bin(csr, source);
        case HEAP_BINOMIAL: return dijkstra_binom(csr, source);
        case HEAP_FIBONACCI: return dijkstra_fib(csr, source);
        default:
            fprintf(stderr, "Unknown heap type %d. Must be one of [0=binary], [1=binomal], and [2=fibonacci]\n", heap_type);
            return NULL;
    }
}
