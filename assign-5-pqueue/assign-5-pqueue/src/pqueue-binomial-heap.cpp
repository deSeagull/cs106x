#include "pqueue-binomial-heap.h"
using namespace std;

BinomialHeapPQueue::BinomialHeapPQueue() {}
BinomialHeapPQueue::~BinomialHeapPQueue() {
    for (int i = 0; i < v.size(); i++) {
        node* curr1 = v[i];
        deletetree(curr1);
    }
}

void BinomialHeapPQueue::deletetree(node* t) {
    if (t == nullptr) {
        return;
    } else {
        for (node* curr : t->children) {
            deletetree(curr);
        }
    }
    delete t;
    logSize--;
}

const string& BinomialHeapPQueue::peek() const {
    if (v.isEmpty()) {
        throw "null";
    } else {
        string s;
        bool found = false;
        for (int i = 0; i < v.size(); i++) {
            if (v[i]) {
                if (!found || v[i]->elem < s) {
                    s = v[i]->elem;
                    found = true;
                }
            }
        }
        return s;
    }
}

string BinomialHeapPQueue::extractMin() {
    if (v.isEmpty()) {
        throw "null";
    } else {
        string s;
        int index = 0;
        bool found = false;
        for (int i = 0; i < v.size(); i++) {
            if (v[i]) {
                if (!found || v[i]->elem < s) {
                    s = v[i]->elem;
                    found = true;
                    index = i;
                }
            }
        } 
        BinomialHeapPQueue two;
        node* minnode = v[index];
        for (int i = 0; i < index; i++) {
            two.v.add(v[index]->children[i]);
        }
        v[index] = nullptr;
        delete minnode;
        merge(this, &two);
        for (int i = 0; i < two.v.size(); i++) {
            two.v[i] = nullptr;
        }
        logSize--;
        return s;
    }
}
BinomialHeapPQueue::node* BinomialHeapPQueue::mixnode(node* a1, node* a2) {
    if (a1->elem < a2->elem) {
        a1->children.add(a2);
        return a1;
    } else {
        a2->children.add(a1);
        return a2;
    }
}

void BinomialHeapPQueue::enqueue(const string& s) {
    node* node1 = new node();
    node1->elem = s;
    BinomialHeapPQueue two;
    two.v.add(node1);
    merge(this, &two);
    two.v[0] = nullptr;
    logSize++;
}

BinomialHeapPQueue *BinomialHeapPQueue::merge(BinomialHeapPQueue * one, BinomialHeapPQueue * two) {
    node* curr = nullptr;
    int maxlen = max(one->v.size(), two->v.size());
    for (int i = 0; i < maxlen || curr; i++) {
        if (i == one->v.size()) {
            one->v.add(nullptr);
        }

        node* a1 = one->v[i];
        node* a2 = (i < two->v.size()) ? two->v[i] : nullptr;

        if (a1 == nullptr && a2 == nullptr && curr == nullptr){
        } else if (a1 == nullptr && a2 == nullptr && curr) {
            one->v[i] = curr;
            curr = nullptr;
        } else if (a1 == nullptr && a2 && curr == nullptr) {
            one->v[i] = two->v[i];
        } else if (a1 == nullptr && a2 && curr) {
            curr = mixnode(a2, curr);
        } else if (a1 && a2 == nullptr && curr == nullptr) {
        } else if (a1 && a2 == nullptr && curr) {
            curr = mixnode(a1, curr);
            one->v[i] = nullptr;
        } else if (a1 && a2 && curr == nullptr) {
            curr = mixnode(a1, a2);
            one->v[i] = nullptr;
        } else if (a1 && a2 && curr) {
            node* tempt = mixnode(a1, a2);
            one->v[i] = curr;
            curr = tempt;
        }  
    }
    for(int j=0; j<two->v.size(); j++) two->v[j] = nullptr;
    one->logSize += two->logSize;
    two->logSize = 0;
    return one;
}
