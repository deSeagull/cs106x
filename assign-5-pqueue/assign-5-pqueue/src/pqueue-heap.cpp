#include "pqueue-heap.h"
using namespace std;

HeapPQueue::HeapPQueue() {
    a = new string[2];
    capacity = 2;
}
HeapPQueue::~HeapPQueue() {
    delete[] a;
}

const string& HeapPQueue::peek() const {
    if (logSize == 0){
        throw "null";
    } else {
        return a[0];
    }
}

void HeapPQueue::heapify(int index){
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;

        if (left < logSize && a[left] < a[smallest]) {
            smallest = left;
        }
        if (right < logSize && a[right] < a[smallest]) {
            smallest = right;
        }

        if(smallest != index) {
            swap(a[index], a[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

string HeapPQueue::extractMin() {
    if (logSize == 0){
        throw "null";
    } else {
        string tempt = a[0];
        a[0] = a[logSize - 1];
        logSize--;
        if (logSize > 0) {
            heapify(0);
        }
        return tempt;
    }
}

void HeapPQueue::expand(int require) {
    if (capacity >= require) {
        return;
    } else {
        int newca = capacity * 2;
        while (newca < require) {
            newca *= 2;
        }
        string* b = new string[newca]();
        for (int i = 0; i < logSize; i++) {
            b[i] = a[i];
        }
        delete[] a;
        a = b;
        capacity = newca;
    }
}

void HeapPQueue::enqueue(const string& s) {
    expand(logSize + 1);
    a[logSize] = s;
    logSize++;
    int index = logSize - 1;
    while (true){
        if (index == 0) {
            return;
        }
        if (a[index] < a[(index + 1) / 2 - 1]) {
            swap(a[index], a[(index + 1) / 2 - 1]);
            index = (index + 1) / 2 - 1;
        } else {
            return;
        }
    }
}

HeapPQueue *HeapPQueue::merge(HeapPQueue * one, HeapPQueue * two) {
    HeapPQueue* res = new HeapPQueue();
    res->expand(one->logSize + two->logSize);
    for (int i = 0; i < one->logSize; i++) {
        res->a[res->logSize] = one->a[i];
        res->logSize++;
    }
    for (int i = 0; i < two->logSize; i++) {
        res->a[res->logSize] = two->a[i];
        res->logSize++;
    }

    int index = res->logSize / 2 - 1;
    for (int i = index; i >= 0; i--) {
        res->heapify(i);
    }
    return res;
}
