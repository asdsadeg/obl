#include "greatest.h"
#include "../ui.h"
#include "../game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_play_field(Game *game, int input_row, int input_column);
void print_score(Player *list_of_players, int number_of_all_players);
void print_input_rules();

TEST test_read_player_name_success() {
    Game *game = create_game();
    game->board = create_board(1, 1, 0);

    FILE *input = freopen("tests/ui/test_input_name.txt", "r", stdin);
    ASSERT(input != NULL);

    bool result = read_player_name(game);
    ASSERT(result);
    ASSERT_STR_EQ("TestPlayer", game->player->name);

    destroy_game(game);
    PASS();
}

TEST test_read_player_name_fail() {
    Game *game = create_game();
    FILE *input = freopen("/dev/null", "r", stdin);
    ASSERT(input != NULL);

    bool result = read_player_name(game);
    ASSERT_EQ(0, result);

    destroy_game(game);
    PASS();
}

TEST test_play_game_runs() {
    Game *game = create_game();
    game->board = create_board(1, 1, 0);
    game->player->score = 0;
    strcpy(game->player->name, "Demo");

    FILE *input = freopen("tests/ui/test_input_play_game.txt", "r", stdin);
    ASSERT(input != NULL);

    play_game(game);
    ASSERT(game->game_state != PLAYING);

    destroy_game(game);
    PASS();
}

TEST test_game_loop_with_invalid_input() {
    Game *game = create_game();
    game->board = create_board(1, 1, 0);

    FILE *input = freopen("tests/ui/test_input_invalid.txt", "r", stdin);
    ASSERT(input != NULL);

    play_game(game);

    destroy_game(game);
    PASS();
}

TEST test_run_game_loop_y_then_n() {
    Game *game = create_game();
    game->board = create_board(1, 1, 0);

    FILE *input = freopen("tests/ui/test_run_y_then_n.txt", "r", stdin);
    ASSERT(input != NULL);

    run(game);

    fclose(input);

    destroy_game(game);
    PASS();
}

TEST test_get_game_parameters() {
    FILE *input = freopen("tests/ui/test_input_parameters.txt", "r", stdin);
    ASSERT(input != NULL);
    int row_count, column_count, mine_count;
    bool result = get_game_parameters(&row_count, &column_count, &mine_count);
    ASSERT(result);
    ASSERT_EQ(5, row_count);
    ASSERT_EQ(5, column_count);
    ASSERT_EQ(5, mine_count);
    fclose(input);
    PASS();
}

TEST test_print_input_rules_does_not_crash() {
    print_input_rules();
    PASS();
}

TEST test_print_play_field_does_not_crash() {
    Game *game = create_game();
    game->board = create_board(3, 3, 0);
    print_play_field(game, -1, -1);
    destroy_game(game);
    PASS();
}

TEST test_print_score_does_not_crash() {
    Player players[2] = {
        {.score = 10, .name = "Alice"},
        {.score = 20, .name = "Bob"}
    };
    print_score(players, 2);
    PASS();
}

SUITE(test_ui) {
    RUN_TEST(test_read_player_name_success);
    RUN_TEST(test_read_player_name_fail);
    RUN_TEST(test_play_game_runs);
    RUN_TEST(test_game_loop_with_invalid_input);
    RUN_TEST(test_run_game_loop_y_then_n);
    RUN_TEST(test_get_game_parameters);
    RUN_TEST(test_print_input_rules_does_not_crash);
    RUN_TEST(test_print_play_field_does_not_crash);
    RUN_TEST(test_print_score_does_not_crash);
}
