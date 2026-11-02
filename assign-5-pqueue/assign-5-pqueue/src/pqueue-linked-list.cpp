#include "pqueue-linked-list.h"
using namespace std;

LinkedListPQueue::LinkedListPQueue() {
    front = nullptr;
}
LinkedListPQueue::~LinkedListPQueue() {
    while (front) {
        Node* node = front;
        front = front->next;
        delete node;
        logSize--;
    }
}

const string& LinkedListPQueue::peek() const {
    if (!front) {
        throw "null";
    } else {
        return front->s;
    }
}

string LinkedListPQueue::extractMin() {
    if (!front) {
        throw "null";
    } else {
        Node* curr = front;
        front = front->next;
        if (front) {
            front->prev = nullptr;
        }
        string s1 = curr->s;
        delete curr;
        logSize--;
        return s1;
    }
}

void LinkedListPQueue::enqueue(const string& s1) {
    Node* curr = front;
    Node* pr = nullptr;
    Node* node = new Node();
    node->s = s1;
    node->next = nullptr;
    node->prev = nullptr;
    while (curr && s1 > curr->s) {
        pr = curr;
        curr = curr->next;
    }
    if (pr == nullptr) {
        node->next = front;
        node->prev = nullptr;
        if (front) {
            front->prev = node;
        }
        front = node;
    }else{
        pr->next = node;
        node->prev = pr;
        node->next = curr;
        if (curr != nullptr) {
            curr->prev = node;
        }
    }
    logSize++;
}

LinkedListPQueue *LinkedListPQueue::merge(LinkedListPQueue * one, LinkedListPQueue * two) {
    Node* curr1 = one->front;
    Node* pr1 = nullptr;
    while (two->front) {
        Node* curr2 = two->front;
        while (curr1 && curr2->s > curr1->s) {
            pr1 = curr1;
            curr1 = curr1->next;
        }
        two->front = two->front->next;
        if (!pr1) {
            curr2->next = one->front;
            curr2->prev = nullptr;
            if (one->front) {
                one->front->prev = curr2;
            }
            one->front = curr2;
        } else {
            curr2->next = curr1;
            curr2->prev = pr1;
            pr1->next = curr2;
            if (curr1) {
                curr1->prev = curr2;
            }
        }
        pr1 = curr2;
        two->logSize--;
        one->logSize++;
    }
    return one;
}

