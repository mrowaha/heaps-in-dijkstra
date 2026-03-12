#ifndef _MM_H_
#define _MM_H_

#define MM_MAXLINE 1000

struct mmdata {

	int N, M, NNZ;
	int *x;
	int *y;
	double *v;

	int symmetricity;
	int binary;
	int ndiagonal;
	int realnnz;
};

struct point 
{

	int x, y;
};

struct csrdata {
	int num_of_rows;
    int *row_ptr;
    int *col_idx;
    double *values;  // NULL if binary
};

int initialize_mm(char *file, struct mmdata *mm);
void convert_to_csr(struct mmdata *mm, struct csrdata *csr);
void print_csr_edges(struct csrdata *csr, int uptil);
void printmm(struct mmdata *mm, char *file);
void freemm(struct mmdata *mm);

#endif 
