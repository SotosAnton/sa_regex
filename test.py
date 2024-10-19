# Importing necessary libraries
import networkx as nx
import matplotlib.pyplot as plt

# Defining the GraphVisualization class
class GraphVisualization:

    def __init__(self):
        # List to store the edges and labels of the graph
        self.visual = []
        self.labels = {}  # Dictionary to store edge labels

    # Method to add an edge and label to the visual list
    def addEdge(self, a, b, label=''):
        temp = [a, b]
        self.visual.append(temp)

        # If multiple edges between the same nodes exist, append labels
        if (a, b) in self.labels:
            self.labels[(a, b)] += f'\n {label}'  # Append new label to the existing one
        else:
            self.labels[(a, b)] = label  # Add new label for the first time

    # Method to visualize the graph using networkx and matplotlib
    def visualize(self):
        G = nx.MultiDiGraph()  # MultiDiGraph allows multiple edges between the same nodes
        G.add_edges_from(self.visual)

        pos = nx.shell_layout(G)  # Positioning the nodes
        nx.draw_networkx(G, pos, arrows=True)
        
        # Draw edge labels, handling multiple edges by appending labels
        nx.draw_networkx_edge_labels(G, pos, edge_labels=self.labels)

        plt.show()

    # Method to load edges and labels from a file
    def load_from_file(self, file_path):
        with open(file_path, 'r') as f:
            for line in f:
                # Parse each line, expecting format "x -> y [label]"
                parts = line.strip().split('->')
                if len(parts) == 2:
                    a_b = parts[1].split('[')
                    a = int(parts[0].strip())
                    b = int(a_b[0].strip())
                    label = a_b[1].strip('] ') if len(a_b) > 1 else ''
                    self.addEdge(a, b, label)

# Driver code
G = GraphVisualization()

# Load edges and labels from a file (replace 'state_machine.txt' with your file path)
G.load_from_file('engine.txt')

# Visualize the graph
G.visualize()
