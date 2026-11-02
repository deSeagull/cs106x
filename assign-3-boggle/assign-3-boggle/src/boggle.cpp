/**
 * File: boggle.cpp
 * ----------------
 * Implements the game of Boggle.
 */

#include <cctype>
#include <iostream>
#include <ctype.h>
#include "console.h"
#include "gboggle.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "random.h"
#include "grid.h"
#include "map.h"
#include "set.h"
#include "lexicon.h"
using namespace std;

static const string kStandardCubes[16] = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

static const string kBigBoggleCubes[25] = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

static const int kMinLength = 4;
static const double kDelayBetweenHighlights = 100;
static const double kDelayAfterAllHighlights = 500;

/**
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */
static void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl;
    cout << endl;
}

/**
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */
static void giveInstructions() {
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl;
    cout << endl;
    cout << "Hit return when you're ready...";
    getLine(); // ignore return value
}

/**
 * Function: getPreferredBoardSize
 * -------------------------------
 * Repeatedly prompts the user until he or she responds with one
 * of the two supported Boggle board dimensions.
 */

static int getPreferredBoardSize() {
    cout << "You can choose standard Boggle (4x4 grid) or Big Boggle (5x5 grid)." << endl;
    return getIntegerBetween("Which dimension would you prefer: 4 or 5?", 4, 5);
}

/**
 * Function: playBoggle
 * --------------------
 * Manages all details needed for the user to play one
 * or more games of Boggle.
 */
static int playBoggle() {
    int dimension = getPreferredBoardSize();
    drawBoard(dimension, dimension);
    cout << "This is where you'd play the game of Boggle" << endl;
    return dimension;
}

void randominitializeBoard(int dimension, Grid<char>& board) {
    string s[25];
    if (dimension == 4) {
        for (int i = 0; i < dimension * dimension; i++) {
            s[i] = kStandardCubes[i];
        }
    } else {
        for (int i = 0; i < dimension * dimension; i++) {
            s[i] = kBigBoggleCubes[i];
        }
    }
    for (int i = 0; i < dimension * dimension; i++) {
        int r = randomInteger(i, dimension * dimension - 1);
        string str = s[i];
        s[i] = s[r];
        s[r] = str;
    }
    int index = 0;
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            char ch = s[index][randomInteger(0,5)];
            board[i][j] = ch;
            labelCube(i, j, ch);
            index++;
        }
    }
}

bool isAllLetters(const string& s) {
    for (char c: s) {
        if(!isalpha(c)) {
            return false;
        }
    }
    return true;
}

void playerinitializeBoard(int dimension, Grid<char>& board) {
    cout << "Enter a "<< dimension * dimension << " -character string to identify which letters you want on the cubes."
            "The first " << dimension << " characters form the top row, the next "<< dimension
         << " characters form the second row, and so forth." << endl << "enter a string: ";
    string s = getLine();
    while (true) {
        if (s.size() != dimension * dimension) {
            cout << "Enter a string that's precisely " << dimension * dimension << " characters long." << endl;
            s = getLine("Try again: ");
        } else if (!isAllLetters(s)) {
            cout << "Enter a string with only alphabetic letters." << endl;
            s = getLine("Try again: ");
        } else {
            int k = 0;
            for (int i = 0; i < dimension; i++) {
                for (int j = 0; j < dimension; j++) {
                    board[i][j] = s[k];
                    labelCube(i, j, s[k]);
                    k++;
                }
            }
            return;
        }
    }
}

void computeAllWordsHelper(const Grid<char>& board, Grid<bool>& board0, string s, int myrow, int mycol,
                           Lexicon& lex, Vector<int>& v, Map<string, Vector<int>>& map1) {
    if (s.size() >= kMinLength && lex.contains(s) && !map1.containsKey(s)) {
        map1[s] = v;
    }
    int dx[] = {-1, 0, 1, 1, 1, 0, -1, -1};
    int dy[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    for (int i = 0; i < 8; i++) {
        int nextcol = mycol + dx[i];
        int nextrow = myrow + dy[i];
        if(board.inBounds(nextrow, nextcol) && board0[nextrow][nextcol] == false){
            char ch = tolower(board[nextrow][nextcol]);
            string nextword = s + ch;
            if (lex.containsPrefix(nextword)){
                board0[nextrow][nextcol] = true;
                v.add(nextrow);
                v.add(nextcol);
                computeAllWordsHelper(board, board0, nextword, nextrow, nextcol, lex, v, map1);
                board0[nextrow][nextcol] = false;
                v.remove(v.size() - 1);
                v.remove(v.size() - 1);
            }
        }
    }
}

Map<string, Vector<int>> computeAllWords(const Grid<char>& board, Lexicon& lex) {
    int n = board.numCols();
    Map<string, Vector<int>> map1;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            string s = charToString(tolower(board[i][j]));
            Grid<bool> board0(n, n);
            Vector<int> v;
            v.add(i);
            v.add(j);
            board0[i][j] = true;
            computeAllWordsHelper(board, board0, s, i, j, lex, v, map1);
        }
    }
    return map1;
}

void humanPutWord(Map<string, Vector<int>>& map1, Lexicon& lex) {
    Set<string> set0;
    while (true) {
        string s = getLine("Enter a word: ");
        s = toLowerCase(s);
        if (s == ""){
            return;
        } else if (s.size() < kMinLength) {
            cout << "Sorry, that isn't long enough to even be considered." << endl;
        } else if (!lex.contains(s)) {
            cout << "Sorry, that isn't even a word." << endl;
        } else if (!map1.containsKey(s)) {
            cout << "That word can't be constructed with this board." << endl;
        } else if (set0.contains(s)) {
            cout << "You've already guessed that word." << endl;
        } else {
            Player hu = HUMAN;
            recordWordForPlayer(s, hu);
            Vector<int> v = map1[s];
            int len = s.size();
            for (int i = 0; i < len; i++) {
                highlightCube(v[2 * i], v[2 * i + 1], true);
                pause(kDelayBetweenHighlights);
            }
            for (int i = len - 1; i >= 0; i--) {
                highlightCube(v[2 * i], v[2 * i + 1], false);
                pause(kDelayBetweenHighlights);
            }
            pause(kDelayAfterAllHighlights);
            set0.add(s);
            map1.remove(s);
        }
    }
}
void computerPutWord(Map<string, Vector<int>>& map1){
    Player co = COMPUTER;
    for(string s: map1){
        recordWordForPlayer(s, co);
    }
}

/**
 * Function: main
 * --------------
 * Serves as the entry point to the entire program.
 */
int main() {
    GWindow gw(kBoggleWindowWidth, kBoggleWindowHeight);
    initGBoggle(gw);
    welcome();
    if (getYesOrNo("Do you need instructions?")) giveInstructions();
    do {
        int dimension = playBoggle();
        Grid<char> board(dimension, dimension);
        Lexicon lex("res/dictionary.txt");
        if(getYesOrNo("I'll give you a chance to set up the board to your specification,"
                       "which makes it easier to confirm your boggle program is working."
                       "Do you want to force the board configuration?")){
            playerinitializeBoard(dimension, board);
        } else {
            randominitializeBoard(dimension, board);
        }
        Map<string, Vector<int>> map1 = computeAllWords(board, lex);
        humanPutWord(map1, lex);
        computerPutWord(map1);
    } while (getYesOrNo("Would you like to play again?"));
    cout << "Thank you for playing!" << endl;
    shutdownGBoggle();
    return 0;
}
