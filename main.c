#include <stdio.h>
#include "maze_gen.c"
#include "ncurses.h"

//print maze with optional message to the ncurses default window
void printMaze(struct maze newMaze, int *potions, char *message){
    //clear the default window
    clear();
    printw("Potions: %d\n", *potions);
    for (int i = 0; i < newMaze.h; i++) {
        for (int j = 0; j < newMaze.w; j++) {
            printw("%c", newMaze.a[i][j]);
        }
        printw("\n");
    }
    //print message if it is not null
    if(message != NULL){
        printw("%s", message);
    }
    //refresh the default window
    refresh();
}

//check if the position is a potion
void checkPotion(int x,int y,char **maze,int *potions){
    if(maze[x][y] == '#'){
        *potions += 1;
    }
}

//find the entry point of the generated maze along the left wall 
void findEntry(int *entryX,int *entryY,int *x,int *y,struct maze newMaze,int *potions){
    char **maze = newMaze.a;
    for (int i = 0; i < newMaze.h; i++) {
        if(maze[i][0] == ' '){
            *x = i;
            *y = 0;
            maze[*x][*y] = '/';
        }
        else if(maze[i][0] == '#'){
            *x = i;
            *y = 0;
            *potions = *potions + 1;
            maze[*x][*y] = '/';
        }
    }
    *entryX = *x;
    *entryY = *y;
}

//find the exit point of the generated maze along the right wall
void findExit(int *x,int *y,struct maze newMaze){
    char **maze = newMaze.a;
    for (int i = 0; i < newMaze.h; i++) {
        if(maze[i][newMaze.w - 1] == ' '){
            *x = i;
            *y = newMaze.w-1;
        }
    }
}

//move the player to the new position
void playerMove(int x, int y,int *playerX,int *playerY,struct maze newMaze,int *potion){
    //get the 2d array from the struct
    char **maze = newMaze.a;
    checkPotion(x,y,maze,potion);
    //set the previous position to a space and the new position to the player
    maze[*playerX][*playerY] = ' ';
    *playerX = x;
    *playerY = y;
    maze[*playerX][*playerY] = '/';
}


int main() {
    int width;
    int height;
    int cellSize;
    int seed;
    int potions = 0;

    int X = 0;
    int Y = 0;

    int entryX =0;
    int entryY = 0;

    int exitX = 0;
    int exitY = 0;

    //get user input
    printf("Width:");
    scanf("%d",&width);
    printf("Height:");
    scanf("%d",&height);
    printf("Cell Size:");
    scanf("%d",&cellSize);
    printf("Random Seed:");
    scanf("%d",&seed);

    //generate the maze
    struct maze newMaze = generate_maze(width, height, cellSize, seed);
    char **maze = newMaze.a;

    //find the entry and exit points
    findEntry(&entryX,&entryY,&X,&Y,newMaze,&potions);
    findExit(&exitX,&exitY,newMaze);

    //start ncurses
    initscr();
    cbreak();
    noecho();
    printMaze(newMaze,&potions,NULL);

    char input = ' ';
    //get user input
    while ((input = getchar())) {
        //check if the user has pressed a key and if so move the player in that direction if allowed
        switch (input) {
            case 'w':
                if(maze[X -1][Y] != 'w') {
                        playerMove(X-1,Y,&X,&Y,newMaze,&potions);
                        printMaze(newMaze,potions,NULL);

                }
                break;
            case 's':
                if(maze[X +1][Y] != 'w') {
                    playerMove(X+1,Y,&X,&Y,newMaze,&potions);
                    printMaze(newMaze,potions,NULL);
                }
                break;
            case 'a':
                if(maze[X][Y - 1] != 'w') {
                    
                    if(X == entryX && (Y -1) < entryY) {
                        printMaze(newMaze,&potions,"Cannot exit through here!");
                    }
                    else{
                        playerMove(X,Y-1,&X,&Y,newMaze,&potions);
                        printMaze(newMaze,potions,NULL);

                    }
                }
                break;
            case 'd':
                if(maze[X][Y + 1] != 'w'){
                    if((X) == exitX && (Y+1) == exitY){
                        if(potions == 3){
                            playerMove(X,Y+1,&X,&Y,newMaze,&potions);
                            printMaze(newMaze,&potions,"You have escaped!");
                            return 0;
                        }
                        else{
                            printMaze(newMaze,&potions,"You need all 3 potions to escape!");
                        }
                    }
                    else{
                        playerMove(X,Y+1,&X,&Y,newMaze,&potions);
                        printMaze(newMaze,potions,NULL);

                    }
                }
                break;
            default:
                break;
        }
    }
    //end ncurses
    endwin();
}
