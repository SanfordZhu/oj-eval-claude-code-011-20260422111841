#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T data;
        Node *left;
        Node *right;
        int npl;  // null path length

        Node(const T &d) : data(d), left(nullptr), right(nullptr), npl(0) {}
    };

    Node *root;
    size_t sz;
    Compare cmp;

    int npl(Node *p) const {
        return p ? p->npl : -1;
    }

    Node *merge(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;

        if (cmp(a->data, b->data)) {
            std::swap(a, b);
        }

        a->right = merge(a->right, b);

        if (npl(a->left) < npl(a->right)) {
            std::swap(a->left, a->right);
        }

        a->npl = npl(a->right) + 1;
        return a;
    }

    Node *copyNode(Node *p) {
        if (!p) return nullptr;
        Node *newNode = new Node(p->data);
        newNode->left = copyNode(p->left);
        newNode->right = copyNode(p->right);
        newNode->npl = p->npl;
        return newNode;
    }

    void deleteNode(Node *p) {
        if (!p) return;
        deleteNode(p->left);
        deleteNode(p->right);
        delete p;
    }

public:
    priority_queue() : root(nullptr), sz(0), cmp() {}

    priority_queue(const priority_queue &other) : root(nullptr), sz(other.sz), cmp(other.cmp) {
        root = copyNode(other.root);
    }

    ~priority_queue() {
        deleteNode(root);
    }

    priority_queue &operator=(const priority_queue &other) {
        if (this == &other) return *this;

        Node *newRoot = nullptr;
        try {
            newRoot = copyNode(other.root);
        } catch (...) {
            deleteNode(newRoot);
            throw;
        }

        deleteNode(root);
        root = newRoot;
        sz = other.sz;
        cmp = other.cmp;
        return *this;
    }

    const T &top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->data;
    }

    void push(const T &e) {
        Node *newNode = nullptr;
        try {
            newNode = new Node(e);
        } catch (...) {
            throw;
        }

        Node *newRoot = nullptr;
        try {
            newRoot = merge(root, newNode);
        } catch (...) {
            deleteNode(newNode);
            throw;
        }

        root = newRoot;
        sz++;
    }

    void pop() {
        if (empty()) {
            throw container_is_empty();
        }

        Node *oldRoot = root;
        Node *newRoot = nullptr;

        try {
            newRoot = merge(root->left, root->right);
        } catch (...) {
            throw;
        }

        root = newRoot;
        sz--;
        delete oldRoot;
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return sz == 0;
    }

    void merge(priority_queue &other) {
        if (this == &other) return;

        Node *savedRoot1 = root;
        Node *savedRoot2 = other.root;
        size_t savedSz1 = sz;
        size_t savedSz2 = other.sz;

        try {
            root = merge(root, other.root);
            sz += other.sz;
            other.root = nullptr;
            other.sz = 0;
        } catch (...) {
            root = savedRoot1;
            other.root = savedRoot2;
            sz = savedSz1;
            other.sz = savedSz2;
            throw runtime_error();
        }
    }
};

}

#endif
