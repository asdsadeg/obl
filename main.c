#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "ui.h"
#include "board.h"

int main() {
    int row_count, column_count, mine_count;

    Game *game = create_game();

    if (!read_player_name(game)) {
        printf("Failed to read player name.\n");
        destroy_game(game);
        return 1;
    }

    printf("Zadajte parametre hracieho poľa:\n");
    while (!get_game_parameters(&row_count, &column_count, &mine_count)) {
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Prosím, zadajte správne hodnoty.\n");
    }

    game->board = create_board(row_count, column_count, mine_count);
    run(game);
    destroy_game(game);
    return 0;
}
