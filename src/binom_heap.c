#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "binom_heap.h"

BinomHeap *binom_heap_create() {
    BinomHeap *h = (BinomHeap *)malloc(sizeof(BinomHeap));
    h->head = NULL;
    h->size = 0;
    return h;
}

// Link y under x (x.key <= y.key)
static inline void binom_link(BinomNode *x, BinomNode *y) {
    y->parent = x;
    y->sibling = x->child;
    x->child = y; // because y becomes the new leftmost child
    x->degree++;
}

// Merge two root lists into one sorted by degree
static BinomNode *merge_root_lists(BinomNode *a, BinomNode *b) {
    if (!a) return b;
    if (!b) return a;
    // merging like a mergesort
    BinomNode *head = NULL, **tail = &head;
    while (a && b) {
        if (a->degree <= b->degree) {
          // for example, in the first iteration head will be a
          // and tail advances and now points to the sibling pointer of a, which is where the next node should be attached
          // so if in next iteration b is chosen, it will be attached to a->sibling, which is correct
            *tail = a; tail = &a->sibling; a = a->sibling;
        } else {
            *tail = b; tail = &b->sibling; b = b->sibling;
        }
    }
    *tail = a ? a : b;
    return head;
}

static BinomNode *binom_heap_union(BinomNode *h1, BinomNode *h2) {
    BinomNode *head = merge_root_lists(h1, h2);
    if (!head) return NULL;

    // through out the procedure, three pointers are maintained: prev, curr, next
    BinomNode *prev = NULL, *curr = head, *next = curr->sibling;
    while (next) {
        if (curr->degree != next->degree ||
            (next->sibling && next->sibling->degree == curr->degree)) {
            // simply march forward either when degree of curr is different from next,
            // or when degree[curr] = degree[next] = degree[next->sibling]
            // in the second case, in the next iteration degree[curr] = degree[next] and we will link
            prev = curr;
            curr = next;
            // updating next is handled at the end of the loop, so that we can check the degree of next->sibling in the next iteration
        } else if (curr->key <= next->key) {
            curr->sibling = next->sibling;
            binom_link(curr, next);
        } else {
            if (!prev) head = next;
            else prev->sibling = next;
            binom_link(next, curr);
            curr = next;
        }
        next = curr->sibling;
    }
    return head;
}

BinomNode *binom_heap_insert(BinomHeap *h, int vertex, double key) {
    BinomNode *n = (BinomNode *)malloc(sizeof(BinomNode));
    n->vertex = vertex;
    n->key = key;
    n->degree = 0;
    n->parent = NULL;
    n->child = NULL;
    n->sibling = NULL;;
    h->head = binom_heap_union(h->head, n);
    h->size++;
    return n;
}

BinomNode *binom_heap_minimum(BinomHeap *h) {
    // the minimum key is always in the root list
    BinomNode *min = NULL, *curr = h->head;
    while (curr) {
        if (!min || curr->key < min->key) min = curr;
        curr = curr->sibling;
    }
    return min;
}

BinomNode *binom_heap_extract_min(BinomHeap *h) {
    if (!h->head) return NULL;

    // find the root with the minimum key in the root list of H
    // and remove it from the root list
    BinomNode *min_prev = NULL, *min = h->head;
    BinomNode *prev = NULL, *curr = h->head;
    while (curr) {
        if (curr->key < min->key) {
            min_prev = prev;
            min = curr;
        }
        prev = curr;
        curr = curr->sibling;
    }

    // remove min from root list
    if (min_prev) min_prev->sibling = min->sibling;
    else h->head = min->sibling;

    // reverse min's children and merge
    BinomNode *child = min->child, *rev = NULL;
    while (child) {
        BinomNode *next = child->sibling;
        child->sibling = rev;
        // since root is removed, these children will be attached to the root list, so their parent should be set to NULL
        child->parent = NULL;
        rev = child;
        child = next;
    }
    h->head = binom_heap_union(h->head, rev);
    h->size--;
    return min;
}

inline void swap(BinomNode *a, BinomNode *b) {
  // swaps vertices and keys only
  // if there was anyother satellite information, we would need to swap them as well
    int tmp_v = a->vertex; a->vertex = b->vertex; b->vertex = tmp_v;
    double tmp_k = a->key; a->key = b->key; b->key = tmp_k;
}

void binom_heap_decrease_key(BinomHeap *h, BinomNode *node, double new_key) {
  // defensive
    if (new_key > node->key) {
        fprintf(stderr, "New key is greater than current key\n");
        return;
    }

    node->key = new_key;
    BinomNode *curr = node, *parent = curr->parent;
    while (parent && curr->key < parent->key) {
        swap(curr, parent);
        curr = parent;
        parent = curr->parent;
    }
}

int binom_heap_is_empty(BinomHeap *h) {
    return h->size == 0;
}

static void free_node(BinomNode *n) {
    if (!n) return;
    free_node(n->child);
    free_node(n->sibling);
    free(n);
}

void binom_heap_free(BinomHeap *h) {
    // recursively frees the heap
    free_node(h->head);
    free(h);
}
