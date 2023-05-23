#include <stdio.h>
#include "maze_gen.c"
#include "ncurses.h"
#define FOG_RANGE 3
#define FOG_CLEARED '.'
// print maze with optional message to the ncurses default window
void printMaze(struct maze newMaze, struct maze *fogMaze, int *potions, char *message, int x, int y)
{
    // clear the default window
    clear();
    printw("Potions: %d\n", *potions);
    int startRow = x - FOG_RANGE;
    int endRow = x + FOG_RANGE;
    int startCol = y - FOG_RANGE;
    int endCol = y + FOG_RANGE;
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

// find the entry point of the generated maze along the left wall
void findEntry(int *entryX, int *entryY, int *x, int *y, struct maze newMaze, int *potions)
{
    char **maze = newMaze.a;
    for (int i = 0; i < newMaze.h; i++)
    {
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

// find the exit point of the generated maze along the right wall
void findExit(int *x, int *y, struct maze newMaze)
{
    char **maze = newMaze.a;
    for (int i = 0; i < newMaze.h; i++)
    {
        if (maze[i][newMaze.w - 1] == ' ')
        {
            *x = i;
            *y = newMaze.w - 1;
        }
    }
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
    int width;
    int height;
    int cellSize;
    int seed;
    int potions = 0;

    int X = 0;
    int Y = 0;

    int entryX = 0;
    int entryY = 0;

    int exitX = 0;
    int exitY = 0;
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
    findEntry(&entryX, &entryY, &X, &Y, newMaze, &potions);
    findExit(&exitX, &exitY, newMaze);

    // start ncurses
    initscr();
    cbreak();
    noecho();
    printMaze(newMaze, &fogMaze, &potions, NULL, X, Y);

    char input = ' ';
    // get user input
    while ((input = getchar()))
    {
        // check if the user has pressed a key and if so move the player in that direction if allowed
        switch (input)
        {
        case 'w':
            if (maze[X - 1][Y] != 'w')
            {
                playerMove(X - 1, Y, &X, &Y, newMaze, &potions);
                printMaze(newMaze, &fogMaze, &potions, NULL, X, Y);
            }
            break;
        case 's':
            if (maze[X + 1][Y] != 'w')
            {
                playerMove(X + 1, Y, &X, &Y, newMaze, &potions);
                printMaze(newMaze, &fogMaze, &potions, NULL, X, Y);
            }
            break;
        case 'a':
            if (maze[X][Y - 1] != 'w')
            {

                if (X == entryX && (Y - 1) < entryY)
                {
                    printMaze(newMaze, &fogMaze, &potions, "Cannot exit through here!", X, Y);
                }
                else
                {
                    playerMove(X, Y - 1, &X, &Y, newMaze, &potions);
                    printMaze(newMaze, &fogMaze, &potions, NULL, X, Y);
                }
            }
            break;
        case 'd':
            if (maze[X][Y + 1] != 'w')
            {
                if ((X == exitX && Y + 1 == exitY))
                {
                    if (potions == 3)
                    {
                        playerMove(X, Y + 1, &X, &Y, newMaze, &potions);
                        printMaze(newMaze, &fogMaze, &potions, "You have escaped!", X, Y);
                        return 0;
                    }
                    else
                    {
                        printMaze(newMaze, &fogMaze, &potions, "You need all 3 potions to escape!", X, Y);
                    }
                }
                else
                {
                    playerMove(X, Y + 1, &X, &Y, newMaze, &potions);
                    printMaze(newMaze, &fogMaze, &potions, NULL, X, Y);
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
