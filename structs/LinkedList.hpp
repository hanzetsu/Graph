#pragma once
#include "../libs/Exceptions.hpp"
#include <cstddef>

template<typename T>
class Node {
    template<class U> friend class LinkedList;
    T data;
    Node* prev;
    Node* next;
    Node(const T& v) : data(v), prev(nullptr), next(nullptr) {}
};

template<typename T>
class LinkedList {
    Node<T>* head;
    Node<T>* tail;
public:
    class Iterator {
        Node<T>* ptr;
    public:
        Iterator(Node<T>* p = nullptr) : ptr(p) {}
        T& operator*() { return ptr->data; }
        const T& operator*() const { return ptr->data; }
        Iterator& operator++() { ptr = ptr->next; return *this; }
        bool operator==(const Iterator& o) const { return ptr == o.ptr; }
        bool operator!=(const Iterator& o) const { return ptr != o.ptr; }
    };

    LinkedList() : head(nullptr), tail(nullptr) {}
    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;
    ~LinkedList() {
        while (head) { Node<T>* n = head->next; delete head; head = n; }
    }

    std::size_t GetLength() const {
        std::size_t n = 0;
        for (Node<T>* c = head; c; c = c->next) ++n;
        return n;
    }
    void Append(T item) {
        Node<T>* nn = new Node<T>(item);
        if (!head) head = tail = nn;
        else { tail->next = nn; nn->prev = tail; tail = nn; }
    }
    T Get(std::size_t index) const {
        std::size_t n = GetLength();
        if (index >= n) throw IndexOutOfRange(index, n, "LinkedList::Get");
        Node<T>* c = head;
        for (std::size_t i = 0; i < index; ++i) c = c->next;
        return c->data;
    }
    T& GetRef(std::size_t index) {
        std::size_t n = GetLength();
        if (index >= n) throw IndexOutOfRange(index, n, "LinkedList::GetRef");
        Node<T>* c = head;
        for (std::size_t i = 0; i < index; ++i) c = c->next;
        return c->data;
    }
    void Set(std::size_t index, const T& v) { GetRef(index) = v; }
    Iterator begin() const { return Iterator(head); }
    Iterator end() const { return Iterator(nullptr); }
};