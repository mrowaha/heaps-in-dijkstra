#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "fib_heap.h"

FibHeap *fib_heap_create() {
    FibHeap *h = (FibHeap *)malloc(sizeof(FibHeap));
    h->min = NULL;
    h->size = 0;
    return h;
}

static FibNode *make_node(int vertex, double key) {
    FibNode *n = (FibNode *)malloc(sizeof(FibNode));
    n->vertex = vertex;
    n->key = key;
    n->degree = 0;
    n->marked = 0;
    n->parent = NULL;
    n->child = NULL;
    n->left = n;
    n->right = n;
    return n;
}

// Insert node into a circular doubly-linked root list
static void insert_into_list(FibNode **list, FibNode *n) {
    if (!*list) {
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

// Remove node from its circular doubly-linked list
static void remove_from_list(FibNode *n) {
    n->left->right = n->right;
    n->right->left = n->left;
    n->left = n;
    n->right = n;
}

FibNode *fib_heap_insert(FibHeap *h, int vertex, double key) {
    FibNode *n = make_node(vertex, key);
    insert_into_list(&h->min, n);
    if (h->min == NULL || n->key < h->min->key)
        h->min = n;
    h->size++;
    return n;
}

FibNode *fib_heap_minimum(FibHeap *h) {
    return h->min;
}

static void fib_link(FibHeap *h, FibNode *y, FibNode *x) {
    // Remove y from root list and make it a child of x
    remove_from_list(y);
    y->parent = x;
    insert_into_list(&x->child, y);
    x->degree++;
    y->marked = 0;
}

FibNode *fib_heap_extract_min(FibHeap *h) {
    FibNode *z = h->min;
    if (!z) return NULL;

    // Add all children of z to root list
    if (z->child) {
        FibNode *child = z->child;
        FibNode *start = child;
        do {
            FibNode *next = child->right;
            insert_into_list(&h->min, child);
            child->parent = NULL;
            child = next;
        } while (child != start);
        z->child = NULL;
    }

    // Remove z from root list
    if (z->right == z) {
        h->min = NULL;
    } else {
        h->min = z->right;
        remove_from_list(z);
        // Consolidate
        int max_degree = (int)(log2(h->size) + 2) + 1;
        FibNode **A = (FibNode **)calloc(max_degree, sizeof(FibNode *));

        // Collect all roots
        int num_roots = 0;
        FibNode *curr = h->min;
        FibNode *start = h->min;
        do { num_roots++; curr = curr->right; } while (curr != start);

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
                if (w->key > y->key) { FibNode *tmp = w; w = y; y = tmp; }
                fib_link(h, y, w);
                A[d] = NULL;
                d++;
            }
            if (d < max_degree) A[d] = w;
        }

        // Rebuild root list and find new min
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
    x->marked = 0;
}

static void cascading_cut(FibHeap *h, FibNode *y) {
    FibNode *z = y->parent;
    if (z) {
        if (!y->marked) y->marked = 1;
        else {
            cut(h, y, z);
            cascading_cut(h, z);
        }
    }
}

void fib_heap_decrease_key(FibHeap *h, FibNode *x, double new_key) {
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

static void free_node_recursive(FibNode *n) {
    if (!n) return;
    FibNode *curr = n;
    do {
        FibNode *next = curr->right;
        free_node_recursive(curr->child);
        free(curr);
        curr = next;
    } while (curr != n);
}

void fib_heap_free(FibHeap *h) {
    free_node_recursive(h->min);
    free(h);
}
