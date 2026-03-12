#ifndef _BINOM_HEAP_H_
#define _BINOM_HEAP_H_

typedef struct binom_node {
    int vertex;
    double key;
    int degree; //  number of children
    struct binom_node *parent;
    struct binom_node *child; // the leftmost child
    struct binom_node *sibling; // the right sibling
    // for root nodes, sibling points to the next root in the root list
} BinomNode;

typedef struct binom_heap {
    BinomNode *head;    // head of root list head[H]
    int size;
} BinomHeap;

BinomHeap *binom_heap_create();
BinomNode *binom_heap_insert(BinomHeap *h, int vertex, double key);
BinomNode *binom_heap_minimum(BinomHeap *h);
BinomNode *binom_heap_extract_min(BinomHeap *h);
void binom_heap_decrease_key(BinomHeap *h, BinomNode *node, double new_key);
int binom_heap_is_empty(BinomHeap *h);
void binom_heap_free(BinomHeap *h);

#endif
