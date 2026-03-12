#ifndef _FIB_HEAP_H_
#define _FIB_HEAP_H_

#include <stdbool.h>

typedef struct fib_node {
    int vertex;
    double key;
    int degree;
    bool marked; // indicates if this node has lost a child since it became a child of its current parent   
    struct fib_node *parent;
    struct fib_node *child; // pointer to any one of the children
    // pointers to maintan a circular doubly-linked list of siblings (or root list if parent is NULL)
    struct fib_node *left;
    struct fib_node *right;
} FibNode;

typedef struct fib_heap {
    FibNode *min; // min[H]
    int size; // n[H]
} FibHeap;

FibHeap *fib_heap_create();
FibNode *fib_heap_insert(FibHeap *h, int vertex, double key) __attribute__((nonnull(1)));
FibNode *fib_heap_minimum(FibHeap *h) __attribute__((nonnull));
FibNode *fib_heap_extract_min(FibHeap *h) __attribute__((nonnull));
void fib_heap_decrease_key(FibHeap *h, FibNode *node, double new_key) __attribute__((nonnull(1,2)));
int fib_heap_is_empty(FibHeap *h) __attribute__((nonnull));
void fib_heap_free(FibHeap *h) __attribute__((nonnull));

#endif
