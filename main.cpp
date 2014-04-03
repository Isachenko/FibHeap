#include <stdio.h>
#include <ogdf/basic/HashArray.h>
#include <dijkstra.h>
#include <chrono>
#include <cmdParser.h>
#include <fibonacciHeap.h>
#include <bynaryHeap.h>
#include <memory>

using namespace ogdf;

char* inFile;
int startIndex = 0;
enum COST_TYPE{
    COST_TYPE_TIME = 0,
    COST_TYPE_DISTANCE
} costType;

enum PQ_TYPE {
    PQ_TYPE_FIB_HEAP = 0,
    PQ_TYPE_BIN_HEAP
} pqType;


//----------------
Graph graph;
EdgeArray<float> weight(graph);
NodeArray<edge> predecessor(graph);
NodeArray<float> dist(graph);
node startV;

//prin usage fuide
void printInfo() {
    printf("Usage: test [argument] file\n");
}

//parse cmd params and return "true" if all params are correct
bool parseCmdOptions(int argc, char *argv[]) {
    if (argc < 2) {
        printInfo();
        return false;
    }

    inFile = argv[argc - 1];


    if(cmdOptionExists(argv, argv+argc, "-h") || cmdOptionExists(argv, argv+argc, "-help")) {
        printInfo();
        return false;
    }

    if(cmdOptionExists(argv, argv+argc, "-pq")) {
        if(cmdOptionExists(argv, argv+argc, "bin")) {
            pqType = PQ_TYPE_BIN_HEAP;
        } else {
            pqType = PQ_TYPE_FIB_HEAP;
        }
    }

    if(cmdOptionExists(argv, argv+argc, "-cost")) {
        if(cmdOptionExists(argv, argv+argc, "time")) {
            costType = COST_TYPE_TIME;
        } else {
            costType = COST_TYPE_DISTANCE;
        }
    }


    if(cmdOptionExists(argv, argv+argc, "-start")) {
        char *strStartV = getCmdOption(argv, argv + argc, "-start");
        sscanf(strStartV, "%d", &startIndex);
    }
    return true;
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
    startV = idToNode[startIndex];

    int edgeCount;
    fscanf(inFile, "%d", &edgeCount);
    for (int i = 0; i < edgeCount; ++i) {
        int idU, idV, roadCategory;
        float time, range;
        fscanf(inFile, "%d %d %f %f %d", &idU, &idV, &time, &range, &roadCategory);
        edge e = graph.newEdge(idToNode[idU], idToNode[idV]);
        if (costType == COST_TYPE_TIME) {
            weight[e] = time;
        } else {
            weight[e] = range;
        }
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

//main test function
int main(int argc, char *argv[]) {
    if(!parseCmdOptions(argc, argv)) {
        return 0 ;
    }
    auto start = std::chrono::high_resolution_clock::now();

    parseFile(inFile);
    Dijkstra<float> djks;
    //dijkstra speed test
    if (pqType == PQ_TYPE_FIB_HEAP) {
        FibonacciHeap<float, node> queue;
        djks.call(graph, weight, startV, predecessor, dist, queue);
    } else {
        BynaryHeap<float, node> queue;
        djks.call(graph, weight, startV, predecessor, dist, queue);
    }

    auto finish = std::chrono::high_resolution_clock::now();
    double dijkstraTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count() / 1000000;
    printf("time : %3.10lf s\n", dijkstraTime);
    //printPredcessors();
    return 0;
}
