/**
 * File: word-ladder.cpp
 * ---------------------
 * Implements a program to find word ladders connecting pairs of words.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "lexicon.h"
#include "strlib.h"
#include "simpio.h"
#include "vector.h"
#include "queue.h"

static string getWord(const Lexicon& english, const string& prompt) {
    while (true) {
        string response = trim(toLowerCase(getLine(prompt)));
        if (response.empty() || english.contains(response)) return response;
        cout << "Your response needs to be an English word, so please try again." << endl;
    }
}

static Vector<string> getstringNearby(const Lexicon& english, const string& word){
    Vector<string> words;
    int len = word.length();
    const int wordnum = 26;
    for(int i = 0; i < len; i++){
        for(int j = 0; j < wordnum; j++){
            string copy = word;
            string near = copy.replace(i, 1, 1, 'a' + j);
            if(near != word){
                if(english.contains(near)){
                    words.add(near);
                }
            }
        }
    }
    return words;
}

static void generateLadder(const Lexicon& english, const string& start, const string& end) {
    Queue<Vector<string>> ladders;
    Vector<string> ladder0;
    ladder0.add(start);
    ladders.enqueue(ladder0);
    Set<string> set;
    set.add(start);
    while(!ladders.isEmpty()){
        Vector<string> ladder = ladders.dequeue();
        if(ladder[ladder.size() - 1] == end) {
            cout << "ladder: ";
            for(int i = 0; i < ladder.size(); i++){
                cout << ladder[i] << " ";
            }
            cout <<endl;
            return;
        } else {
            Vector<string> words = getstringNearby(english, ladder[ladder.size() - 1]);
            for(int i = 0; i < words.size(); i++){
                if(!set.contains(words[i])){
                    Vector<string> lad = ladder;
                    lad.add(words[i]);
                    ladders.enqueue(lad);
                    set.add(words[i]);
                }
            }
        }
    }
    cout << "can't find the ladder" <<endl;
}

static const string kEnglishLanguageDatafile = "res/dictionary.txt";
static void playWordLadder() {
    Lexicon english(kEnglishLanguageDatafile);
    while (true) {
        string start = getWord(english, "Please enter the source word [return to quit]: ");
        if (start.empty()) break;
        string end = getWord(english, "Please enter the destination word [return to quit]: ");
        if (end.empty()) break;
        generateLadder(english, start, end);
    }
}

int main() {
    cout << "Welcome to the CS106 word ladder application!" << endl << endl;
    playWordLadder();
    cout << "Thanks for playing!" << endl;
    return 0;
}
