#include "greatest.h"
#include "../ui.h"
#include "../game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Прототипы функций из ui.c для тестирования
void print_play_field(Game *game, int input_row, int input_column);
void print_score(Player *list_of_players, int number_of_all_players);
void print_input_rules();
void clear_screen();

// Вспомогательная функция для создания временных файлов
FILE* create_temp_input(const char* content) {
    char template[] = "/tmp/test_input_XXXXXX";
    int fd = mkstemp(template);
    FILE* file = fdopen(fd, "w+");
    fprintf(file, "%s", content);
    rewind(file);
    return file;
}

TEST test_read_player_name_success() {
    Game *game = create_game();
    game->board = create_board(1, 1, 0);

    FILE *input = create_temp_input("TestPlayer\n");
    freopen(template, "r", stdin);

    bool result = read_player_name(game);
    ASSERT(result);
    ASSERT_STR_EQ("TestPlayer", game->player->name);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_read_player_name_empty() {
    Game *game = create_game();
    FILE *input = create_temp_input("\n");
    freopen(template, "r", stdin);

    bool result = read_player_name(game);
    ASSERT_FALSE(result);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_read_player_name_fail() {
    Game *game = create_game();
    FILE *input = freopen("/dev/null", "r", stdin);

    bool result = read_player_name(game);
    ASSERT_FALSE(result);

    destroy_game(game);
    PASS();
}

TEST test_get_game_parameters_valid() {
    FILE* input = create_temp_input("5\n10\n15\n");
    freopen(template, "r", stdin);

    int rows, cols, mines;
    ASSERT(get_game_parameters(&rows, &cols, &mines));
    ASSERT_EQ(5, rows);
    ASSERT_EQ(10, cols);
    ASSERT_EQ(15, mines);
    
    fclose(input);
    PASS();
}

TEST test_get_game_parameters_invalid_rows() {
    FILE* input = create_temp_input("0\n50\n5\n10\n15\n");
    freopen(template, "r", stdin);

    int rows, cols, mines;
    ASSERT(get_game_parameters(&rows, &cols, &mines));
    ASSERT_EQ(5, rows);
    
    fclose(input);
    PASS();
}

TEST test_get_game_parameters_invalid_mines() {
    FILE* input = create_temp_input("5\n10\n0\n20\n5\n");
    freopen(template, "r", stdin);

    int rows, cols, mines;
    ASSERT(get_game_parameters(&rows, &cols, &mines));
    ASSERT_EQ(5, mines);
    
    fclose(input);
    PASS();
}

TEST test_get_game_parameters_eof() {
    freopen("/dev/null", "r", stdin);
    
    int rows, cols, mines;
    ASSERT_FALSE(get_game_parameters(&rows, &cols, &mines));
    
    PASS();
}

TEST test_play_game_runs() {
    Game *game = create_game();
    game->board = create_board(1, 1, 0);
    game->player->score = 0;
    strcpy(game->player->name, "Demo");

    FILE* input = create_temp_input("1 1\n");
    freopen(template, "r", stdin);

    play_game(game);
    ASSERT(game->game_state != PLAYING);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_game_loop_with_invalid_input() {
    Game *game = create_game();
    game->board = create_board(3, 3, 1);

    FILE* input = create_temp_input("0 0\n-1 -1\n4 4\na b\n1 1\n");
    freopen(template, "r", stdin);

    play_game(game);
    ASSERT(game->game_state != PLAYING);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_game_loop_eof_handling() {
    Game *game = create_game();
    game->board = create_board(3, 3, 1);
    
    freopen("/dev/null", "r", stdin);
    game_loop(game);
    
    destroy_game(game);
    PASS();
}

TEST test_run_game_loop_y_then_n() {
    Game *game = create_game();
    game->board = create_board(3, 3, 1);

    FILE* input = create_temp_input("y\n3\n3\n1\n1 1\nn\n");
    freopen(template, "r", stdin);

    run(game);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_run_with_board_creation_failure() {
    Game *game = create_game();
    game->board = create_board(3, 3, 1);

    FILE* input = create_temp_input("y\n100 100 9999\n");
    freopen(template, "r", stdin);

    run(game);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_run_with_invalid_choice() {
    Game *game = create_game();
    game->board = create_board(3, 3, 1);

    FILE* input = create_temp_input("x\ny\n3\n3\n1\n1 1\nn\n");
    freopen(template, "r", stdin);

    run(game);

    fclose(input);
    destroy_game(game);
    PASS();
}

TEST test_print_input_rules_output() {
    // Перенаправляем stdout в файл для проверки вывода
    FILE* output = fopen("test_output.txt", "w+");
    FILE* old_stdout = stdout;
    stdout = output;

    print_input_rules();

    // Проверяем вывод
    rewind(output);
    char buffer[256];
    fgets(buffer, sizeof(buffer), output);
    ASSERT_STR_EQ("Zadajte čísло riadka, medzerу a čísло stĺpca. Napр. 2 3, 5 9\n", buffer);

    // Восстанавливаем stdout
    stdout = old_stdout;
    fclose(output);
    remove("test_output.txt");
    PASS();
}

TEST test_print_play_field_with_selection() {
    Game *game = create_game();
    game->board = create_board(3, 3, 1);

    // Проверяем что функция не падает
    print_play_field(game, 1, 1);
    print_play_field(game, 2, 2);
    print_play_field(game, 3, 3);
    print_play_field(game, -1, -1);
    
    destroy_game(game);
    PASS();
}

TEST test_print_score_edge_cases() {
    // Пустой список
    Player empty_list[1] = {{0}};
    print_score(empty_list, 0);

    // Один игрок
    Player single_player[1] = {{.score = 10, .name = "Single"}};
    print_score(single_player, 1);

    // Полный список
    Player full_list[PLAYERS_LIMIT];
    for (int i = 0; i < PLAYERS_LIMIT; i++) {
        snprintf(full_list[i].name, MAX_PLAYER_LENGTH, "Player%d", i);
        full_list[i].score = i * 10;
    }
    print_score(full_list, PLAYERS_LIMIT);
    
    PASS();
}

TEST test_clear_screen() {
    // Просто проверяем что функция выполняется без ошибок
    clear_screen();
    PASS();
}

SUITE(test_ui) {
    RUN_TEST(test_read_player_name_success);
    RUN_TEST(test_read_player_name_empty);
    RUN_TEST(test_read_player_name_fail);
    RUN_TEST(test_get_game_parameters_valid);
    RUN_TEST(test_get_game_parameters_invalid_rows);
    RUN_TEST(test_get_game_parameters_invalid_mines);
    RUN_TEST(test_get_game_parameters_eof);
    RUN_TEST(test_play_game_runs);
    RUN_TEST(test_game_loop_with_invalid_input);
    RUN_TEST(test_game_loop_eof_handling);
    RUN_TEST(test_run_game_loop_y_then_n);
    RUN_TEST(test_run_with_board_creation_failure);
    RUN_TEST(test_run_with_invalid_choice);
    RUN_TEST(test_print_input_rules_output);
    RUN_TEST(test_print_play_field_with_selection);
    RUN_TEST(test_print_score_edge_cases);
    RUN_TEST(test_clear_screen);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(test_ui);
    GREATEST_MAIN_END();
}
