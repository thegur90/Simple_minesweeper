#pragma once
#include <string>

void reveal_all_mines(std::string& board);

bool is_this_near_that(int this_index, int that_index, int num_col);

int random_between(int min, int max);

int get_index_from_xy(int x, int y, int cols);

char get_visible_state(int x, int y, std::string& board, int cols);
char get_slot_value(int x, int y, std::string& board, int cols);
char set_visible_state(int x, int y, std::string& board, int cols, char state);
void set_slot_value(int x, int y, std::string& board, int cols, char value);

std::string generate_random_board(int row, int col, int mine_percent);

void flood_reveal(int index, std::string& board, int cols);

void chord_reveal(int index, std::string& board, int cols);

void flag_chord(int index, std::string& board, int cols);

int count_flags_around_index(int index, std::string& board, int cols);
int count_unrevealed_neighbors(int index, std::string& board, int cols);

bool detect_win(std::string& board);

int calc_active_flags(std::string& board);