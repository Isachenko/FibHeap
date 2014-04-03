#include <stdio.h>
#include <ogdf/basic/HashArray.h>
#include <dijkstra.h>
#include <chrono>
#include <cmdParser.h>
#include <fibonacciHeap.h>
#include <bynaryHeap.h>
#include <memory>

//#define FIB_TEST

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
EdgeArray<float> timeWeight(graph);
EdgeArray<float> rangeWeight(graph);
NodeArray<edge> predecessor(graph);
NodeArray<float> dist(graph);
node startV;

//
void printInfo() {
    printf("Usage: test [argument] file\n");
}

//reurn treu if all parametrs are correct
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

void fibHeapTest() {
    int n = 1000000;
    typedef typename PriorityQueue<double, int>::item qItem;
    std::unique_ptr<qItem[]> qpos( new qItem[n] );
    PriorityQueue<double, int, std::less<double> > *pq = new FibonacciHeap<double, int, std::less<double> >();
    for(int i = 0; i < n; ++i) {
        auto itm = pq->insert(i, i);
        qpos[i] = itm;
    }
    auto x = pq->findMin();
    int ans = pq->prio(x);
    //printf("prior: %d\n", ans);
    pq->delMin();
    for(int i = n - 1; i > 0; --i) {
        pq->decPrio(qpos[i], 0);
    }

    for(int i = 0; i < n-1; ++i) {
        auto x = pq->findMin();
        int ans = pq->prio(x);
        //printf("prior: %d\n", ans);
        pq->delMin();
    }
    printf("end\n");
}

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
#ifdef FIB_TEST
    fibHeapTest();
#else
    if(!parseCmdOptions(argc, argv)) {
        return 0 ;
    }
    parseFile(inFile);
    Dijkstra<float> djks;
    //dijkstra speed test
    //FibonacciHeap<float, node> queue;
    BynaryHeap<float, node> queue;
    djks.call(graph, timeWeight, startV, predecessor, dist, queue);
#endif
    auto finish = std::chrono::high_resolution_clock::now();
    double dijkstraTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count() / 1000000;
    printf("time : %3.10lf s\n", dijkstraTime);
    //printPredcessors();
    return 0;
}
