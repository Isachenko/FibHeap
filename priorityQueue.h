/*
* $Author: Isachenko Andrew $
***************************************************************/

#ifdef _MSC_VER
#pragma once
#endif

#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <functional>
#include <cmath>
#include <memory>

using namespace ogdf;

template<class PRIO = int, class VALUE = int, class CMP = std::less<PRIO> >
class PriorityQueue {
protected:
    class PriorityQueueItem
    {
    protected:
        friend class PriorityQueue<PRIO, VALUE, CMP>;

        PRIO m_prio;
        VALUE m_value;

        PriorityQueueItem(const PRIO &prio, const VALUE &value) :
            m_prio(prio), m_value(value) { }

        virtual ~PriorityQueueItem() {}
    };
    size_t m_size;
    CMP m_cmp;
public:
    /*! Inner presentation of priority queue item.*/
    typedef std::shared_ptr<PriorityQueueItem> item;

    //! Constructor.
    /*! constructs an empty heap with a given compare functor.*/
    explicit PriorityQueue(const CMP &cmp = CMP()) : m_size(0), m_cmp(cmp) {}

    virtual ~PriorityQueue() {}

    // returns number of elements stored in the heap
    virtual size_t size() const {
        return m_size;
    }

    // returns true if the heap is empty
    virtual bool empty() {
        if (0 == m_size) {
            return true;
        }
        return false;
    }

    // access to priority and value of an element
    virtual const PRIO &prio(item it) const {
        return it->m_prio;
    }
    virtual const VALUE &value(item it) const {
        return it->m_value;
    }
    //VALUE &value(item it);

    //! Insert.
    /*! insert a new alement.
        \return created item
    */
    virtual item insert(const PRIO &prio, const VALUE &value) = 0;

    //! Decrease priority.
    /*! decreases the priority of item to prio */
    virtual void decPrio(item const &itm, const PRIO &prio) = 0;

    //! Delete Minimum
    /*! deletes the current minimum */
    virtual void delMin() = 0;

    //! Find Minimum
    /*! returns the minimum element
        \return current min
    */
    virtual item findMin() const = 0;

    //You may also add a copy constructor and assignment operator (both with and without move semantics) if you wish.


};

#endif // PRIORITYQUEUE_H
