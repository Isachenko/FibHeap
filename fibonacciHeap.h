#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include <functional>
#include <cmath>
#include <priorityQueue.h>
#include <memory>

using namespace ogdf;

template<class PRIO = int, class VALUE = int, class CMP = std::less<PRIO> >
class FibonacciHeap : public PriorityQueue<PRIO, VALUE, CMP>
{
private:
    class FibHeapItem : public PriorityQueue<PRIO, VALUE, CMP>::PriorityQueueItem
    {
    private:
        friend class FibonacciHeap<PRIO, VALUE, CMP>;

        size_t m_rank;
        std::shared_ptr<FibHeapItem> m_right;
        std::shared_ptr<FibHeapItem> m_left;
        std::shared_ptr<FibHeapItem> m_firsChild;
        std::shared_ptr<FibHeapItem> m_parent;
        bool m_mark;
    public:
        FibHeapItem(const PRIO &prio, const VALUE &value) :
            PriorityQueue<PRIO, VALUE, CMP>::PriorityQueueItem(prio, value),
            m_rank(0),
            m_right(nullptr),
            m_left(nullptr),
            m_firsChild(nullptr),
            m_parent(nullptr),
            m_mark(false) {}
    };
    typedef std::shared_ptr<FibHeapItem> item;
    typedef std::weak_ptr<FibHeapItem> weakItem;
    typedef typename PriorityQueue<PRIO, VALUE, CMP>::item baseItem;
    item m_min;

    void checkAndSetIfLessThenMin(item const &v) {
        if (this->m_cmp(v->m_prio, m_min->m_prio)) {
            m_min = v;
        }
    }

    void pushVRightOfU(item const &v, item const &u) {
        v->m_right = u->m_right;
        v->m_left = u;
        v->m_right->m_left = v;
        v->m_left->m_right = v;
        v->m_parent = u->m_parent;
    }

    void pushItemToRootList(item const &v) {
        v->m_parent = nullptr;
        if (!m_min) {
            m_min = v;
            m_min->m_right = m_min;
            m_min->m_left = m_min;
        } else {
            pushVRightOfU(v, m_min);
            checkAndSetIfLessThenMin(v);
        }
    }

    // min may be set wrong
    void pushListToRightOfV(item const &begin, item const &end, item const &v) {
        begin->m_left = v;
        end->m_right = v->m_right;
        begin->m_left->m_right = begin;
        end->m_right->m_left = end;

    }

    void cleanItem(item const &v) {
        v->m_left = nullptr;
        v->m_right = nullptr;
    }

    void pop(item const &v) {
        v->m_left->m_right = v->m_right;
        v->m_right->m_left = v->m_left;
    }


    void setVAsChildOfU(item const &v, item const &u) {
        pop(v);
        if (u->m_firsChild) {
            pushVRightOfU(v, u->m_firsChild);
        } else {
            u->m_firsChild = v;
            v->m_right = v;
            v->m_left = v;
        }
        u->m_rank += 1;
        v->m_parent = u;
    }

    const item &unity(item const &v, item const &u) {
        if (this->m_cmp(v->m_prio, u->m_prio)) {
            setVAsChildOfU(u, v);
            return v;
        } else {
            setVAsChildOfU(v, u);
            return u;
        }
    }

    void consolidate() {
        if (!m_min) {
            return;
        }
        size_t maxRank = (size_t)log2(this->m_size) + 2; //  ToDo: thik about it
        item rankToItem[maxRank];
        for (size_t i = 0; i < maxRank; ++i) {
            rankToItem[i] = nullptr;
        }
        item curItem(m_min);
        item newMin(m_min);
        while (true) {
            size_t rank = curItem->m_rank;
            if ((rankToItem[rank] == curItem) || curItem->m_parent != nullptr) { // all roots have been gone
                break;
            }
            item nextItem = curItem->m_right;
            while ((rank < maxRank) && rankToItem[rank]) {
                curItem = unity(rankToItem[rank], curItem);
                rankToItem[rank++] = nullptr;
            }
            if (newMin->m_parent || this->m_cmp(curItem->m_prio, newMin->m_prio)) {
                newMin = curItem;
            }
            rankToItem[rank] = curItem;
            curItem = nextItem;
        }
        //set min

        m_min = newMin;
    }

    void tearOff(item const &v) {
        if (v->m_parent) {
            item parent(v->m_parent);
            item firstChild = parent->m_firsChild;
            if (firstChild == firstChild->m_right) {    // only one child
                parent->m_firsChild = nullptr;
            } else {                                    // more then one child
                if (firstChild == v) {
                    parent->m_firsChild = v->m_right;
                }
                pop(v);
            }
            pushVRightOfU(v, m_min);
            checkAndSetIfLessThenMin(v);
            v->m_mark = false;
            parent->m_rank -= 1;
            if (parent->m_mark) {
                tearOff(parent);
            } else {
                parent->m_mark = true;
            }
        } else {
            checkAndSetIfLessThenMin(v);
        }
    }

public:

    // constructs an empty heap with a given compare functor.
    explicit FibonacciHeap(const CMP &cmp = CMP()) :
        PriorityQueue<PRIO, VALUE, CMP>(cmp),
        m_min(nullptr) { }

    //VALUE &value(item it);

    // inserts a new element
    baseItem insert(const PRIO &prio, const VALUE &value) {
        item newItem(std::make_shared<FibHeapItem>(prio, value));
        pushItemToRootList(newItem);
        this->m_size += 1;
        return newItem;
    }


    // decreases the priority of item to prio
    void decPrio(baseItem const &v, const PRIO &prio) {
        item u = std::dynamic_pointer_cast<FibHeapItem>(v);
        if (this->m_cmp(u->m_prio, prio)) {
            return;
        }
        u->m_prio = prio;
        tearOff(u);
    }

    // deletes the current minimum
    void delMin() {
        if (!m_min) {
            return;
        }
        this->m_size -= 1;
        item newMin;
        if (m_min->m_firsChild) { //if have child
            item firstChild = m_min->m_firsChild;
            item lastChild = firstChild->m_left;
            //delete link to parent
            item curItem = firstChild;
            do {
                curItem->m_parent = nullptr;
                curItem = curItem->m_right;
            } while (curItem != lastChild);
            lastChild->m_parent = nullptr;

            if (m_min->m_left != m_min) { // more then one root
                m_min->m_left->m_right = m_min->m_right;
                m_min->m_right->m_left = m_min->m_left;
                newMin = m_min->m_left;
                pushListToRightOfV(firstChild, lastChild, newMin);
            } else {
                newMin = firstChild;
            }
        } else {
            if (m_min->m_left != m_min) { // more then one root
                pop(m_min);
                newMin = m_min->m_left;
            } else {
                newMin = nullptr;
            }
        }
        cleanItem(m_min);
        m_min = newMin;
        consolidate();
    }

    // returns the minimum element
    baseItem findMin() const {
        return m_min;
    }

    //You may also add a copy constructor and assignment operator (both with and without move semantics) if you wish.


};
#endif // FIBONACCIHEAP_H
