#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define MAX_SIZE 8

int board[MAX_SIZE][MAX_SIZE];
int previousBoard[MAX_SIZE][MAX_SIZE];
int boardRows, boardCols;
int gameOver = 0;
int score = 0;
int previousScore = 0;

void setColor(int value) {
    int color = 7;
    switch (value) {
        case 2: color = 8; break;
        case 4: color = 7; break;
        case 8: color = 6; break;
        case 16: color = 4; break;
        case 32: color = 2; break;
        case 64: color = 3; break;
        case 128: color = 9; break;
        case 256: color = 11; break;
        case 512: color = 13; break;
        case 1024: color = 14; break;
        case 2048: color = 10; break;
        default: color = 15; break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawBoard() {
    system("cls");
    printf("2048 Game - Use WASD to move. Press Q to quit. U to undo.\n\n");
    printf("Score: %d\n\n", score);

    for (int i = 0; i < boardRows; i++) {
        for (int j = 0; j < boardCols; j++) {
            if (board[i][j] == 0) {
                printf(".\t");
            } else {
                setColor(board[i][j]);
                printf("%d\t", board[i][j]);
                setColor(7);  // Reset to white after printing
            }
        }
        printf("\n\n");
    }
}

void saveState() {
    for (int i = 0; i < boardRows; i++)
        for (int j = 0; j < boardCols; j++)
            previousBoard[i][j] = board[i][j];
    previousScore = score;
}

void undo() {
    for (int i = 0; i < boardRows; i++)
        for (int j = 0; j < boardCols; j++)
            board[i][j] = previousBoard[i][j];
    score = previousScore;
}

void addRandomTile() {
    int empty[MAX_SIZE * MAX_SIZE][2];
    int count = 0;
    for (int i = 0; i < boardRows; i++)
        for (int j = 0; j < boardCols; j++)
            if (board[i][j] == 0) {
                empty[count][0] = i;
                empty[count][1] = j;
                count++;
            }
    if (count == 0) return;
    int randIndex = rand() % count;
    int x = empty[randIndex][0];
    int y = empty[randIndex][1];
    board[x][y] = (rand() % 10 == 0) ? 4 : 2;
}

void initBoard() {
    for (int i = 0; i < boardRows; i++)
        for (int j = 0; j < boardCols; j++)
            board[i][j] = 0;
    score = 0;
    addRandomTile();
    addRandomTile();
    saveState();
}

int canMove() {
    for (int i = 0; i < boardRows; i++)
        for (int j = 0; j < boardCols; j++) {
            if (board[i][j] == 0)
                return 1;
            if (i < boardRows - 1 && board[i][j] == board[i + 1][j])
                return 1;
            if (j < boardCols - 1 && board[i][j] == board[i][j + 1])
                return 1;
        }
    return 0;
}

int slide(int row[MAX_SIZE], int length) {
    int changed = 0;
    int temp[MAX_SIZE] = {0};
    int index = 0;
    for (int i = 0; i < length; i++)
        if (row[i] != 0)
            temp[index++] = row[i];
    for (int i = 0; i < length; i++) {
        if (row[i] != temp[i])
            changed = 1;
        row[i] = temp[i];
    }
    return changed;
}

int combine(int row[MAX_SIZE], int length) {
    int changed = 0;
    for (int i = 0; i < length - 1; i++) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            score += row[i];
            row[i + 1] = 0;
            changed = 1;
        }
    }
    return changed;
}

int moveLeft() {
    int changed = 0;
    for (int i = 0; i < boardRows; i++) {
        int rowChanged = 0;
        rowChanged |= slide(board[i], boardCols);
        rowChanged |= combine(board[i], boardCols);
        rowChanged |= slide(board[i], boardCols);
        if (rowChanged) changed = 1;
    }
    return changed;
}

int moveRight() {
    int changed = 0;
    for (int i = 0; i < boardRows; i++) {
        int row[MAX_SIZE];
        for (int j = 0; j < boardCols; j++)
            row[j] = board[i][boardCols - 1 - j];
        int rowChanged = 0;
        rowChanged |= slide(row, boardCols);
        rowChanged |= combine(row, boardCols);
        rowChanged |= slide(row, boardCols);
        for (int j = 0; j < boardCols; j++)
            board[i][boardCols - 1 - j] = row[j];
        if (rowChanged) changed = 1;
    }
    return changed;
}

int moveUp() {
    int changed = 0;
    for (int j = 0; j < boardCols; j++) {
        int col[MAX_SIZE];
        for (int i = 0; i < boardRows; i++)
            col[i] = board[i][j];
        int colChanged = 0;
        colChanged |= slide(col, boardRows);
        colChanged |= combine(col, boardRows);
        colChanged |= slide(col, boardRows);
        for (int i = 0; i < boardRows; i++)
            board[i][j] = col[i];
        if (colChanged) changed = 1;
    }
    return changed;
}

int moveDown() {
    int changed = 0;
    for (int j = 0; j < boardCols; j++) {
        int col[MAX_SIZE];
        for (int i = 0; i < boardRows; i++)
            col[i] = board[boardRows - 1 - i][j];
        int colChanged = 0;
        colChanged |= slide(col, boardRows);
        colChanged |= combine(col, boardRows);
        colChanged |= slide(col, boardRows);
        for (int i = 0; i < boardRows; i++)
            board[boardRows - 1 - i][j] = col[i];
        if (colChanged) changed = 1;
    }
    return changed;
}

void selectBoardSize() {
    printf("Select board size:\n");
    printf("1. 2x2\n");
    printf("2. 4x4\n");
    printf("3. 4x8\n");
    printf("4. 8x8\n");
    printf("Enter your choice (1-4): ");
    int choice;
    scanf("%d", &choice);
    switch (choice) {
        case 1: boardRows = boardCols = 2; break;
        case 2: boardRows = boardCols = 4; break;
        case 3: boardRows = 4; boardCols = 8; break;
        case 4: boardRows = boardCols = 8; break;
        default: boardRows = boardCols = 4; break;
    }
}

int main() {
    srand(time(NULL));
    selectBoardSize();
    initBoard();

    while (!gameOver) {
        drawBoard();

        if (!canMove()) {
            printf("No moves left! Game Over!\n");
            break;
        }

        char ch = getch();
        int moved = 0;

        switch (ch) {
            case 'w': case 'W': saveState(); moved = moveUp(); break;
            case 's': case 'S': saveState(); moved = moveDown(); break;
            case 'a': case 'A': saveState(); moved = moveLeft(); break;
            case 'd': case 'D': saveState(); moved = moveRight(); break;
            case 'u': case 'U': undo(); continue;
            case 'q': case 'Q': gameOver = 1; continue;
            default: printf("Invalid input.\n"); Sleep(500); break;
        }

        if (moved)
            addRandomTile();
    }

    return 0;
}
