#ifndef FIBONACCIHEAP_H
#define FIBONACCIHEAP_H

#include<functional>
#include<cmath>

using namespace ogdf;

template<class PRIO = int, class VALUE = int, class CMP = std::less<PRIO> >
class FibonacciHeap {
private:
    class FibHeapItem
    {
    private:
        friend class FibonacciHeap<PRIO, VALUE, CMP>;

        PRIO m_prior;
        VALUE m_value;
        size_t m_rank;
        FibHeapItem* m_right;
        FibHeapItem* m_left;
        FibHeapItem* m_firsChild;
        FibHeapItem* m_parent;
        bool m_mark;

        FibHeapItem(const PRIO &prior, const VALUE &value) :
            m_prior(prior), m_value(value), m_rank(0), m_right(nullptr),
            m_left(nullptr), m_firsChild(nullptr), m_parent(nullptr), m_mark(false) {}
    };
    typedef FibHeapItem* item;
    size_t m_size;
    FibHeapItem* m_min;
    CMP m_cmp;


    void pushVRightOfU(item v, item u) {
        v->m_right = u->m_right;
        v->m_left = u;
        v->m_right->m_left = v;
        v->m_left->m_right = v;
        v->m_parent = u->m_parent;
    }

    item pushItemToRootList(item v) {
        v->m_parent = nullptr;
        if (!m_min) {
            m_min = v;
            m_min->m_right = m_min;
            m_min->m_left = m_min;
        } else {
            pushVRightOfU(v, m_min);
            if (v->m_prior < m_min->m_prior) {
                m_min = v;
            }
        }
        return m_min;
    }

    // min may be set wrong
    item pushListToRightOfV(item begin, item end, item v) {
        if (!v) {
            v = begin;
        } else {
            begin->m_left = v;
            end->m_right = v->m_right;
            begin->m_left->m_right = begin;
            end->m_right->m_left = end;
        }
        return v;
    }

    void pop(item v) {
        v->m_left->m_right = v->m_right;
        v->m_right->m_left = v->m_left;
    }

    void setVAsChildOfU(item v, item u) {
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

    item unity(item v, item u) {
        if (v->m_prior < u->m_prior) {
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
        size_t maxRank = (size_t)log2(m_size) + 1;
        item rankToItem[maxRank];
        for (int i = 0; i < maxRank; ++i) {
            rankToItem[i] = nullptr;
        }
        item curItem = m_min;
        while (true) {
            size_t rank = curItem->m_rank;
            //ToDo: check loop
            if ((rankToItem[rank] == curItem) || curItem->m_parent != nullptr) { // all roots have been gone
                break;
            }
            item nextItem = curItem->m_right;
            while ((rank < maxRank) && rankToItem[rank]) {
                curItem = unity(rankToItem[rank], curItem);
                rankToItem[rank++] = nullptr;
            }
            rankToItem[rank] = curItem;
            curItem = nextItem;
        }
        //set min
        for (size_t i = 0; i < maxRank; ++i) {
            curItem = rankToItem[i];
            if (curItem && (!m_min || (curItem->m_prior < m_min->m_prior))) {
                m_min = curItem;
            }
        }
    }


public:

    // constructs an empty heap with a given compare functor.
    explicit FibonacciHeap(const CMP &cmp = CMP()) : m_size(0), m_min(nullptr), m_cmp(cmp) {}

    // returns number of elements stored in the heap
    size_t size() const {
        return m_size;
    }

    // returns true if the heap is empty
    bool empty() {
        if (0 == m_size) {
            return false;
        }
        return true;
    }

    // access to priority and value of an element
    const PRIO &prio(item it) const {
        return it->m_prior;
    }
    const VALUE &value(item it) const {
        return it->m_value;
    }
    //VALUE &value(item it);

    // inserts a new element
    item insert(const PRIO &prio, const VALUE &value) {
        item newItem = new FibHeapItem(prio, value);
        pushItemToRootList(newItem);
        ++m_size;
        return newItem;
    }

    // decreases the priority of item to prio
//    void decPrio(item, const PRIO &prio) {

//    }

    // deletes the current minimum
    void delMin() {
        if (!m_min) {
            return;
        }
        --m_size;
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
                m_min->m_left->m_right = m_min->m_right;
                m_min->m_right->m_left = m_min->m_left;
                newMin = m_min->m_left;
            } else {
                newMin = nullptr;
            }
        }
        delete m_min;
        m_min = newMin;
        consolidate();
    }

    // returns the minimum element
    item findMin() const {
        return m_min;
    }

    //You may also add a copy constructor and assignment operator (both with and without move semantics) if you wish.


};
#endif // FIBONACCIHEAP_H
