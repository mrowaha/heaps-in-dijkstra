# Dijkstra Implementations with Different Heaps

## Dataset usage
The matrices used in this project follow the `.mtx` __Matrix Market__ format. The first line of the datasets [cage8](cage8/mtx), [sparse_graph300](sparse_graph300.mtx), and [sparse_graph1000](sparse_graph1000.mtx) always starts with `%MatrixMarket matrix coordinate real general` where `coordinate` tells that the format is sparse matrix (only non-zero entries listed), `real` describes the field tyle and `general` tells that there is no symmetry. After comments, a line describes the matrix size `rows columns nonzeros`. Following this line, there are data entries where, in the `coordinate` format, the following is applied `row column value`.
> [!Note]
> This format starts the data entries (vertex index) with 1

The datasets are loaded, and parsed into a `csr` (compressed sparse row) data structures. Instead of storing a full NxN matrix, csr only stores the non-zero edges. `csr` uses three arrays:
1. row_ptr
2. col_idx
3. values

where `col_idx` stores the destination vertices for all edges and `values` stores the edge weights. `row_ptr` is a bit complex:
```
For a given vertex i:
row_ptr[i] = index where destionation vertices (edge weights) of the edges of vertex i start in col_idx (values)
row_ptr[i+1] - 1 = where they end
```
