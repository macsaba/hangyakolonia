import numpy as np
import math
import csv
import sys

N = 21048
nodes = []
npdist = np.zeros(shape=(N,N))

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
        #lines = plt.plot([npnodes[int(edge[1])][0], npnodes[int(edge[2])][0]], [npnodes[int(edge[1])][1], npnodes[int(edge[2])][1]])
        npdist[int(edge[1]), int(edge[2])] = float(edge[3])
        npdist[int(edge[2]), int(edge[1])] = float(edge[3])
        edges.append(edge)

degree = 0
numOf2Deg = 0
neighbours = [0, 0]
indexesToDelete = np.array([])
n = N
for i in range(N):
    print(i)
    degree = 0
    for j in range(N):
        if npdist[i, j] != 0:
            if degree < 2:
                neighbours[degree] = j;
            degree +=1

    if degree == 2:
        numOf2Deg +=1
        dist = npdist[i, neighbours[0]]+npdist[i, neighbours[1]]
        npdist[i, neighbours[0]] = 0;
        npdist[i, neighbours[1]] = 0;
        npdist[neighbours[0], i] = 0;
        npdist[neighbours[1], i] = 0;
        indexesToDelete = np.append(indexesToDelete, i)
        if dist < npdist[neighbours[0], neighbours[1]] or npdist[neighbours[0], neighbours[1]] == 0:
            npdist[neighbours[0], neighbours[1]] = dist
            npdist[neighbours[1], neighbours[0]] = dist

print(numOf2Deg)
print(indexesToDelete)
k = 0
q = 0
npdist2 = np.zeros(shape=(N-numOf2Deg,N-numOf2Deg))
r = 0
s = 0

indexesToDelete = np.append(indexesToDelete, -1)
originalIndexes = np.zeros(N-numOf2Deg)

for i in range(N):
    print(i)
    if i != indexesToDelete[k]:
        originalIndexes[r] = i
        q = 0
        s = 0
        for j in range(N):
            if j != indexesToDelete[q]:
                npdist2[r, s] = npdist[i, j]
                s += 1
            else:
                q += 1 #nem kell
        r += 1
    else:
        k += 1
id = 0;
print("numOf2Deg")

print(numOf2Deg)

with open('dist.csv', 'w', newline='') as csvfile:
    spamwriter = csv.writer(csvfile, delimiter=' ')
    for i in range(N-numOf2Deg):
        for j in range(N-numOf2Deg):
            if npdist2[i, j] != 0:
                spamwriter.writerow([id, i, j, npdist2[i, j], originalIndexes[i], originalIndexes[j]])
                id += 1
