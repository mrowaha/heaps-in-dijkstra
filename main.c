// needed for CLOCK_MONOTONIC in non-posix environment
#define _POSIX_C_SOURCE 199309L 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mm.h"
#include "dijkstra.h"

static void usage(const char *prog) {
    fprintf(stderr, "Incorrect usage detected. Please use one of the following:\n");
    fprintf(stderr, "%s <graph.mtx> <binary|binomial|fibonacci> [source_vertex=1]\n", prog);
    fprintf(stderr, "%s --test\n", prog);
}

static inline void free_dataset(
  struct csrdata *csr,
  struct mmdata *mm
) {
  free(csr->row_ptr);
  free(csr->col_idx);
  if (csr->values) free(csr->values);
  freemm(mm);
}

static void write_dist_to_file(
  const double* dist,
  int n,
  const char* heap_name,
  // with the file extension .mtx
  const char* graph_file
) {
    const char *base = strrchr(graph_file, '/');
    base = base ? base + 1 : graph_file;

    char mtx_name[256];
    strncpy(mtx_name, base, sizeof(mtx_name) - 1);
    mtx_name[sizeof(mtx_name) - 1] = '\0';
    char *dot = strrchr(mtx_name, '.');
    if (dot) *dot = '\0';

    // Build output filename: <heapname>_<mtxname>_distances.txt
    char out_name[512];
    snprintf(out_name, sizeof(out_name), "out/%s_%s_distances.txt", heap_name, mtx_name);

    FILE *f = fopen(out_name, "w");
    if (!f) {
        fprintf(stderr, "Failed to open output file: %s\n", out_name);
        return;
    }

    for (int i = 0; i < n; i++) {
        if (dist[i] == __DBL_MAX__)
            fprintf(f, "inf\n");
        else
            fprintf(f, "%.6f\n", dist[i]);
    }

    fclose(f);
    printf("Distances written to %s\n", out_name);
}

int main(int argc, char *argv[]) {
  
    // if not testing, expect the path to the graph file, the name of the heap
    // the number of the source vertex is optional
    if (argc < 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int heap_type;
    const char* heap_name = argv[2];
    if (strcmp(heap_name, "binomial") == 0) heap_type = HEAP_BINOMIAL;
    else if (strcmp(heap_name, "fibonacci") == 0) heap_type = HEAP_FIBONACCI;
    else if (strcmp(heap_name, "binary") == 0) heap_type = HEAP_BINARY;
    else {
        fprintf(stderr, "Unknown heap type '%s'. Use binary, binomial, or fibonacci.\n", argv[2]);
        return EXIT_FAILURE;
    }

    char *graph_file = argv[1];
    // the default vertex is 1, but we convert it to 0 because of the CSR conversion
    int source = (argc > 3) ? atoi(argv[3] - 1) : 0;

    struct mmdata mm;
    if (initialize_mm(graph_file, &mm) != 0) {
        fprintf(stderr, "Failed to read graph file: %s\n", graph_file);
        return EXIT_FAILURE;
    }

    if (mm.binary == 1) {
      fprintf(stderr, "Unweighted-directed graphs not supported");
      freemm(&mm);
      return EXIT_FAILURE;
    }

    struct csrdata csr;
    convert_to_csr(&mm, &csr);

    if (source < 0 || source >= csr.num_of_rows) {
        fprintf(stderr, "Source vertex %d out of range [0, %d)\n", source, csr.num_of_rows);
        free_dataset(&csr, &mm);
        return EXIT_FAILURE;
    }

    printf("Running Dijkstra from source=%d using %s heap on %s\n", source, heap_name, graph_file);
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    double *dist = dijkstra(&csr, source, heap_type);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;
    printf("Dijkstra elapsed time: %.6f seconds\n", elapsed);
    write_dist_to_file(dist, csr.num_of_rows, heap_name, graph_file);
    
    free(dist);
    free_dataset(&csr, &mm);
    return EXIT_SUCCESS;
}
