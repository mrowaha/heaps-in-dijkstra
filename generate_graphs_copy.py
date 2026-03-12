import networkx as nx
import random
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

def save_directed_graph_to_matrix_market(G, filename, weight_range=(0,1)):
    """
    Saves the directed graph to a MatrixMarket file in coordinate format with weights,
    for a directed graph (not symmetric).

    :param G: The directed graph to be saved
    :param filename: The file to save the graph in MatrixMarket format
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



# Example usage of the functions:

num_of_nodes = 300

# Sparse Graph with Random Weights
G_sparse = generate_random_directed_graph(num_of_nodes, 0.1)
save_directed_graph_to_matrix_market(G_sparse, f'sparse_graph{num_of_nodes}.mtx')

G_medium = generate_random_directed_graph(num_of_nodes, 0.6)
save_directed_graph_to_matrix_market(G_medium, f'mediumdense_graph{num_of_nodes}.mtx')

# Dense Graph with Random Weights
G_dense = generate_random_directed_graph(num_of_nodes, 0.9)
save_directed_graph_to_matrix_market(G_dense, f'dense_graph{num_of_nodes}.mtx')
