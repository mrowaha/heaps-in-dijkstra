# Dijkstra Implementations with Different Heaps

## Execution steps

### Build

Compile all source files and link the binary using:

```bash
make
```

This produces the `dijkstrahw1` executable. To also clean up intermediate `.o` files after linking:

```bash
make NO_OBJS=1
```

To remove all build artifacts:

```bash
make clean
```

### Run

```
./dijkstrahw1 <graph.mtx> <binary|binomial|fibonacci>
```

- `<graph.mtx>` — path to a Matrix Market `.mtx` graph file (e.g. `mm/cage8.mtx`)
- `<binary|binomial|fibonacci>` — heap implementation to use for Dijkstra's algorithm

**Examples:**

```bash
./dijkstrahw1 mm/sparse_graph300.mtx binary
./dijkstrahw1 mm/sparse_graph1000.mtx binomial
./dijkstrahw1 mm/cage8.mtx fibonacci
```

Output distance files are written to the `out/` directory with the naming convention `<heap>_<graph>_distances.txt`. Each line in the output corresponds to the shortest distance from the vertex 1 to that vertex index, or `inf` if unreachable.

### Experimental Analysis on Generated Graphs

The archive `generated_graphs.tar.gz` contains 30 generated `.mtx` graphs covering three density classes and vertex counts from 500 to 5000 (in steps of 500):

| Density class | Naming pattern             | Vertex counts            |
| ------------- | -------------------------- | ------------------------ |
| Sparse        | `sparse_graph<N>.mtx`      | 500, 1000, 1500, …, 5000 |
| Medium-dense  | `mediumdense_graph<N>.mtx` | 500, 1000, 1500, …, 5000 |
| Dense         | `dense_graph<N>.mtx`       | 500, 1000, 1500, …, 5000 |

All files are extracted into `mm/generated/`.

**Step 1 — Generate graphs**

```bash
python3 scripts/generate_graphs.py
```

This creates `mm/generated/` with all `.mtx` files.

**Step 2 — Create the output directory if needed**

```bash
mkdir -p out
```

**Step 3 — Run Dijkstra over all graphs and all heap types**

Use the provided [scripts/benchmark.py](scripts/benchmark.py) script, which runs every `.mtx` file in `mm/generated/` against all three heap types and collects the timings automatically:

```bash
python3 scripts/benchmark.py
```

The script iterates over all graphs matching the pattern `{sparse|mediumdense|dense}_graph<N>.mtx`, invokes `./dijkstrahw1` for each `(graph, heap)` pair, parses the elapsed time from stdout, and writes all results to `times.json` in the following structure:

```json
[
  {
    "type": "binary",
    "results": {
      "sparse":      { "500": 0.000123, "1000": 0.000456, ... },
      "mediumdense": { "500": 0.000789, ... },
      "dense":       { "500": 0.001234, ... }
    }
  },
  { "type": "binomial", "results": { ... } },
  { "type": "fibonacci", "results": { ... } }
]
```

## Dataset usage

The matrices used in this project follow the `.mtx` **Matrix Market** format. The first line of the datasets [cage8](cage8/mtx), [sparse_graph300](sparse_graph300.mtx), and [sparse_graph1000](sparse_graph1000.mtx) always starts with `%MatrixMarket matrix coordinate real general` where `coordinate` tells that the format is sparse matrix (only non-zero entries listed), `real` describes the field tyle and `general` tells that there is no symmetry. After comments, a line describes the matrix size `rows columns nonzeros`. Following this line, there are data entries where, in the `coordinate` format, the following is applied `row column value`.

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
