#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "bin_heap.h"

BinHeap *bin_heap_create(int capacity) {
    BinHeap *h = (BinHeap *)malloc(sizeof(BinHeap));
    h->data = (BinHeapNode *)malloc(capacity * sizeof(BinHeapNode));
    h->pos = (int *)malloc(capacity * sizeof(int));
    for (int i = 0; i < capacity; i++) h->pos[i] = -1;
    h->size = 0;
    h->capacity = capacity;
    return h;
}

static void swap(BinHeap *h, int i, int j) {
    BinHeapNode tmp = h->data[i];
    h->data[i] = h->data[j];
    h->data[j] = tmp;
    h->pos[h->data[i].vertex] = i;
    h->pos[h->data[j].vertex] = j;
}

static void sift_up(BinHeap *h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].key > h->data[i].key) {
            swap(h, i, parent);
            i = parent;
        } else break;
    }
}

static void sift_down(BinHeap *h, int i) {
    int n = h->size;
    while (1) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < n && h->data[left].key < h->data[smallest].key)
            smallest = left;
        if (right < n && h->data[right].key < h->data[smallest].key)
            smallest = right;
        if (smallest == i) break;
        swap(h, i, smallest);
        i = smallest;
    }
}

void bin_heap_insert(BinHeap *h, int vertex, double key) {
    int i = h->size++;
    h->data[i].vertex = vertex;
    h->data[i].key = key;
    h->pos[vertex] = i;
    sift_up(h, i);
}

BinHeapNode bin_heap_extract_min(BinHeap *h) {
    BinHeapNode min = h->data[0];
    h->pos[min.vertex] = -1;
    h->size--;
    if (h->size > 0) {
        h->data[0] = h->data[h->size];
        h->pos[h->data[0].vertex] = 0;
        sift_down(h, 0);
    }
    return min;
}

void bin_heap_decrease_key(BinHeap *h, int vertex, double new_key) {
    int i = h->pos[vertex];
    h->data[i].key = new_key;
    sift_up(h, i);
}

int bin_heap_is_empty(BinHeap *h) {
    return h->size == 0;
}

void bin_heap_free(BinHeap *h) {
    free(h->data);
    free(h->pos);
    free(h);
}
