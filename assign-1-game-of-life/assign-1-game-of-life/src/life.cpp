/**
 * File: life.cpp
 * --------------
 * Implements the Game of Life.
 */

#include <fstream>
#include <iostream>  // for cout
using namespace std;

#include "console.h" // required of all files that contain the main function
#include "simpio.h"  // for getLine
#include "gevent.h" // for mouse event detection
#include "strlib.h"
#include "gtimer.h"

#include "life-constants.h"  // for kMaxAge
#include "life-graphics.h"   // for class LifeDisplay

/**
 * Function: welcome
 * -----------------
 * Introduces the user to the Game of Life and its rules.
 */
static void welcome() {
    cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
    cout << "Cells live and die by the following rules:" << endl << endl;
    cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
    cout << "\tLocations with 2 neighbors remain stable" << endl;
    cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
    cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl << endl;
    cout << "In the animation, new cells are dark and fade to gray as they age." << endl << endl;
    getLine("Hit [enter] to continue....   ");
}

void initilizeSetting(int& numRows,int& numColumns,Grid<int>& ages){
    cout << "You can start your colony with random cells or read from a prepared file."<<endl
         <<"Enter name of colony file (or RETURN to seed randomly): ";
    ifstream file;
    string s;
    getline(cin, s);
    if(s.empty()){
        numRows = randomInteger(40,60);
        numColumns = randomInteger(40,60);
        ages.resize(numRows,numColumns);
        for(int i = 0; i < numRows; i++){
            for(int j = 0; j < numColumns; j++){
                int boo = randomInteger(0,1);
                if(boo == 1){
                ages[i][j] = randomInteger(1, kMaxAge);
                }else{
                    ages[i][j] = 0;
                }
            }
        }
    }else{
        file.open(s);
        if(!file.is_open()){
            cout << "File open failed" <<endl;
            return initilizeSetting(numRows, numColumns, ages);
        }
        int flag = 0;
        char ch;
        int i = 0;
        string line;
        while(getline(file, line)){
            istringstream ss(line);
            if(flag == 0 && ss >> numRows){
                flag = 1;
                continue;
            }
            if(flag == 1){
                ss >> numColumns;
                flag++;
                ages.resize(numRows,numColumns);
                continue;
            }
            if(flag > 1){
                int j = 0;
                while(ss >> ch){
                    if(ch == 'X'){
                        ages[i][j] = 1;
                    }else{
                        ages[i][j] = 0;
                    }
                    j++;
                }
                i++;
            }               
        }
    }
}

void advance(Grid<int>& futher, Grid<int>& ages){
    int dx[8] = {-1,0,1,1,1,0,-1,-1};
    int dy[8] = {-1,-1,-1,0,1,1,1,0};
    int row = ages.numRows();
    int col = ages.numCols();
    futher = ages;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            int count = 0;
            for(int k = 0; k < 8; k++){
                int r = i + dy[k];
                int c = j + dx[k];
                if(r >= 0 && r < row && c >= 0 && c < col && ages[r][c] > 0){
                count++;
                }
            }
            if(count == 1 || count == 0 || count > 3){
                futher[i][j] = 0;
            }
            if(count == 2 && ages[i][j] > 0){
                futher[i][j]++;
            }
            if(count == 3){
                futher[i][j]++;
            }
        }
    }
}

void paint(LifeDisplay& display,Grid<int>& futher){
    int r = futher.numRows();
    int c = futher.numCols();
    display.setDimensions(r, c);
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            display.drawCellAt(i, j, futher[i][j]);
        }
    }
    display.repaint();
}

bool stable(Grid<int>& futher,Grid<int>& ages){
    int r = futher.numRows();
    int c = futher.numCols();
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            if(ages[i][j] > 0){
                if(ages[i][j] < kMaxAge){
                    return false;
                }
                if(ages[i][j] != futher[i][j] - 1){
                    return false;
                }
            } 
        }
    }
    return true;
}

void runAnimation(LifeDisplay& display, Grid<int>& ages, Grid<int>& futher, int ms) {
    GTimer timer(ms);
    timer.start();
    while (true) {
        GEvent event = waitForEvent(TIMER_EVENT + MOUSE_EVENT);
        if (event.getEventClass() == TIMER_EVENT) {
            advance(futher, ages);
            paint(display, futher);
            if(stable(futher, ages)){
                break;
            }
        ages = futher;
        }else if(event.getEventType() == MOUSE_PRESSED) {
            break;
        }
    }
    timer.stop();
}
void manulMode(LifeDisplay& display, Grid<int>& ages, Grid<int>& futher){
    while(true){
        string s = getLine("Please return to advance [or type out 'quit' to end]: ");
        advance(futher, ages);
        paint(display, futher);
        ages = futher;
        if(s == "quit"){
            break;
        }
    }
}

int getModeNum(){
    cout <<"You choose how fast to run the simulation."<<endl
    <<      "1 = As fast as this chip can go!"<<endl
    <<      "2 = Not too fast, this is a school zone."<<endl
    <<      "3 = Nice and slow so I can watch everything that happens."<<endl
    <<      "4 = Require enter key be pressed before advancing to next generation."<<endl
    <<"Your choice: ";
    while(true){
        int num = getInteger();
        if(num > 0 && num < 5){
            return num;
        }else{
            cout << "Illegal integer format. Try again." << endl
                 << "Enter an integer: ";
        }
    }

}

bool reset(){
    cout <<"Would you like to run another?";
    while(true){
        string s = getLine();
        if(s == "yes"){
            return true;
        }else if(s == "no"){
            return false;
        }else{
            cout << "Please enter yes or no";
        }
    }
}

/**
 * Function: main
 * --------------
 * Provides the entry point of the entire program.
 */
int main() {
    LifeDisplay display;
    display.setTitle("Game of Life");
    welcome();
    do{
        int numRows = 0;
        int numColumns = 0;
        Grid<int> ages;
        Grid<int> futher;
        initilizeSetting(numRows, numColumns, ages);
        paint(display, ages);
        switch(getModeNum()){
        case 1:
            runAnimation(display, ages, futher, 10);
            break;
        case 2:
            runAnimation(display, ages, futher, 50);
            break;
        case 3:
            runAnimation(display, ages, futher, 200);
            break;
        case 4:
            manulMode(display, ages, futher);
            break;
        }
    }while(reset());
    return 0;
}
