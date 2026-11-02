#pragma once
#include "pqueue.h"
#include <string>
#include "vector.h"

class BinomialHeapPQueue : public PQueue {
public:
	BinomialHeapPQueue();
	~BinomialHeapPQueue();
	
	static BinomialHeapPQueue *merge(BinomialHeapPQueue *one, BinomialHeapPQueue *two);
	
	void enqueue(const std::string& elem);
    std::string extractMin();
    const std::string& peek() const;
	
private:
    struct node {
        std::string elem;
        Vector<node *> children;
    };
    Vector<node *> v;
    static node* mixnode(node* a1, node* a2);
    void deletetree(node* t);
};

