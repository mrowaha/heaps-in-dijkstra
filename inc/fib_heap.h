#ifndef _FIB_HEAP_H_
#define _FIB_HEAP_H_

typedef struct fib_node {
    int vertex;
    double key;
    int degree;
    int marked;
    struct fib_node *parent;
    struct fib_node *child;
    struct fib_node *left;
    struct fib_node *right;
} FibNode;

typedef struct fib_heap {
    FibNode *min;
    int size;
} FibHeap;

FibHeap *fib_heap_create();
FibNode *fib_heap_insert(FibHeap *h, int vertex, double key);
FibNode *fib_heap_minimum(FibHeap *h);
FibNode *fib_heap_extract_min(FibHeap *h);
void fib_heap_decrease_key(FibHeap *h, FibNode *node, double new_key);
int fib_heap_is_empty(FibHeap *h);
void fib_heap_free(FibHeap *h);

#endif
