#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

#include "fib_heap.h"

FibHeap *fib_heap_create() {
    FibHeap *h = (FibHeap *)malloc(sizeof(FibHeap));
    h->min = NULL;
    h->size = 0;
    return h;
}

// insert node into a circular doubly-linked root list
static void insert_into_list(FibNode **list, FibNode *n)  {
    if (!*list) {
      // if the list is empty, initialize it with n
        *list = n;
        n->left = n;
        n->right = n;
    } else {
        n->right = *list;
        n->left = (*list)->left;
        (*list)->left->right = n;
        (*list)->left = n;
    }
}

FibNode *fib_heap_insert(FibHeap *h, int vertex, double key) {
    FibNode *n = (FibNode *)malloc(sizeof(FibNode));
    n->vertex = vertex;
    n->key = key;
    n->degree = 0;
    n->marked = false;
    n->parent = NULL;
    n->child = NULL;
    n->left = n;
    n->right = n;

    insert_into_list(&h->min, n);
    if (h->min == NULL || n->key < h->min->key)
        h->min = n;
    h->size++;
    return n;
}

FibNode *fib_heap_minimum(FibHeap *h) {
    return h->min;
}

// remove node from its circular doubly-linked list
static void remove_from_list(FibNode *n) {
    n->left->right = n->right;
    n->right->left = n->left;
    n->left = n;
    n->right = n;
}

static void fib_link(FibHeap *h, FibNode *y, FibNode *x) {
    // remove y from root list and make it a child of x
    remove_from_list(y);
    y->parent = x;
    insert_into_list(&x->child, y);
    x->degree++;
    y->marked = false;
}

FibNode *fib_heap_extract_min(FibHeap *h) {
    FibNode *z = h->min;
    // defensive
    if (!z) return NULL;

    // add all children of z to root list
    if (z->child) {
      // this can be any child, not the leftmost necessarily
        FibNode *child = z->child, *start = child;
        do {
            FibNode *next = child->right;
            insert_into_list(&h->min, child);
            child->parent = NULL;
            child = next;
        } while (child != start);
        z->child = NULL;
    }

    // remove z from root list
    if (z->right == z) {
        h->min = NULL;
    } else {
        h->min = z->right;
        remove_from_list(z);

        // consolidate (reduce the number of trees)
        // goal: no two trees in the root list should have the same degree
        int max_degree = (int)(log2(h->size) + 2) + 1;
        FibNode **A = (FibNode **)calloc(max_degree, sizeof(FibNode *));

        // collect all roots
        int num_roots = 0;
        FibNode *curr = h->min;
        FibNode *start = h->min;
        do { num_roots++; curr = curr->right; } while (curr != start);

        // have a stable snapshot of root list since we will be modifying it when linking
        FibNode **roots = (FibNode **)malloc(num_roots * sizeof(FibNode *));
        curr = h->min;
        for (int i = 0; i < num_roots; i++) {
            roots[i] = curr;
            curr = curr->right;
        }

        for (int i = 0; i < num_roots; i++) {
            FibNode *w = roots[i];
            int d = w->degree;
            while (d < max_degree && A[d]) {
                FibNode *y = A[d];
                // ensure the smaller key node is the parent before linking
                if (w->key > y->key) { FibNode *tmp = w; w = y; y = tmp; }
                fib_link(h, y, w);
                A[d] = NULL;
                d++;
            }
            if (d < max_degree) A[d] = w;
        }

        // rebuild root list and find new min
        h->min = NULL;
        for (int i = 0; i < max_degree; i++) {
            if (A[i]) {
                A[i]->left = A[i];
                A[i]->right = A[i];
                insert_into_list(&h->min, A[i]);
                if (!h->min || A[i]->key < h->min->key)
                    h->min = A[i];
            }
        }
        free(A);
        free(roots);
    }

    h->size--;
    return z;
}

static void cut(FibHeap *h, FibNode *x, FibNode *y) {
    // Remove x from y's child list
    if (x->right == x) y->child = NULL;
    else {
        if (y->child == x) y->child = x->right;
        remove_from_list(x);
    }
    y->degree--;
    insert_into_list(&h->min, x);
    x->parent = NULL;
    x->marked = false;
}

static void cascading_cut(FibHeap *h, FibNode *y) {
    FibNode *z = y->parent;
    if (z) {
        if (!y->marked) y->marked = true;
        else {
            cut(h, y, z);
            cascading_cut(h, z);
        }
    }
}

void fib_heap_decrease_key(FibHeap *h, FibNode *x, double new_key) {
    // defensive
    if (new_key > x->key) {
        fprintf(stderr, "New key is greater than current key\n");
        return;
    }
  
    x->key = new_key;
    FibNode *y = x->parent;
    if (y && x->key < y->key) {
        cut(h, x, y);
        cascading_cut(h, y);
    }
    if (x->key < h->min->key)
        h->min = x;
}

int fib_heap_is_empty(FibHeap *h) {
    return h->size == 0;
}

static void free_node(FibNode *n) {
    if (!n) return;
    FibNode *curr = n;
    do {
        FibNode *next = curr->right;
        free_node(curr->child);
        free(curr);
        curr = next;
    } while (curr != n);
}

void fib_heap_free(FibHeap *h) {
    // recursively frees the heap
    free_node(h->min);
    free(h);
}
