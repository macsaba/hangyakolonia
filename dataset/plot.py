import numpy as np
import matplotlib.pyplot as plt
import math
import csv

nodes = []

with open('california/nodes.csv', newline='') as nodescsv:
    myreader = csv.reader(nodescsv, delimiter=' ', quotechar='|')
    for row in myreader:
        node = []
        for part in row:
            node.append(float(part))
        nodes.append(node[1:3])

npnodes = np.array(nodes)


i = 0
edges = []


with open('california/edges.csv', newline='') as edgescsv:
    myreader = csv.reader(edgescsv, delimiter=' ', quotechar='|')
    for row in myreader:
        edge = []
        for part in row:
            edge.append(part)
        lines = plt.plot([npnodes[int(edge[1])][0], npnodes[int(edge[2])][0]], [npnodes[int(edge[1])][1], npnodes[int(edge[2])][1]])
        edges.append(edge)

print("step2")

plt.show()
