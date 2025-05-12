#include <stdio.h>

#define SIZE 20

int whowin(char board[SIZE][SIZE], int x, int y, char player) {
    int directions[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};
    for(int i = 0; i < 4; i++) {
        int count = 1;
        int dx = directions[i][0];
        int dy = directions[i][1];

        int cx = x + dx;
        int cy = y + dy;
        while(cx >= 0 && cx < SIZE && cy >= 0 && cy < SIZE && board[cy][cx] == player) {
            count++;
            cx += dx;
            cy += dy;
        }

        cx = x - dx;
        cy = y - dy;
        while(cx >= 0 && cx < SIZE && cy >= 0 && cy < SIZE && board[cy][cx] == player) {
            count++;
            cx -= dx;
            cy -= dy;
        }

        if(count >= 5) return 1;
    }
    return 0;
}

void InputPosition(int p, char plane[SIZE][SIZE], int xy[2]) {
    char player = (p % 2 == 0) ? 'B' : 'W';
    int x, y;

    while(1) {
        printf("%c's turn (x,y 1-%d): ", player, SIZE);
        if(scanf("%d,%d", &x, &y) != 2) {  
            while(getchar() != '\n'); 
            printf("Plz Try Agin\n");
            continue;
        }
        x--;
        y--;

        if(x < 0 || x >= SIZE || y < 0 || y >= SIZE) {
            printf("Plz Try Agin\n");
            continue;
        }
        if(plane[y][x] != '+') {
            printf("Plz Try Agin\n");
            continue;
        }

        plane[y][x] = player;
        xy[0] = x;
        xy[1] = y;
        break;
    }
}

void printBoard(char board[SIZE][SIZE]) {
    printf("\n  ");
    for(int x = 0; x < SIZE; x++) 
        printf(" %2d", x+1);  

    for(int y = 0; y < SIZE; y++) {
        printf("\n%2d ",  y+1);  
        for(int x = 0; x < SIZE; x++) 
            printf(" %c ", board[y][x]);
    }
    printf("\n");
}

int main() {
    char board[SIZE][SIZE];
    int p = 0;
    int xy[2];

    
    for(int y = 0; y < SIZE; y++) 
        for(int x = 0; x < SIZE; x++) 
            board[y][x] = '+';

    while(1) {
        printBoard(board);
        InputPosition(p, board, xy);

        if(p>8 && whowin(board, xy[0], xy[1], (p%2==0)?'B':'W')) {
            printBoard(board);
            printf("\nPLAYER  %c WIN!!\n", (p%2==0)?'B':'W');
            break;
        }
        p++;
    }
    return 0;
}
