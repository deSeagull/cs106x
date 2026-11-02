#include "pqueue-vector.h"
using namespace std;

VectorPQueue::VectorPQueue() {}
VectorPQueue::~VectorPQueue() {}

const string& VectorPQueue::peek() const {
    if (v.isEmpty()) {
        throw "null";
    }
    else {
        return v[0];
    }
}

string VectorPQueue::extractMin() {
    if (v.isEmpty()) {
        throw "null";
    } else {
        string min = v[0];
        int index = 0;
        for (int i = 0; i < logSize; i++) {
            if (v[i] < min) {
                min = v[i];
                index = i;
            }
        }
        v.remove(index);
        logSize--;
        return min;
    }
}

void VectorPQueue::enqueue(const string& s) {
    v.add(s);
    logSize++;
}

VectorPQueue *VectorPQueue::merge(VectorPQueue * one, VectorPQueue * two) {
    for (int i = 0; i < two->logSize; i++) {
        one->enqueue(two->v[i]);
    }
    return one;
}
