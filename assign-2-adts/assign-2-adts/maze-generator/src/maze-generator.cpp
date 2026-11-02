/**
 * File: maze-generator.cpp
 * ------------------------
 * Presents an adaptation of Kruskal's algorithm to generate mazes.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "simpio.h"
#include "set.h"
#include "vector.h"
#include "maze-graphics.h"
#include "maze-types.h"
#include "random.h"

static int getMazeDimension(string prompt,
                            int minDimension = 7, int maxDimension = 50) {
    while (true) {
        int response = getInteger(prompt);
        if (response == 0) return response;
        if (response >= minDimension && response <= maxDimension) return response;
        cout << "Please enter a number between "
             << minDimension << " and "
             << maxDimension << ", inclusive." << endl;
    }
}

void generateWalls(int dimension, Vector<struct wall>& walls2){
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension - 1; j++){
            struct cell one = {i, j};
            struct cell two = {i, j + 1};
            struct wall a = {one, two};
            walls2.add(a);
        }
    }
    for (int i = 0; i < dimension - 1; i++) {
        for (int j = 0; j < dimension; j++){
            struct cell one = {i, j};
            struct cell two = {i + 1, j};
            struct wall a = {one, two};
            walls2.add(a);
        }
    }
}

void generateChambers(int dimension, Vector<Set<struct cell>>& chambers){
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++){
            struct cell one = {i, j};
            Set<struct cell> chamber;
            chamber.add(one);
            chambers.add(chamber);
        }
    }
}

void drawWalls(MazeGeneratorView& maze, int dimension, Vector<struct wall>& walls2){
    maze.setDimension(dimension);
    maze.addAllWalls(walls2);
    repaint();
}

struct wall randomChooseWall(Vector<struct wall>& walls2){
    int len = walls2.size();
    int index = randomInteger(0, len - 1);
    struct wall one = walls2[index];
    struct wall last = walls2[len - 1];
    walls2[index] = last;
    walls2.remove(len - 1);
    return one;
}

bool judgeWall(struct wall one, Vector<Set<struct cell>>& chambers){
    struct cell a = one.one;
    struct cell b = one.two;
    int len = chambers.size();
    for(int i = 0; i < len; i++){
        if(chambers[i].contains(a) && chambers[i].contains(b)) {
            return false;
        }
    }
    return true;
}

void reMoveWall(struct wall one, Vector<Set<struct cell>>& chambers){
    struct cell a = one.one;
    struct cell b = one.two;
    int len = chambers.size();
    Set<struct cell> chamber1;
    for(int i = 0; i < len; i++){
        if(chambers[i].contains(b) && !chambers[i].contains(a)){
            chamber1 = chambers[i];
            chambers.remove(i);
            len--;
        }
    }
    for(int i = 0; i < len; i++){
        if(chambers[i].contains(a)) {
            chambers[i] +=chamber1;
        }
    }
}

int main() {
    MazeGeneratorView maze;
    while (true) {
        int dimension = getMazeDimension("What should the dimension of your maze be [0 to exit]? ");
        if (dimension == 0) break;
        cout << "This is where I'd animate the construction of a maze of dimension " << dimension << "." << endl;
        Vector<struct wall> walls2;
        Vector<Set<struct cell>> chambers;
        generateWalls(dimension, walls2);
        generateChambers(dimension, chambers);
        drawWalls(maze, dimension, walls2);
        while(true){
            struct wall one = randomChooseWall(walls2);
            if(judgeWall(one, chambers)){
                reMoveWall(one, chambers);
                maze.removeWall(one);
                maze.repaint();
            }
            if(chambers.size() == 1){
                break;
            }
        }
    }
    return 0;
}
