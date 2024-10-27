# Importing necessary libraries
import networkx as nx
import matplotlib.pyplot as plt
import time
import os 
import signal
signal.signal(signal.SIGINT, signal.SIG_DFL)
# Defining the GraphVisualization class
class GraphVisualization:

    def __init__(self):
        # List to store the edges and labels of the graph
        self.reset()

        self.color_dict = { 'init':(0,0,1),
                            'default':(1,0,0),
                            'final':(0,1,0)}
        
    def reset(self):
        self.visual = []
        self.labels = {}  # Dictionary to store edge labels
        self.node_colors = {}  # Dictionary to store edge labels
        self.init_node = -1
        self.final_node = -1


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
        G.add_nodes_from([self.init_node, self.final_node])
        
        if self.init_node >= 0:
            self.node_colors[self.init_node] = self.color_dict['init']
        if self.final_node >= 0 and self.final_node < len(self.node_colors) :
            self.node_colors[self.final_node] = self.color_dict['final']
        
        colors = []
        print(colors)
        for node in G.nodes():
            print(node)
            if node == self.init_node:
                colors.append(self.color_dict['init'])
            elif node == self.final_node:
                colors.append(self.color_dict['final'])
            else:
                colors.append(self.color_dict['default'])

        self.ax.clear()

        pos = nx.shell_layout(G)  # Positioning the nodes
        pos = nx.planar_layout(G)  # Positioning the nodes
        nx.draw_networkx(G, pos, arrows=True , node_color = colors, ax=self.ax)
        
        # Draw edge labels, handling multiple edges by appending labels
        nx.draw_networkx_edge_labels(G, pos, edge_labels=self.labels, connectionstyle='arc3, rad = 0.1',ax=self.ax)

        # drawing updated values
        self.figure.canvas.draw()
    
        # This will run the GUI event
        # loop until all UI events
        # currently waiting have been processed
        self.figure.canvas.flush_events()
        
        plt.draw()

    # Method to load edges and labels from a file
    def load_from_file(self, file_path):
        with open(file_path, 'r') as f:
            for line in f:
                # Parse each line, expecting format "x -> y [label]"

                if '->' in line:
                    parts = line.strip().split('->')
                    if len(parts) == 2:
                        a_b = parts[1].split('[')
                        a = int(parts[0].strip())
                        b = int(a_b[0].strip())
                        label = a_b[1].strip('] ') if len(a_b) > 1 else ''
                        self.addEdge(a, b, label)
                        
                        for node_id in [a,b]:
                            if node_id not in self.node_colors:
                                 self.node_colors[node_id] = self.color_dict['default']                               

                if 's:' in line:
                    parts = line.strip().split(':')
                    if len(parts) == 2:
                        self.init_node = int(parts[1].strip())
                        print('init_node = ' , self.init_node)

                if 'f:' in line:
                    parts = line.strip().split(':')
                    if len(parts) == 2:
                        self.final_node = int(parts[1].strip())
                        print('final = ' , self.final_node)

    def load_file_contents(self, file):
        
        self.reset()
        # Load edges and labels from a file (replace 'state_machine.txt' with your file path)
        self.load_from_file(file)

        # Visualize the graph
        self.visualize()


    def watch_file(self, file_path, check_interval=1):

        # Initialize the plot
        plt.ion()  # Enable interactive mode
        self.figure, self.ax = plt.subplots()

        """Watches a file for changes and reloads contents if modified."""
        if not os.path.exists(file_path):
            print(f"File '{file_path}' does not exist.")
            return

        last_mtime = 0
        print("Watching for file changes...")

        while True:            
            try:
                current_mtime = os.path.getmtime(file_path)
                if current_mtime != last_mtime:
                    print("File has changed, reloading contents...")
                    self.load_file_contents(file_path)               
                    last_mtime = current_mtime
            except FileNotFoundError:
                print(f"File '{file_path}' was deleted.")
                break
            time.sleep(check_interval)

if __name__ == "__main__":

     # Driver code
    G = GraphVisualization()


    file_path = "engine.txt"  # Replace with your file path
    G.watch_file(file_path)