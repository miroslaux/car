#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define CAR_WIDTH  32
#define CAR_HEIGHT 7
#define MAX_SMOKE  150

typedef struct {
    float x, y;
    float vx, vy;
    int life;
    int char_idx;
} Particle;

Particle smoke_stack[MAX_SMOKE];

void init_smoke() {
    for (int i = 0; i < MAX_SMOKE; i++) smoke_stack[i].life = -1;
}

void add_smoke(int x, int y) {
    for (int i = 0; i < MAX_SMOKE; i++) {
        if (smoke_stack[i].life <= 0) {
            smoke_stack[i].x = x;
            smoke_stack[i].y = y;
            smoke_stack[i].vx = (rand() % 10) / 10.0f + 0.5f; 
            smoke_stack[i].vy = -(rand() % 5) / 10.0f;
            smoke_stack[i].life = 15 + (rand() % 10);
            smoke_stack[i].char_idx = 0;
            break;
        }
    }
}

void update_and_draw_smoke() {
    char *shapes = "@#%*o+:. ";
    attron(COLOR_PAIR(2));
    for (int i = 0; i < MAX_SMOKE; i++) {
        if (smoke_stack[i].life > 0) {
            smoke_stack[i].x += smoke_stack[i].vx;
            smoke_stack[i].y += smoke_stack[i].vy;
            smoke_stack[i].life--;
            
            int idx = 8 - (smoke_stack[i].life / 3);
            if (idx > 8) idx = 8;

            int dy = (int)smoke_stack[i].y;
            int dx = (int)smoke_stack[i].x;
            if (dx >= 0 && dx < COLS && dy >= 0 && dy < LINES) {
                mvaddch(dy, dx, shapes[idx]);
            }
        }
    }
    attroff(COLOR_PAIR(2));
}

void draw_utility_car(int x, int y, int frame) {
    static char *frames[2][CAR_HEIGHT] = {
        {
            "          __________________    ",
            "         |                | \\   ",
            "  _______|________________|__\\  ",
            " [|_|_|_|  _             _    ] ",
            " |        / \\___________/ \\   | ",
            " |_______( o )_________( o )__| ",
            "          \\_/           \\_/     "
        },
        {
            "          __________________    ",
            "         |                | \\   ",
            "  _______|________________|__\\  ",
            " [|_|_|_|  _             _    ] ",
            " |        / \\___________/ \\   | ",
            " |_______( O )_________( O )__| ",
            "          \\_/           \\_/     "
        }
    };

    for (int i = 0; i < CAR_HEIGHT; ++i) {
        for (int j = 0; j < CAR_WIDTH; j++) {
            int cur_x = x + j;
            int cur_y = y + i;
            if (cur_x >= 0 && cur_x < COLS && cur_y >= 0 && cur_y < LINES) {
                char ch = frames[frame % 2][i][j];
                
                if (ch == 'o' || ch == 'O') attron(COLOR_PAIR(3) | A_BOLD);
                else if (i == 1 || i == 2) attron(COLOR_PAIR(4));
                else attron(COLOR_PAIR(1));

                mvaddch(cur_y, cur_x, ch);
                
                attroff(COLOR_PAIR(1) | COLOR_PAIR(3) | COLOR_PAIR(4) | A_BOLD);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    initscr();
    start_color();
    curs_set(0);
    nodelay(stdscr, TRUE);
    noecho();

    init_pair(1, COLOR_CYAN, COLOR_BLACK);   // Body
    init_pair(2, COLOR_WHITE, COLOR_BLACK);  // Smoke
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Wheels/Lights
    init_pair(4, COLOR_BLUE, COLOR_BLACK);   // Windows

    signal(SIGINT,  SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    init_smoke();

    int x = COLS;
    int base_y = LINES / 2 - 3;
    int frame = 0;

    while (x + CAR_WIDTH > 0) {
        erase();

        // Physics: Simulate suspension bounce
        int offset_y = (frame % 6 < 3) ? 0 : 1;
        int cur_y = base_y + offset_y;

        if (frame % 2 == 0) {
            add_smoke(x + 12, cur_y + 6);
            add_smoke(x + 26, cur_y + 6);
        }

        update_and_draw_smoke();
        draw_utility_car(x, cur_y, frame);

        refresh();
        usleep(35000);
        
        x -= 2;
        frame++;
    }

    endwin();
    return 0;
}
