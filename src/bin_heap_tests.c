#include <stdio.h>
#include <stdlib.h>

#include "bin_heap.h"

void run_bin_heap_tests() {
    printf("=== Binary Heap Tests ===\n");

    BinHeap *h = bin_heap_create(10);

    bin_heap_insert(h, 0, 5.0);
    bin_heap_insert(h, 1, 3.0);
    bin_heap_insert(h, 2, 8.0);
    bin_heap_insert(h, 3, 1.0);
    bin_heap_insert(h, 4, 4.0);

    // Test extract_min order
    BinHeapNode n = bin_heap_extract_min(h);
    printf("extract_min: vertex=%d key=%.1f (expected vertex=3 key=1.0)\n", n.vertex, n.key);

    // Test decrease_key
    bin_heap_decrease_key(h, 2, 0.5);
    n = bin_heap_extract_min(h);
    printf("after decrease_key(2, 0.5), extract_min: vertex=%d key=%.1f (expected vertex=2 key=0.5)\n", n.vertex, n.key);

    while (!bin_heap_is_empty(h)) {
        n = bin_heap_extract_min(h);
        printf("extract_min: vertex=%d key=%.1f\n", n.vertex, n.key);
    }

    bin_heap_free(h);
    printf("=== Binary Heap Tests Done ===\n\n");
}
