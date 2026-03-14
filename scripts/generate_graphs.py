import networkx as nx
import random
import argparse
from scipy.sparse import lil_matrix


def generate_random_directed_graph(n, density, weight_range=(1, 10)):
    """
    Generates a random directed graph with `n` nodes, given edge density, and random weights.

    :param n: Number of nodes in the graph
    :param density: Edge density (0 to 1) where 1 is a fully connected graph
    :param weight_range: Tuple indicating the range of random edge weights
    :return: Directed graph with random weights
    """
    G = nx.gnp_random_graph(n, density, directed=True)

    # Add random weights to each edge
    for u, v in G.edges():
        G[u][v]['weight'] = random.randint(weight_range[0], weight_range[1])

    return G


def generate_disconnected_directed_graph(n, num_components, density=0.3, weight_range=(1, 10)):
    """
    Generates a directed graph with `n` nodes split into `num_components` disconnected subgraphs.
    Nodes are partitioned roughly evenly across components; edges only exist within each component.

    :param n: Total number of nodes
    :param num_components: Number of disconnected components
    :param density: Intra-component edge density (0 to 1)
    :param weight_range: Range of random edge weights
    :return: Directed graph with disconnected components
    """
    G = nx.DiGraph()
    G.add_nodes_from(range(n))

    # Partition nodes into components
    indices = list(range(n))
    random.shuffle(indices)
    component_sizes = [n // num_components] * num_components
    for i in range(n % num_components):
        component_sizes[i] += 1

    offset = 0
    for size in component_sizes:
        component_nodes = indices[offset:offset + size]
        offset += size

        # Add edges within this component using gnp density
        for u in component_nodes:
            for v in component_nodes:
                if u != v and random.random() < density:
                    G.add_edge(u, v, weight=random.randint(weight_range[0], weight_range[1]))

    return G


def save_directed_graph_to_matrix_market(G, filename, weight_range=(0, 1)):
    """
    Saves the directed graph to a MatrixMarket file in coordinate format with weights,
    for a directed graph (not symmetric).

    :param G: The directed graph to be saved
    :param filename: The file to save the graph in MatrixMarket format
    :param weight_range: Range for random float weights written to file
    """
    n = len(G.nodes)
    matrix = lil_matrix((n, n))

    # Add directed edges to the matrix
    for u, v, data in G.edges(data=True):
        matrix[u, v] = random.uniform(weight_range[0], weight_range[1])

    # Write the MatrixMarket header
    with open(filename, 'w') as f:
        f.write('%%MatrixMarket matrix coordinate real general\n')
        f.write(f'{n} {n} {len(G.edges)}\n')  # Number of directed edges in the graph

        # Write each edge in (row, column, weight) format
        for i, j in zip(*matrix.nonzero()):
            f.write(f'{i+1} {j+1} {matrix[i, j]}\n')  # 1-based indexing


parser = argparse.ArgumentParser(description='Generate random directed graphs in MatrixMarket format')
parser.add_argument('--disconnected', action='store_true', help='Generate disconnected graphs instead of connected ones')
args = parser.parse_args()

if args.disconnected:
    for num_of_nodes in range(500, 5_000 + 1, 500):
        # 2 disconnected components, sparse intra-component edges
        G_2comp = generate_disconnected_directed_graph(num_of_nodes, num_components=2, density=0.1)
        save_directed_graph_to_matrix_market(G_2comp, f'mm/generated/disconnected2_graph{num_of_nodes}.mtx')
        print(f"saved 2-component disconnected graph for {num_of_nodes}")

        # 5 disconnected components, moderate density
        G_5comp = generate_disconnected_directed_graph(num_of_nodes, num_components=5, density=0.3)
        save_directed_graph_to_matrix_market(G_5comp, f'mm/generated/disconnected5_graph{num_of_nodes}.mtx')
        print(f"saved 5-component disconnected graph for {num_of_nodes}")

        # 10 disconnected components, denser intra-component edges
        G_10comp = generate_disconnected_directed_graph(num_of_nodes, num_components=10, density=0.5)
        save_directed_graph_to_matrix_market(G_10comp, f'mm/generated/disconnected10_graph{num_of_nodes}.mtx')
        print(f"saved 10-component disconnected graph for {num_of_nodes}")
else:
    for num_of_nodes in range(500, 5_000+1, 500):
        # Sparse Graph with Random Weights
        G_sparse = generate_random_directed_graph(num_of_nodes, 0.1)
        save_directed_graph_to_matrix_market(G_sparse, f'mm/generated/sparse_graph{num_of_nodes}.mtx')
        print(f"saved sparse matrix for {num_of_nodes}")

        G_medium = generate_random_directed_graph(num_of_nodes, 0.6)
        save_directed_graph_to_matrix_market(G_medium, f'mm/generated/mediumdense_graph{num_of_nodes}.mtx')
        print(f"saved medium dense matrix for {num_of_nodes}")

        # Dense Graph with Random Weights
        G_dense = generate_random_directed_graph(num_of_nodes, 0.9)
        save_directed_graph_to_matrix_market(G_dense, f'mm/generated/dense_graph{num_of_nodes}.mtx')
        print(f"saved dense matrix for {num_of_nodes}")
