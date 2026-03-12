import argparse
import os
import networkx as nx
from scipy.io import mmread

parser = argparse.ArgumentParser(description="Benchmark Dijkstra's algorithm on a graph in Matrix Market format")
parser.add_argument("graph_file", help="Path to the graph file in Matrix Market format")
parser.add_argument("--source", type=int, default=0, help="Source vertex for Dijkstra's algorithm")

args = parser.parse_args()
# load matrix
A = mmread(args.graph_file)
# convert to graph
G = nx.from_scipy_sparse_array(A, create_using=nx.DiGraph)

# # run dijkstra
distances = nx.single_source_dijkstra_path_length(G, source=args.source)
graph_name = os.path.splitext(os.path.basename(args.graph_file))[0]
with open(f"networkx_{graph_name}_distances.txt", "w") as f:
    for node in range(G.number_of_nodes()):
        d = distances.get(node, float("inf"))
        f.write(f"{d:.6f}\n")