/**
 * File: random-sentence-generator.cpp
 * -----------------------------------
 * Presents a short program capable of reading in
 * context-free grammar files and generating arbitrary
 * sentences from them.
 */

#include <iostream>
#include <fstream>
using namespace std;

#include "console.h"
#include "simpio.h"   // for getLine
#include "strlib.h"   // for toLowerCase, trim

static const string kGrammarsDirectory = "res/grammars/";
static const string kGrammarFileExtension = ".g";
static string getNormalizedFilename(string filename) {
    string normalizedFileName = kGrammarsDirectory + filename;
    if (!endsWith(normalizedFileName, kGrammarFileExtension))
        normalizedFileName += kGrammarFileExtension;
    return normalizedFileName;
}

static bool isValidGrammarFilename(string filename) {
    string normalizedFileName = getNormalizedFilename(filename);
    ifstream infile(normalizedFileName.c_str());
    return !infile.fail();
}

static string getFileName() {
    while (true) {
        string filename = trim(getLine("Name of grammar file? [<return> to quit]: "));
        if (filename.empty() || isValidGrammarFilename(filename)) return getNormalizedFilename(filename);
        cout << "Failed to open the grammar file named \"" << filename << "\". Please try again...." << endl;
    }
}

void getFileData(string filename, Vector<Vector<string>>& v){
    ifstream file(filename);
    string s;
    while (getline(file, s)) {
        Vector<string> v0;
        v0.add(s);
        getline(file, s);
        int row = stringToInteger(s);
        for (int i = 0; i < row; i++) {
            getline(file,s);
            v0.add(s);
        }
        v.add(v0);
        getline(file, s);
    }
}

void initialBuild(Vector<Vector<string>>& v,string& s){
    int len = v[0].size();
    int index = randomInteger(1, len - 1);
    s = v[0][index];
}

string findFirstNonterminal(string s, int& startindex, int& endindex){
    int len = s.size();
    for (int i = 0; i < len; i++) {
        if(s[i] == '<'){
            startindex = i;
        }
        if(s[i] == '>'){
            endindex = i;
            return s.substr(startindex, endindex - startindex + 1);
        }
    }
    return "";
}

string randomChoose(Vector<Vector<string>>& v, string str){
    int len = v.size();
    int index;
    for(int i = 0; i < len; i++){
        if(v[i][0] == str){
            index = i;
            break;
        }
    }
    int num = randomInteger(1, v[index].size() - 1);
    return v[index][num];
}

int main() {
    while (true) {
        string filename = getFileName();
        if (filename.empty()) break;
        cout << "Here's where you read in the \"" << filename << "\" grammar "
             << "and generate three random sentences." << endl;
        Vector<Vector<string>> v;
        string s;
        int startindex = 0;
        int endindex = 0;
        getFileData(filename, v);
        for(int i = 0; i < 3; i++){
            initialBuild(v, s);
            while(findFirstNonterminal(s, startindex, endindex) != ""){
                string str0 = findFirstNonterminal(s, startindex, endindex);
                string str1 = randomChoose(v, str0);
                s.replace(startindex, endindex - startindex + 1, str1);
            }
            cout << s << endl << endl;
        }
    }
    cout << "Thanks for playing!" << endl;
    return 0;
}
