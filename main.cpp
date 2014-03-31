#include <stdio.h>
#include <ogdf/basic/HashArray.h>
#include <dijkstra.h>
#include <chrono>
#include <cmdParser.h>

using namespace ogdf;

const char* inFile = "/home/isaac/Documents/OGDF/graphs/TX.tmp";

//----------------
Graph graph;
EdgeArray<float> timeWeight(graph);
EdgeArray<float> rangeWeight(graph);
NodeArray<edge> predecessor(graph);
NodeArray<float> distance(graph);
node startV;

//
void printInfo() {
    //printf("")
}

void parseCmdOptions(int argc, char *argv[]) {
    if(cmdOptionExists(argv, argv+argc, "-h") || cmdOptionExists(argv, argv+argc, "-help")) {

    }

    char * filename = getCmdOption(argv, argv + argc, "-f");

    if (filename) {
        // Do interesting things
        // ...
    }

}

//read graph from file
void parseFile(const char* fileName) {
    HashArray<int, node> idToNode;
    FILE* inFile = fopen(fileName, "r");

    int graphSize;
    fscanf(inFile, "%d", &graphSize);
    for (int i = 0; i < graphSize; ++i) {
        int id, longitude, latitude;
        fscanf(inFile, "%d %d %d", &id, &longitude, &latitude);
        node v = graph.newNode(id);
        idToNode[id] = v;
    }
    startV = idToNode[0];

    int edgeCount;
    fscanf(inFile, "%d", &edgeCount);
    for (int i = 0; i < edgeCount; ++i) {
        int idU, idV, roadCategory;
        float time, range;
        fscanf(inFile, "%d %d %f %f %d", &idU, &idV, &time, &range, &roadCategory);
        edge e = graph.newEdge(idToNode[idU], idToNode[idV]);
        timeWeight[e] = time;
        rangeWeight[e] = range;
    }
}

//print predecessor for all nodes
void printPredcessors() {
    node v;
    forall_nodes(v, graph) {
        edge e = predecessor[v];
        if (nullptr == e) {
            printf("predecessor of %d is NULL\n", v->index());
        } else {
            node w = e->opposite(v);
            printf("predecessor of %d is %d\n", v->index(), w->index());
        }
    }
}

int main(int argc, char *argv[]) {
    parseCmdOptions(argc, argv);
    parseFile(inFile);
    Dijkstra<float> djks;
    //dijkstra speed test
    auto start = std::chrono::high_resolution_clock::now();
    djks.call(graph, timeWeight, startV, predecessor, distance);
    auto finish = std::chrono::high_resolution_clock::now();
    double dijkstraTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count() / 1000000;
    printf("time : %3.10lf s\n", dijkstraTime);
    //printPredcessors();

    return 0;
}
