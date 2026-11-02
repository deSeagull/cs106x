// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own, along with
// comments on every function and on complex code sections.
// TODO: remove this comment header

#include "encoding.h"
#include "priorityqueue.h"
#include "strlib.h"
#include "filelib.h"
using namespace std;

Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    int ob;
    while ((ob = input.get()) != EOF) {
        freqTable[ob]++;
    }
    freqTable[PSEUDO_EOF] = 1;
    return freqTable;
}

HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    if (freqTable.isEmpty()) {
        return nullptr;
    }
    PriorityQueue<HuffmanNode*> pq;
    for (int ch : freqTable) {
        HuffmanNode* node = new HuffmanNode();
        node->character = ch;
        node->count = freqTable[ch];
        node->zero = nullptr;
        node->one = nullptr;
        pq.enqueue(node, node->count);
    }
    while (pq.size() > 1) {
        HuffmanNode* node1 = pq.dequeue();
        HuffmanNode* node2 = pq.dequeue();
        HuffmanNode* curr = new HuffmanNode();
        curr->count = node1->count + node2->count;
        curr->zero = node1;
        curr->one = node2;
        pq.enqueue(curr, curr->count);
    }
    return pq.dequeue();
}

void buildEncodingMapHelper(HuffmanNode* encodingTree, Map<int, string>& encodingMap, HuffmanNode* node1, string s) {
    if (encodingTree == nullptr) {
        return;
    }
    if (node1->isLeaf()) {
        encodingMap[node1->character] = s;
        return;
    } else {
        HuffmanNode* nodezero = node1->zero;
        buildEncodingMapHelper(encodingTree, encodingMap, nodezero, s + "0");
        HuffmanNode* nodeone = node1->one;
        buildEncodingMapHelper(encodingTree, encodingMap, nodeone, s + "1");
        return;
    }
}

Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    HuffmanNode* node1 = encodingTree;
    string s;
    buildEncodingMapHelper(encodingTree, encodingMap, node1, s);
    return encodingMap;
}

void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    int ch;
    while ((ch = input.get()) != EOF) {
        string s = encodingMap[ch];
        int len = s.length();
        for (int i = 0; i < len; i++) {
            output.writeBit(s[i] - '0');
        }
    }
    string s = encodingMap[PSEUDO_EOF];
    int len = s.length();
    for (int i = 0; i < len; i++) {
        output.writeBit(s[i] - '0');
    }
}

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    HuffmanNode* node =  encodingTree;
    while (true) {
        int i;
        if ((i = input.readBit()) == 0) {
            node = node->zero;
            if (node->isLeaf()) {
                int ch = node->character;
                if (ch != PSEUDO_EOF) {
                    output.put(node->character);
                    node = encodingTree;
                } else {
                    break;
                }
            }
        } else {
            node = node->one;
            if (node->isLeaf()) {
                int ch = node->character;
                if (ch != PSEUDO_EOF) {
                    output.put(node->character);
                    node = encodingTree;
                } else {
                    break;
                }
            }
        }

    }
}

void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);
    output << freqTable;
    rewindStream(input);
    encodeData(input, encodingMap, output);
    freeTree(encodingTree);
}

void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    input >> freqTable;
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    decodeData(input, encodingTree, output);
    freeTree(encodingTree);
}

void freeTree(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    } else {
        HuffmanNode* nodezero = node->zero;
        freeTree(nodezero);
        HuffmanNode* nodeone = node->one;
        freeTree(nodeone);
        delete(node);
    }
}
