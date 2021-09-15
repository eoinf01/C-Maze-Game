#include <stdio.h>
#include "maze_gen.c"
void printMaze(struct maze newMaze){
    printf("\n");
    for (int i = 0; i < newMaze.h; i++) {
        for (int j = 0; j < newMaze.w; j++) {
            printf("%c", newMaze.a[i][j]);
        }
        printf("\n");
    }
}

void checkPotion(int x,int y,char **maze,int *potions){
    if(maze[x][y] == '#'){
        *potions += 1;
    }
}

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
void findExit(int *x,int *y,struct maze newMaze){
    char **maze = newMaze.a;
    for (int i = 0; i < newMaze.h; i++) {
        if(maze[i][newMaze.w - 1] == ' '){
            *x = i;
            *y = newMaze.w-1;
        }
    }
}

void playerMove(int x, int y,int *playerX,int *playerY,struct maze newMaze,int *potion){
    char **maze = newMaze.a;
    checkPotion(x,y,maze,potion);
    maze[*playerX][*playerY] = ' ';
    *playerX = x;
    *playerY = y;
    maze[*playerX][*playerY] = '/';
    printMaze(newMaze);
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

    char exit = 0;

    printf("Width:");
    scanf("%d",&height);
    printf("Height:");
    scanf("%d",&width);
    printf("Cell Size:");
    scanf("%d",&cellSize);
    printf("Random Seed:");
    scanf("%d",&seed);

    struct maze newMaze = generate_maze(width, height, cellSize, seed);
    char **maze = newMaze.a;
    findEntry(&entryX,&entryY,&X,&Y,newMaze,&potions);

    findExit(&exitX,&exitY,newMaze);

    printMaze(newMaze);

    char input = 0;
    while ((input = 0  != '\n') && (exit != '\n')) {
        input = getchar();
        switch (input) {
            case 'w':
                if(maze[X -1][Y] != 'w') {
                        playerMove(X-1,Y,&X,&Y,newMaze,&potions);
                }
                break;
            case 's':
                if(maze[X +1][Y] != 'w') {
                    playerMove(X+1,Y,&X,&Y,newMaze,&potions);
                }
                break;
            case 'a':
                if(maze[X][Y - 1] != 'w') {
                    if(X == entryX && (Y -1) == entryY) {
                        printMaze(newMaze);
                        printf("Cannot exit through here!");
                    }
                    else{
                        playerMove(X,Y-1,&X,&Y,newMaze,&potions);
                    }
                }
                break;
            case 'd':
                if(maze[X][Y + 1] != 'w'){
                    if((X) == exitX && (Y+1) == exitY){
                        if(potions == 3){
                            playerMove(X,Y+1,&X,&Y,newMaze,&potions);
                            printf("Congrats! You finished!");
                            exit = getchar();
                        }
                        else{
                            printf("You do not have enough potions!");
                        }
                    }
                    else{
                        playerMove(X,Y+1,&X,&Y,newMaze,&potions);
                    }
                }
                break;
            default:

                break;
        }
    }
}
