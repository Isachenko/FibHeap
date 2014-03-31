#ifdef _MSC_VER
#pragma once
#endif

#ifndef OGDF_DIJKSTRA_H_
#define OGDF_DIJKSTRA_H_

#include <iostream>
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/BinaryHeap2.h>
#include <limits>

using namespace ogdf;

template<class T>
class Dijkstra {
public:
    void call(const Graph &G, const EdgeArray<T> &weight, node s, NodeArray<edge> &predecessor, NodeArray<T> &distance) {
    BinaryHeap2<T, node> queue(G.numberOfNodes());
        int* qpos = new int[G.numberOfNodes()];
        NodeArray<int> vIndex(G);
        T maxEdgeWeight = 0;
        int i = 0;
        node v;
        edge e;

        // determining maximum edge weight
        forall_edges(e, G)
        {
            if (maxEdgeWeight < weight[e]) {
                maxEdgeWeight = weight[e];
            }
        }

        // setting distances to "infinity"
        forall_nodes(v, G)
        {
            vIndex[v] = i;
            distance[v] = std::numeric_limits<T>::max() - maxEdgeWeight - 1;
            predecessor[v] = 0; //?
            queue.insert(v, distance[v], &qpos[i++]);
        }

        distance[s] = 0;
        queue.decreaseKey(qpos[vIndex[s]], 0);

        while (!queue.empty()) {
            v = queue.extractMin();
            forall_adj_edges(e, v)
            {
                node w = e->opposite(v);
                if (distance[w] > distance[v] + weight[e]) {
                    distance[w] = distance[v] + weight[e];
                    queue.decreaseKey(qpos[vIndex[w]], distance[w]);
                    predecessor[w] = e;
                }
            }
        }
        delete[] qpos;
    }
};


#endif /* OGDF_DIJKSTRA_H_ */
