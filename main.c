#include <stdio.h>
#include "maze_gen.c"
#include "ncurses.h"
#define FOG_CLEARED '.'
// print maze with optional message to the ncurses default window
void printMaze(struct maze newMaze, struct maze *fogMaze, int *potions, char *message, int x, int y, int fogRadius)
{
    // clear the default window
    clear();
    printw("Potions: %d\n", *potions);
    int startRow = x - fogRadius;
    int endRow = x + fogRadius;
    int startCol = y - fogRadius;
    int endCol = y + fogRadius;
    if (startRow < 0)
    {
        startRow = 0;
    }
    if (endRow >= newMaze.h)
    {
        endRow = newMaze.h - 1;
    }
    if (startCol < 0)
    {
        startCol = 0;
    }
    if (endCol >= newMaze.w)
    {
        endCol = newMaze.w - 1;
    }
    for (int i = startRow; i <= endRow; i++)
    {
        for (int j = startCol; j <= endCol; j++)
        {
            fogMaze->a[i][j] = FOG_CLEARED;
        }
    }
    for (int i = 0; i < fogMaze->h; i++)
    {
        for (int j = 0; j < fogMaze->w; j++)
        {
            if (fogMaze->a[i][j] == FOG_CLEARED)
            {
                printw("%c", newMaze.a[i][j]);
            }
            else
            {
                printw(" ");
            }
        }
        printw("\n");
    }
    // print message if it is not null
    if (message != NULL)
    {
        printw("%s", message);
    }
    // refresh the default window
    refresh();
}

// check if the position is a potion
void checkPotion(int x, int y, char **maze, int *potions)
{
    if (maze[x][y] == '#')
    {
        *potions += 1;
    }
}

// find the entry point of the generated maze along the left wall and the exit point of the maze
void findEntryExit(int *entryX, int *entryY, int *exitX, int *exitY, int *x, int *y, struct maze newMaze, int *potions)
{
    char **maze = newMaze.a;
    for (int i = 0; i < newMaze.h; i++)
    {
        if (maze[i][newMaze.w - 1] == ' ')
        {
            *exitX = i;
            *exitY = newMaze.w - 1;
        }
        if (maze[i][0] == ' ')
        {
            *x = i;
            *y = 0;
            maze[*x][*y] = '/';
        }
        else if (maze[i][0] == '#')
        {
            *x = i;
            *y = 0;
            *potions = *potions + 1;
            maze[*x][*y] = '/';
        }
    }
    *entryX = *x;
    *entryY = *y;
}

// move the player to the new position
void playerMove(int x, int y, int *playerX, int *playerY, struct maze newMaze, int *potion)
{
    // get the 2d array from the struct
    char **maze = newMaze.a;
    checkPotion(x, y, maze, potion);
    // set the previous position to a space and the new position to the player
    maze[*playerX][*playerY] = ' ';
    *playerX = x;
    *playerY = y;
    maze[*playerX][*playerY] = '/';
}

void inputErrorCheck()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF){}
    printf("That is a not a valid input! Please try again.\n");
}

int main()
{
    int width, height, cellSize, seed,fogRadius, potions = 0;
    int exitX, exitY, entryX, entryY,X,Y;
    int c;
    // get user input
    printf("Width:");
    while (scanf("%d", &width) != 1)
    {
        inputErrorCheck();
        printf("Width:");
    }
    printf("Height:");
    while (scanf("%d", &height) != 1)
    {
        inputErrorCheck();
        printf("Height:");
    }
    printf("Cell Size:");
    while (scanf("%d", &cellSize) != 1)
    {
        inputErrorCheck();
        printf("Cell Size:");
    }
    printf("Fog Radius:");
    while (scanf("%d", &fogRadius) != 1)
    {
        inputErrorCheck();
        printf("Fog Radius:");
    }
    printf("Random Seed:");
    while (scanf("%d", &seed) != 1)
    {
        inputErrorCheck();
        printf("Random Seed:");
    }


    // generate the maze
    struct maze newMaze = generate_maze(width, height, cellSize, seed);
    struct maze fogMaze = generate_maze(width, height, cellSize, seed);

    char **maze = newMaze.a;

    // find the entry and exit points
    findEntryExit(&entryX, &entryY,&exitX, &exitY, &X, &Y, newMaze, &potions);

    // start ncurses
    initscr();
    cbreak();
    noecho();
    printMaze(newMaze, &fogMaze, &potions, NULL, X, Y,fogRadius);

    char input = ' ';
    // get user input
    while ((input = getchar()))
    {
        // check if the user has pressed a key and if so move the player in that direction if allowed
        switch (input)
        {
        case 'w':
            if ( X > 0 & maze[X - 1][Y] != 'w')
            {
                playerMove(X - 1, Y, &X, &Y, newMaze, &potions);
                printMaze(newMaze, &fogMaze, &potions, NULL, X, Y,fogRadius);
            }
            break;
        case 's':
            if ( X < newMaze.h -1 & maze[X + 1][Y] != 'w')
            {
                playerMove(X + 1, Y, &X, &Y, newMaze, &potions);
                printMaze(newMaze, &fogMaze, &potions, NULL, X, Y,fogRadius);
            }
            break;
        case 'a':
            if (Y >= 0 & maze[X][Y - 1] != 'w')
            {

                if (X == entryX && (Y - 1) < entryY)
                {
                    printMaze(newMaze, &fogMaze, &potions, "Cannot exit through here!", X, Y,fogRadius);
                }
                else
                {
                    playerMove(X, Y - 1, &X, &Y, newMaze, &potions);
                    printMaze(newMaze, &fogMaze, &potions, NULL, X, Y,fogRadius);
                }
            }
            break;
        case 'd':
            if (Y < newMaze.w -1 & maze[X][Y + 1] != 'w')
            {
                if ((X == exitX && Y + 1 == exitY))
                {
                    if (potions == 3)
                    {
                        playerMove(X, Y + 1, &X, &Y, newMaze, &potions);
                        printMaze(newMaze, &fogMaze, &potions, "You have escaped!", X, Y,fogRadius);
                        return 0;
                    }
                    else
                    {
                        printMaze(newMaze, &fogMaze, &potions, "You need all 3 potions to escape!", X, Y,fogRadius);
                    }
                }
                else
                {
                    playerMove(X, Y + 1, &X, &Y, newMaze, &potions);
                    printMaze(newMaze, &fogMaze, &potions, NULL, X, Y,fogRadius);
                }
            }
            break;
        default:
            break;
        }
    }
    // end ncurses
    endwin();
}
