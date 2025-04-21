#ifndef MINES_UI_H
#define MINES_UI_H

#include "game.h"

bool read_player_name(Game *game);
void play_game(Game *game);
void run(Game *game);
bool get_game_parameters(int *row_count, int *column_count, int *mine_count);

#endif //MINES_UI_H
