#ifndef BYNARYHEAP_H
#define BYNARYHEAP_H

#include <functional>
#include <cmath>
#include <priorityQueue.h>
#include <memory>
#include <queue>
#include <vector>

using namespace ogdf;

template<class PRIO = int, class VALUE = int, class CMP = std::less<PRIO> >
class BynaryHeap : public PriorityQueue<PRIO, VALUE, CMP>
{
private:
    class BinHeapItem : public PriorityQueue<PRIO, VALUE, CMP>::PriorityQueueItem
    {
    private:
        friend class BynaryHeap<PRIO, VALUE, CMP>;
    public:
        BinHeapItem(const PRIO &prio, const VALUE &value) :
            PriorityQueue<PRIO, VALUE, CMP>::PriorityQueueItem(prio, value) { }
    };

    class ItemComparator
    {
        CMP m_cmp;
    public:
        ItemComparator(const CMP &cmp = CMP()) : m_cmp(cmp) { }

        bool operator () (std::shared_ptr<BinHeapItem> const &v1, std::shared_ptr<BinHeapItem> const &v2) const {
            bool res = this->m_cmp(v1->m_prio, v2->m_prio);
            return res;
        }
    };

    typedef std::shared_ptr<BinHeapItem> item;
    typedef typename PriorityQueue<PRIO, VALUE, CMP>::item baseItem;

    std::priority_queue<item, std::vector<item>, ItemComparator > m_queue;

public:
    // constructs an empty heap with a given compare functor.
    explicit BynaryHeap(const CMP &cmp = CMP()) :
        PriorityQueue<PRIO, VALUE, CMP>(cmp) { }

    //VALUE &value(item it);

    // inserts a new element
    baseItem insert(const PRIO &prio, const VALUE &value) {
        item newItem(std::make_shared<BinHeapItem>(prio, value));
        m_queue.push(newItem);
        return newItem;
    }

    // decreases the priority of item to prio
    void decPrio(baseItem const &v, const PRIO &prio) {
        item newItem(std::make_shared<BinHeapItem>(prio, value(v)));
        m_queue.push(newItem);
    }

    // deletes the current minimum
    void delMin() {
        m_queue.pop();
    }

    // returns the minimum element
    baseItem findMin() const {
        return m_queue.top();
    }

    // returns number of elements stored in the heap
    size_t size() const {
        return m_queue.size();
    }

    // returns true if the heap is empty
    bool empty() {
        return m_queue.empty();
    }
};

#endif // BYNARYHEAP_H
