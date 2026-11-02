/**
 * File: ssmodel.cpp
 * ------------------
 * This file will contain the implementation of the SSModel class (once you
 * have finished it!
 */
 
#include "ssmodel.h"
#include "exp.h"
#include "parser.h"
using namespace std;

SSModel::SSModel(int nRows, int nCols, SSView *view) {
    row = nRows;
    col = nCols + 'A' - 1;
    this->view = view;
    view->displayEmptySpreadsheet();
}

SSModel::~SSModel() {
    for (string s : index) {
        node* one= index[s];
        delete one->expre;
        delete one;
    }
    index.clear();
}

bool SSModel::nameIsValid(const string& cellname) const {
    location loc;
    bool flag1 = stringToLocation(cellname, loc);
    if (loc.col > col) {
        return false;
    }
    if (loc.row > row || loc.row < 1) {
        return false;
    }
    return flag1;
}

bool SSModel::isAcircular(string startname, string currname) {
    if (currname == startname) {
        return true;
    }
    if (index.containsKey(currname)) {
        for (string s : index[currname]->upStream) {
            isAcircular(startname, s);
        }
    }
    return false;
}

void SSModel::getAlldownStream(string cellname, Set<string>& alldownStream) {
    alldownStream += index[cellname]->downStream;
    for (string s : index[cellname]->downStream) {
        if (index.containsKey(s)) {
            getAlldownStream(s, alldownStream);
        }
    }
}

void SSModel::computeIndegree(Map<std::string, int>& indegreeMap, Set<std::string>& alldownStream) {
    for (string s1 : alldownStream) {
        int indegree = 0;
        for (string s2 : alldownStream) {
            if (index[s2]->downStream.contains(s1)) {
                indegree++;
            }
        }
        indegreeMap[s1] = indegree;
    }
}

void SSModel::setCellFromScanner(const string& cellname, TokenScanner& scanner) {
    node* one = new node();
    one->expre = parseExp(scanner, *this);
    one->upStream = one->expre->getDependencies();
    for (string currname : one->upStream) {
        if (isAcircular(cellname, currname)) {
            delete one->expre;
            delete one;
            error("is a circular reference");
        }
    }
    one->name = cellname;
    one->value = one->expre->eval(*this);
    if (index.containsKey(cellname)) {
        one->downStream = index[cellname]->downStream;
        delete index[cellname]->expre;
        delete index[cellname];
    }
    index[cellname] = one;
    for (string s : one->upStream) {
        if (index.containsKey(s)) {
            index[s]->downStream.add(cellname);
        } else {
            node* two = new node();
            two->downStream.add(cellname);
            index[s] = two;
        }
    }
    Set<string> alldownStream;
    Map<string, int> indegreeMap;
    alldownStream.add(cellname);
    getAlldownStream(cellname, alldownStream);
    computeIndegree(indegreeMap, alldownStream);
    Queue<string> q;
    q.enqueue(cellname);
    while(!q.isEmpty()) {
        string s = q.dequeue();
        index[s]->value = index[s]->expre->eval(*this);
        string txt;
        if (index[s]->expre->getType() == TEXTSTRING) {
            txt = index[s]->expre->toString();
        } else {
            txt = realToString(index[s]->value);
        }
        view->displayCell(s, txt);
        for (string s : index[s]->downStream) {
            indegreeMap[s]--;
            if (indegreeMap[s] == 0) {
                q.enqueue(s);
            }
        }
    }
}

void SSModel::printCellInformation(const string& cellname) const {
    if (index.containsKey(cellname)) {
        if (index[cellname]->expre) {
            node* one = index[cellname];
            cout << cellname << " = " << one->expre->toString() << endl;
            cout << "Cells that C2 directly depends on:";
            for (string s : one->upStream) {
                cout << " " << s;
            }
            cout << endl;
            cout << "Cells that directly depend on " << cellname << ":";
            for (string s : one->downStream) {
                cout << " " << s;
            }
            cout << endl;
        } else {
            cout << "The cell is empty." << endl;
        }
    } else {
        cout << "The cell is empty." << endl;
    }
}

void SSModel::writeToStream(ostream& outfile) const {
    for (string s : index) {
        outfile << s << "=" << index[s]->expre->toString() << "\n";
    }
}

void SSModel::readFromStream(istream& infile) {
    clearMap();
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.scanStrings();
    string s;
    while(getline(infile, s)) {
        scanner.setInput(s);
        string cellname = toUpperCase(scanner.nextToken());
        if (!nameIsValid(cellname))
            error("Invalid cell name " + cellname);
        if (scanner.nextToken() != "=")
            error("= expected.");
        setCellFromScanner(cellname, scanner);
    }
}

Expression* SSModel::getCellExpression(std::string name) {
    return index[name]->expre;
}

double SSModel::getCellValue(std::string name) {
    string s = toUpperCase(name);
    if(index.containsKey(s)) {
        return index[s]->value;
    } else {
        return 0.0;
    }
}

void SSModel::clearMap() {
    for (string s : index) {
        node* one= index[s];
        delete one->expre;
        delete one;
    }
    index.clear();
    view->displayEmptySpreadsheet();
}
