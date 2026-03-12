#include <stdio.h>
#include <stdlib.h>

#include "fib_heap.h"

void run_fib_heap_tests() {
    printf("=== Fibonacci Heap Tests ===\n");

    FibHeap *h = fib_heap_create();

    FibNode *nodes[5];
    nodes[0] = fib_heap_insert(h, 0, 5.0);
    nodes[1] = fib_heap_insert(h, 1, 3.0);
    nodes[2] = fib_heap_insert(h, 2, 8.0);
    nodes[3] = fib_heap_insert(h, 3, 1.0);
    nodes[4] = fib_heap_insert(h, 4, 4.0);

    FibNode *min = fib_heap_minimum(h);
    printf("minimum: vertex=%d key=%.1f (expected vertex=3 key=1.0)\n", min->vertex, min->key);

    FibNode *n = fib_heap_extract_min(h);
    printf("extract_min: vertex=%d key=%.1f (expected vertex=3 key=1.0)\n", n->vertex, n->key);
    free(n);

    // Trigger consolidation by extracting, then test decrease_key
    fib_heap_decrease_key(h, nodes[2], 0.5);
    n = fib_heap_extract_min(h);
    printf("after decrease_key(vertex=2, 0.5), extract_min: vertex=%d key=%.1f (expected vertex=2 key=0.5)\n", n->vertex, n->key);
    free(n);

    while (!fib_heap_is_empty(h)) {
        n = fib_heap_extract_min(h);
        printf("extract_min: vertex=%d key=%.1f\n", n->vertex, n->key);
        free(n);
    }

    fib_heap_free(h);
    printf("=== Fibonacci Heap Tests Done ===\n\n");
}
