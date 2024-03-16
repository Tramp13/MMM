#include "mmm.h"
#include <ncurses.h>
#include "raylib.h"
#include <time.h>

int get_random_direction() {
    int value = GetRandomValue(0, 3);
    int direction;
    if (value == 0) {
        direction = NORTH;
    }
    if (value == 1) {
        direction = EAST;
    }
    if (value == 2) {
        direction = SOUTH;
    }
    if (value == 3) {
        direction = WEST;
    }
    return direction;
}

Vector2 delta_from_direction(int direction) {
    Vector2 delta = (Vector2) {0, 0};
    if (direction == NORTH) {
        delta.x = 0;
        delta.y = -1;
    }
    if (direction == EAST) {
        delta.x = 1;
        delta.y = 0;
    }
    if (direction == SOUTH) {
        delta.x = 0;
        delta.y = 1;
    }
    if (direction == WEST) {
        delta.x = -1;
        delta.y = 0;
    }
    return delta;
}

PuzzleBox PuzzleBox_create() {
    PuzzleBox puzzle_box;
    puzzle_box.x[0] = 0;
    puzzle_box.y[0] = 0;
    puzzle_box.puzzle_type[0] = NO_PUZZLE;
    puzzle_box.room_count = 1;
    puzzle_box.entrance[0] = 0;

    for (int i = 1; i < 10; i++) {
        puzzle_box.puzzle_type[i] = NO_PUZZLE;
        int position_found = false;
        int x = 0;
        int y = 0;
        int from_room;
        while (!position_found) {
            Vector2 direction = delta_from_direction(GetRandomValue(0, 3));
            from_room = GetRandomValue(0, puzzle_box.room_count - 1);
            x = puzzle_box.x[from_room] + direction.x;
            y = puzzle_box.y[from_room] + direction.y;
            position_found = true;
            for (int j = 0; j < puzzle_box.room_count; j++) {
                if (puzzle_box.x[j] == x  &&
                    puzzle_box.y[j] == y) {
                    position_found = false;
                }
            }
        }
        puzzle_box.x[i] = x;
        puzzle_box.y[i] = y;
        puzzle_box.entrance[i] = from_room;
        int lock_type = GetRandomValue(0, 2);
        puzzle_box.lock_type[i] = lock_type;
        int room_found = false;
        int solution_room;
        while (!room_found) {
            solution_room = GetRandomValue(0, puzzle_box.room_count);
            if (puzzle_box.puzzle_type[solution_room] == NO_PUZZLE) {
                room_found = true;
            }
        }
        puzzle_box.puzzle_type[solution_room] = lock_type;
        puzzle_box.room_count += 1;
    }
    return puzzle_box;
} 

int main()
{
    int seed = (int) time(NULL);
    SetRandomSeed(seed);

    int ch;

    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    PuzzleBox puzzlebox = PuzzleBox_create();
    for (int i = 0; i < puzzlebox.room_count; i++) {
        printf("wow");
        int x = (puzzlebox.x[i] * 2) + 11;
        int y = (puzzlebox.y[i] * 2) + 7;
        printf("%d, %d\n", x, y);
        int puzzle_type = puzzlebox.puzzle_type[i];
        char puzzle_char = '?';
        if (puzzle_type == NO_PUZZLE) {
            puzzle_char = '+';
        }
        if (puzzle_type == KEY) {
            puzzle_char = 'K';
        }
        if (puzzle_type == FIGHT) {
            puzzle_char = 'F';
        }
        int entrance = puzzlebox.entrance[i];
        int sx = puzzlebox.x[i];
        int sy = puzzlebox.y[i];
        int ex = puzzlebox.x[entrance];
        int ey = puzzlebox.y[entrance];
        if (ex < sx) {
            mvaddch(y, x - 1, '>');
        } else if (ex > sx) {
            mvaddch(y, x + 1, '<');
        } else if (ey < sy) {
            mvaddch(y - 1, x, 'v');
        } else if (ey > sy) {
            mvaddch(y + 1, x, '^');
        } else {
            if (i != 0) {
                mvaddch(0, 0, 'f');
            }
            if (ex == sx && ey == sy) {
                mvaddch(0, 2, 'f');
            }
            if (entrance == i && i != 0) {
                mvaddch(0, 4, 'f');
            }
        }

        mvaddch(y, x, puzzle_char);
    }
    refresh();
    getch();
    endwin();

    return 0;
}
