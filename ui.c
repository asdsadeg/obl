#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ui.h"
#include "view.h"
#include "str_utils.h"
#include "board.h"

#define HOF_FILENAME "score.txt"

/* private functions */
void game_loop(Game *game);
void print_play_field(Game *game, int input_row, int input_column);
void print_score(Player *list_of_players, int number_of_all_players);
void print_input_rules();
void clear_screen();

/**
 * Ask for game parameters (rows, columns, mines) and validate them.
 * Returns true if parameters are valid, false otherwise.
 */
bool get_game_parameters(int *row_count, int *column_count, int *mine_count) {
    printf("Enter number of rows (1-%d): ", MAX_ROW_COUNT);
    if (scanf("%d", row_count) != 1 || *row_count <= 0 || *row_count > MAX_ROW_COUNT) {
        printf("Invalid input. Rows must be between 1 and %d.\n", MAX_ROW_COUNT);
        while (getchar() != '\n' && getchar() != EOF); // Clear input buffer
        return false;
    }

    printf("Enter number of columns (1-%d): ", MAX_COLUMN_COUNT);
    if (scanf("%d", column_count) != 1 || *column_count <= 0 || *column_count > MAX_COLUMN_COUNT) {
        printf("Invalid input. Columns must be between 1 and %d.\n", MAX_COLUMN_COUNT);
        while (getchar() != '\n' && getchar() != EOF);
        return false;
    }

    printf("Enter number of mines (1-%d): ", *row_count * *column_count - 1);
    if (scanf("%d", mine_count) != 1 || *mine_count <= 0 || *mine_count >= (*row_count * *column_count)) {
        printf("Invalid input. Mines must be between 1 and %d.\n", *row_count * *column_count - 1);
        while (getchar() != '\n' && getchar() != EOF);
        return false;
    }

    while (getchar() != '\n' && getchar() != EOF); // Clear input buffer
    return true;
}

/**
 * Ask a player for his name and store in the game.
 * Returns true if name was read successfully.
 */
bool read_player_name(Game *game) {
    assert(game != NULL);
    printf("Zadajte vaše meno prosím: \n");
    char name_of_player[MAX_PLAYER_LENGTH];
    if (fgets(name_of_player, MAX_PLAYER_LENGTH, stdin) == NULL) {
        return false;
    }
    str_remove_spaces(name_of_player);
    strcpy(game->player->name, name_of_player);
    return true;
}

void clear_screen() {
    system("clear");
}

/** 
 * Running game in loop
 */
void run(Game *game) {
    char choise;

    do {
        if (game == NULL || game->board == NULL) {
            printf("Error: Game or board is not initialized.\n");
            return;
        }

        play_game(game);

        while (getchar() != '\n');
        printf("Chcete hrať znova? (y/n): ");

        while (1) {
            choise = getchar();
            while (getchar() != '\n' && getchar() != EOF); // Clear input buffer
            if (choise == EOF) {
                printf("Input error or EOF reached.\n");
                printf("Good bye!\n");
                return;
            }
            if (choise == 'y' || choise == 'n') {
                break;
            } else {
                printf("Invalid answer, please enter 'y' or 'n': ");
            }
        }

        if (choise == 'n') {
            printf("Good bye!\n");
            return;
        } else if (choise == 'y') {
            // Get new parameters for the next game
            int row_count, column_count, mine_count;
            printf("Zadajte parametre hracieho poľa:\n");
            if (!get_game_parameters(&row_count, &column_count, &mine_count)) {
                printf("Failed to get game parameters. Exiting.\n");
                printf("Good bye!\n");
                return;
            }

            destroy_board(game->board);
            game->board = create_board(row_count, column_count, mine_count);
            if (game->board == NULL) {
                printf("Error: Failed to create game board.\n");
                printf("Good bye!\n");
                return;
            }
            reset_game_state(game);
            clear_screen();
        }

    } while (choise == 'y');
}

/**
 * Handle whole process of the Game
 */
void play_game(Game *game) {
    assert(game != NULL);
    assert(game->board != NULL);

    Player list_of_players[PLAYERS_LIMIT];
    int size_of_list = read_scores(HOF_FILENAME, list_of_players);
    if (size_of_list > 0) {
        print_score(list_of_players, size_of_list);
    }
    game_loop(game);

    if (game->game_state == SOLVED) {
        printf("Gratulujem %s. Riešenie je správне!\n", game->player->name);
    } else {
        printf("Ľutujem %s. Riešenie je nesprávне!\n", game->player->name);
    }
    printf("Vaše skóre je: %d\n", game->player->score);

    bool is_player_added_to_list = add_player_to_list(list_of_players, &size_of_list, *game->player);
    if (is_player_added_to_list) {
        save_scores(HOF_FILENAME, list_of_players, size_of_list);
    }
}

/**
 * Handles players input process
 * Loops until game state is playing
 */
void game_loop(Game *game) {
    assert(game != NULL);
    int input, input_row = -1, input_column = -1;
    do {
        print_play_field(game, input_row, input_column);
        print_input_rules();

        while ((input = scanf("%d %d", &input_row, &input_column)) == 0) {
            scanf("%*[^\n]");
            print_input_rules();
        }

        if (input != EOF) {
            open_tile(game, input_row - 1, input_column - 1);
        }

    } while (game->game_state == PLAYING);

    print_play_field(game, input_row, input_column);
}

void print_score(Player *list_of_players, int number_of_all_players) {
    char *text = view_hof(list_of_players, number_of_all_players);
    printf("%s", text);
    free(text);
}

void print_play_field(Game *game, int input_row, int input_column) {
    char *field = view_play_field(game->board, input_row, input_column);
    printf("\n%s\n", field);
    free(field);
}

void print_input_rules() {
    printf("Zadajte čísло riadka, medzerу a čísло stĺpca. Napр. 2 3, 5 9\n");
}
