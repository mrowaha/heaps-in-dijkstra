#ifndef _BIN_HEAP_H_
#define _BIN_HEAP_H_

typedef struct bin_heap_node {
    int vertex;
    double key;
} BinHeapNode;

typedef struct bin_heap {
    BinHeapNode *data;
    int *pos;       // pos[vertex] = index in heap array
    int size;
    int capacity;
} BinHeap;

BinHeap *bin_heap_create(int capacity);
void bin_heap_insert(BinHeap *h, int vertex, double key);
BinHeapNode bin_heap_extract_min(BinHeap *h);
void bin_heap_decrease_key(BinHeap *h, int vertex, double new_key);
int bin_heap_is_empty(BinHeap *h);
void bin_heap_free(BinHeap *h);

#endif
