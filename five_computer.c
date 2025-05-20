#include <stdio.h>
#include <stdlib.h>

 
#define SIZE 20// 오목판 크기


int my_abs(int x) {
    return x < 0 ? -x : x;
}


int whowin(char (*board)[SIZE], int x, int y, char player) {
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


int checkSequence(char (*board)[SIZE], int x, int y, int dx, int dy, char player, int *empty_x, int *empty_y) {
    int count = 1;  
    int spaces = 0;
    int temp_empty_x = -1, temp_empty_y = -1;
    

    int cx = x + dx;
    int cy = y + dy;
    while(cx >= 0 && cx < SIZE && cy >= 0 && cy < SIZE && (board[cy][cx] == player || board[cy][cx] == '+') && spaces <= 1) {
        if(board[cy][cx] == player) {
            count++;
        } else {  
            if(temp_empty_x == -1) {
                temp_empty_x = cx;
                temp_empty_y = cy;
            }
            spaces++;
        }
        cx += dx;
        cy += dy;
    }
    

    spaces = 0;
    cx = x - dx;
    cy = y - dy;
    while(cx >= 0 && cx < SIZE && cy >= 0 && cy < SIZE && (board[cy][cx] == player || board[cy][cx] == '+') && spaces <= 1) {
        if(board[cy][cx] == player) {
            count++;
        } else {  
            if(temp_empty_x == -1) {
                temp_empty_x = cx;
                temp_empty_y = cy;
            }
            spaces++;
        }
        cx -= dx;
        cy -= dy;
    }
    

    if(count >= 3 && temp_empty_x != -1) {
        *empty_x = temp_empty_x;
        *empty_y = temp_empty_y;
    }
    
    return count;
}


void computerMove(char (*board)[SIZE], int last_x, int last_y, int *xy) {
    int directions[8][2] = {{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}};
    int best_x = -1, best_y = -1;
    int max_sequence = 0;
    char opponent = 'B';  // 플레이어는 항상 흑돌
    char computer = 'W';  // 컴퓨터는 항상 백돌
    
  
    for(int y = 0; y < SIZE; y++) {
        for(int x = 0; x < SIZE; x++) {
            if(board[y][x] == opponent) {

                for(int i = 0; i < 4; i++) {
                    int empty_x = -1, empty_y = -1;
                    int count = checkSequence(board, x, y, directions[i][0], directions[i][1], opponent, &empty_x, &empty_y);
                    
                    if(count >= 4 && empty_x != -1) {
                      
                        xy[0] = empty_x;
                        xy[1] = empty_y;
                        board[empty_y][empty_x] = computer;
                        return;
                    } else if(count >= 3 && empty_x != -1 && max_sequence < 4) {
                        best_x = empty_x;
                        best_y = empty_y;
                        max_sequence = 3;
                    }
                }
            }
        }
    }
    

    if(best_x != -1 && best_y != -1) {
        xy[0] = best_x;
        xy[1] = best_y;
        board[best_y][best_x] = computer;
        return;
    }
    

    for(int i = 0; i < 8; i++) {
        int nx = last_x + directions[i][0];
        int ny = last_y + directions[i][1];
        
        if(nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx] == '+') {
            xy[0] = nx;
            xy[1] = ny;
            board[ny][nx] = computer;
            return;
        }
    }

    int min_distance = SIZE * SIZE; 
    int closest_x = -1, closest_y = -1;
    
    for(int y = 0; y < SIZE; y++) {
        for(int x = 0; x < SIZE; x++) {
            if(board[y][x] == '+') {
             
                int distance = my_abs(x - last_x) + my_abs(y - last_y);
                if(distance < min_distance) {
                    min_distance = distance;
                    closest_x = x;
                    closest_y = y;
                }
            }
        }
    }
    

    if(closest_x != -1) {
        xy[0] = closest_x;
        xy[1] = closest_y;
        board[closest_y][closest_x] = computer;
        return;
    }
}

void InputPosition(int p, char (*board)[SIZE], int *xy, int game_mode) {
    char player = (p % 2 == 0) ? 'B' : 'W';
    

    if(game_mode == 1 && player == 'W') {
        printf("Computer's turn (W)...\n");
        computerMove(board, xy[0], xy[1], xy);
        return;
    }
    

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
        if(board[y][x] != '+') {
            printf("Plz Try Agin\n");
            continue;
        }

        board[y][x] = player;
        xy[0] = x;
        xy[1] = y;
        break;
    }
}

void printBoard(char (*board)[SIZE]) {
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
    int xy[2] = {SIZE/2, SIZE/2}; 
    int game_mode;
    
    srand(time(NULL)); 
    
    // 보드 초기화
    for(int y = 0; y < SIZE; y++) 
        for(int x = 0; x < SIZE; x++) 
            board[y][x] = '+';
    
    // 게임 모드 선택
    printf("Choose game mode:\n");
    printf("1. Play against computer (You: Black, Computer: White)\n");
    printf("2. Two players\n");
    printf("Enter your choice (1 or 2): ");
    
    while(scanf("%d", &game_mode) != 1 || (game_mode != 1 && game_mode != 2)) {
        while(getchar() != '\n');
        printf("Invalid choice. Please enter 1 or 2: ");
    }
    
    while(1) {
        printBoard(board);
        InputPosition(p, board, xy, game_mode);

        if(p > 8 && whowin(board, xy[0], xy[1], (p%2==0)?'B':'W')) {
            printBoard(board);
            printf("\nPLAYER %c WIN!!\n", (p%2==0)?'B':'W');
            break;
        }
        p++;
    }
    return 0;
}
