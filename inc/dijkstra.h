#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "mm.h"

#define HEAP_BINARY   0
#define HEAP_BINOMIAL 1
#define HEAP_FIBONACCI 2

double *dijkstra(struct csrdata *csr, int source, int heap_type);

#endif
