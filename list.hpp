#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        node *prev, *next;
        T *val;
        node(): prev(nullptr), next(nullptr), val(nullptr) {}
        explicit node(const T &v): prev(nullptr), next(nullptr), val(new T(v)) {}
        ~node(){ if (val) { delete val; val = nullptr; } }
    };

protected:
    /**
     * add data members for linked list as protected members
     */
    node *head = nullptr;
    size_t sz = 0;

    /**
     * insert node cur before node pos
     * return the inserted node cur
     */
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        ++sz;
        return cur;
    }
    /**
     * remove node pos from list (no need to delete the node)
     * return the removed node pos
     */
    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        pos->prev = pos->next = nullptr;
        --sz;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        list *owner = nullptr;
        node *ptr = nullptr;
        friend class const_iterator;
        friend class list;
    public:
        iterator() = default;
        iterator(list *o, node *p) : owner(o), ptr(p) {}
        /**
         * iter++
         */
        iterator operator++(int) {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        /**
         * ++iter
         */
        iterator & operator++() {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        /**
         * iter--
         */
        iterator operator--(int) {
            if (!owner || !ptr) throw invalid_iterator();
            if (owner->sz == 0) throw invalid_iterator();
            if (ptr->prev == owner->head) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        /**
         * --iter
         */
        iterator & operator--() {
            if (!owner || !ptr) throw invalid_iterator();
            if (owner->sz == 0) throw invalid_iterator();
            if (ptr->prev == owner->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        /**
         * TODO *it
         * remember to throw if iterator is invalid
         */
        T & operator *() const {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            return *(ptr->val);
        }
        /**
         * TODO it->field
         * remember to throw if iterator is invalid
         */
        T * operator ->() const {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            return ptr->val;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && ptr == rhs.ptr; }
        bool operator==(const const_iterator &rhs) const { return rhs == *this; }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const { return !(rhs == *this); }
    };
    /**
     * TODO
     * has same function as iterator, just for a const object.
     * should be able to construct from an iterator.
     */
    class const_iterator {
    private:
        const list *owner = nullptr;
        node *ptr = nullptr;
        friend class iterator;
        friend class list;
    public:
        const_iterator() = default;
        const_iterator(const list *o, node *p) : owner(o), ptr(p) {}
        const_iterator(const iterator &it) : owner(it.owner), ptr(it.ptr) {}
        const_iterator operator++(int) {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (!owner || !ptr) throw invalid_iterator();
            if (owner->sz == 0) throw invalid_iterator();
            if (ptr->prev == owner->head) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (!owner || !ptr) throw invalid_iterator();
            if (owner->sz == 0) throw invalid_iterator();
            if (ptr->prev == owner->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        const T & operator *() const {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            return *(ptr->val);
        }
        const T * operator ->() const {
            if (!owner || !ptr || ptr == owner->head) throw invalid_iterator();
            return ptr->val;
        }
        bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && ptr == rhs.ptr; }
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && ptr == rhs.ptr; }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    };
    /**
     * TODO Constructs
     * Atleast two: default constructor, copy constructor
     */
    list() {
        head = new node();
        head->next = head->prev = head;
        sz = 0;
    }
    list(const list &other) {
        head = new node();
        head->next = head->prev = head;
        sz = 0;
        for (auto it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
    }
    /**
     * TODO Destructor
     */
    virtual ~list() {
        clear();
        if (head) { delete head; head = nullptr; }
    }
    /**
     * TODO Assignment operator
     */
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (auto it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
        return *this;
    }
    /**
     * access the first / last element
     * throw container_is_empty when the container is empty.
     */
    const T & front() const {
        if (sz == 0) throw container_is_empty();
        return *(head->next->val);
    }
    const T & back() const {
        if (sz == 0) throw container_is_empty();
        return *(head->prev->val);
    }
    /**
     * returns an iterator to the beginning.
     */
    iterator begin() { return iterator(this, head->next); }
    const_iterator cbegin() const { return const_iterator(this, head->next); }
    /**
     * returns an iterator to the end.
     */
    iterator end() { return iterator(this, head); }
    const_iterator cend() const { return const_iterator(this, head); }
    /**
     * checks whether the container is empty.
     */
    virtual bool empty() const { return sz == 0; }
    /**
     * returns the number of elements
     */
    virtual size_t size() const { return sz; }

    /**
     * clears the contents
     */
    virtual void clear() {
        node *cur = head ? head->next : nullptr;
        while (cur && cur != head) {
            node *nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        if (head) head->next = head->prev = head;
        sz = 0;
    }
    /**
     * insert value before pos (pos may be the end() iterator)
     * return an iterator pointing to the inserted value
     * throw if the iterator is invalid
     */
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this || pos.ptr == nullptr) throw invalid_iterator();
        node *cur = new node(value);
        insert(pos.ptr, cur);
        return iterator(this, cur);
    }
    /**
     * remove the element at pos (the end() iterator is invalid)
     * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
     * throw if the container is empty, the iterator is invalid
     */
    virtual iterator erase(iterator pos) {
        if (sz == 0) throw container_is_empty();
        if (pos.owner != this || pos.ptr == nullptr || pos.ptr == head) throw invalid_iterator();
        node *nxt = pos.ptr->next;
        node *rm = erase(pos.ptr);
        delete rm;
        return iterator(this, nxt);
    }
    /**
     * adds an element to the end
     */
    void push_back(const T &value) { insert(iterator(this, head), value); }
    /**
     * removes the last element
     * throw when the container is empty.
     */
    void pop_back() {
        if (sz == 0) throw container_is_empty();
        erase(iterator(this, head->prev));
    }
    /**
     * inserts an element to the beginning.
     */
    void push_front(const T &value) { insert(iterator(this, head->next), value); }
    /**
     * removes the first element.
     * throw when the container is empty.
     */
    void pop_front() {
        if (sz == 0) throw container_is_empty();
        erase(iterator(this, head->next));
    }
    /**
     * sort the values in ascending order with operator< of T
     */
    void sort() {
        if (sz <= 1) return;
        node **a = new node*[sz];
        size_t idx = 0;
        for (node *cur = head->next; cur != head; cur = cur->next) a[idx++] = cur;
        int *L = new int[sz], *R = new int[sz];
        int top = 0; L[0] = 0; R[0] = (int)sz - 1;
        while (top >= 0) {
            int l = L[top], r = R[top];
            --top;
            int i = l, j = r;
            node *pivot = a[(l + r) >> 1];
            while (i <= j) {
                while (*(a[i]->val) < *(pivot->val)) ++i;
                while (*(pivot->val) < *(a[j]->val)) --j;
                if (i <= j) { node *t = a[i]; a[i] = a[j]; a[j] = t; ++i; --j; }
            }
            if (l < j) { ++top; L[top] = l; R[top] = j; }
            if (i < r) { ++top; L[top] = i; R[top] = r; }
        }
        head->next = a[0]; a[0]->prev = head;
        for (size_t k = 1; k < sz; ++k) { a[k-1]->next = a[k]; a[k]->prev = a[k-1]; }
        a[sz-1]->next = head; head->prev = a[sz-1];
        delete [] L; delete [] R; delete [] a;
    }
    /**
     * merge two sorted lists into one (both in ascending order)
     * compare with operator< of T
     * container other becomes empty after the operation
     * for equivalent elements in the two lists, the elements from *this shall always precede the elements from other
     * the order of equivalent elements of *this and other does not change.
     * no elements are copied or moved
     */
    void merge(list &other) {
        if (this == &other || other.sz == 0) return;
        node *a = head->next, *b = other.head->next;
        node *tail = head;
        // detach current chain
        head->next = head->prev = head;
        size_t new_sz = sz + other.sz;
        while (a != nullptr && b != nullptr && a != head && b != other.head) {
            if (*(b->val) < *(a->val)) {
                node *nb = b->next;
                // splice b
                b->prev = tail; tail->next = b; tail = b; b = nb;
            } else {
                node *na = a->next;
                a->prev = tail; tail->next = a; tail = a; a = na;
            }
        }
        while (a != head) { node *na = a->next; a->prev = tail; tail->next = a; tail = a; a = na; }
        while (b != other.head) { node *nb = b->next; b->prev = tail; tail->next = b; tail = b; b = nb; }
        tail->next = head; head->prev = tail;
        // other becomes empty
        other.head->next = other.head->prev = other.head;
        other.sz = 0;
        sz = new_sz;
    }
    /**
     * reverse the order of the elements
     * no elements are copied or moved
     */
    void reverse() {
        if (sz <= 1) return;
        node *cur = head;
        do {
            node *tmp = cur->next;
            cur->next = cur->prev;
            cur->prev = tmp;
            cur = tmp;
        } while (cur != head);
    }
    /**
     * remove all consecutive duplicate elements from the container
     * only the first element in each group of equal elements is left
     * use operator== of T to compare the elements.
     */
    void unique() {
        if (sz <= 1) return;
        node *cur = head->next;
        while (cur != head && cur->next != head) {
            if (*(cur->val) == *(cur->next->val)) {
                node *dup = cur->next;
                erase(dup);
                delete dup;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
