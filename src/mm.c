#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mm.h"

#define STRINGSIZE 1000

char * dummy;

struct mmdata *alloc_mm() {
	return (struct mmdata *)malloc(sizeof(struct mmdata));
}

int initialize_mm(char *file, struct mmdata *mm) {

	char filename[STRINGSIZE];
	char line[MM_MAXLINE];
	char *saveptr, *token;
	sprintf(filename, "%s", file);
	
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		perror("Failed to open file");
		return -1;  // Return an error code or handle it as needed
	}

	mm -> symmetricity = 0;
	
	dummy = fgets(line, MM_MAXLINE, f);
	if(line[0] == '%') {
	
		token = strtok_r(line, " \n", &saveptr);
		while(token != NULL) {
			if(!strcmp(token, "symmetric")) {
				mm -> symmetricity = 1;
				break;
			}
			token = strtok_r(NULL, " \n", &saveptr);
		}

		dummy = fgets(line, MM_MAXLINE, f);	
		while(line[0] == '%') {
			dummy = fgets(line, MM_MAXLINE, f);	
		}
	}
		
	sscanf(line, " %d %d %d", &mm->N, &mm->M, &mm->NNZ);
	
	mm -> ndiagonal = 0;
	mm -> x = (int *)malloc(mm->NNZ * sizeof(int));
	mm -> y = (int *)malloc(mm->NNZ * sizeof(int));
	mm -> v = NULL;	

	dummy = fgets(line, MM_MAXLINE, f);
	double v;
	if(2 == sscanf(line, " %d %d %lf", &mm -> x[0], &mm -> y[0], &v)) 
		mm -> binary = 1;
	else {
		mm->binary = 0;
		mm -> v = (double *)malloc(mm->NNZ * sizeof(double));	
		mm->v[0] = v;
	}
	mm -> x[0] --;
	mm -> y[0] --;
	if(mm->symmetricity && mm -> x[0] == mm -> y[0])
		mm -> ndiagonal ++;
	
	int i;
	for(i=1; i<mm->NNZ; i++) {

		dummy = fgets(line, MM_MAXLINE, f);
		if(! mm -> binary) 
			sscanf(line, " %d %d %lf", &mm -> x[i], &mm -> y[i], &mm -> v[i]);
		else	
			sscanf(line, " %d %d", &mm -> x[i], &mm -> y[i]);

		mm -> x[i] --;
		mm -> y[i] --;

		if(mm->symmetricity && mm -> x[i] == mm -> y[i])
			mm -> ndiagonal ++;
	}

	mm -> realnnz = (!mm -> symmetricity)? mm->NNZ: 2 * mm->NNZ - mm->ndiagonal;
	fclose(f);
	return 0;
}

void convert_to_csr(struct mmdata *mm, struct csrdata *csr) {
    int N = mm->N;
    int *row_counts = (int *)calloc(N + 1, sizeof(int));
    int total_nnz = mm->realnnz;

    // Allocate CSR arrays
    csr->row_ptr = (int *)malloc((N + 1) * sizeof(int));
    csr->col_idx = (int *)malloc(total_nnz * sizeof(int));
    csr->values = (mm->binary) ? NULL : (double *)malloc(total_nnz * sizeof(double));
	csr->num_of_rows = mm->N;
    // Step 1: Count non-zeros per row
    for (int i = 0; i < mm->NNZ; i++) {
        row_counts[mm->x[i] + 1]++;
        if (mm->symmetricity && mm->x[i] != mm->y[i])  // Symmetric part
            row_counts[mm->y[i] + 1]++;
    }

    // Step 2: Prefix sum for row_ptr
    csr->row_ptr[0] = 0;
    for (int i = 1; i <= N; i++)
        csr->row_ptr[i] = csr->row_ptr[i - 1] + row_counts[i];

    // Reset row_counts to track current insertion index
    for (int i = 0; i <= N; i++) row_counts[i] = csr->row_ptr[i];

    // Step 3: Fill col_idx and values
    for (int i = 0; i < mm->NNZ; i++) {
        int row = mm->x[i];
        int idx = row_counts[row]++;
        csr->col_idx[idx] = mm->y[i];
        if (!mm->binary) csr->values[idx] = mm->v[i];

        // Add symmetric entry if needed
        if (mm->symmetricity && row != mm->y[i]) {
            int sym_row = mm->y[i];
            int sym_idx = row_counts[sym_row]++;
            csr->col_idx[sym_idx] = row;
            if (!mm->binary) csr->values[sym_idx] = mm->v[i];
        }
    }

    free(row_counts);
}

void print_csr_edges(struct csrdata *csr, int uptil) {
for(int v=0; v<uptil; v++) {
    for(int j=csr->row_ptr[v]; j<csr->row_ptr[v+1]; j++) {
        printf("%d %d\n", v, csr->col_idx[j]);
    }
}}


void printmm(struct mmdata *mm, char *filename) {

	FILE *f = fopen(filename, "w");
	fprintf(f, "%c%cMatrixMarket matrix coordinate %s symmetric\n", '%', '%', mm->binary? "pattern": "real");
	fprintf(f, "%d %d %d\n", mm->N, mm->M, mm->NNZ);
	int i;
	for(i=0; i<mm->NNZ; i++)
		fprintf(f, "%d %d\n", mm->x[i]+1, mm->y[i]+1);
	fclose(f);
}

void freemm(struct mmdata *mm) {
	free(mm->x);
	free(mm->y);
	free(mm->v);
}
