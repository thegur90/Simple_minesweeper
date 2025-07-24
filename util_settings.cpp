#include <string>
#include <iostream>
#include <fstream>

void save_settings(int board_rows, int board_cols, int mine_count) {
    std::ofstream file("minesweeper_settings.txt");
    if (file.is_open()) {
        file << board_rows << std::endl;
        file << board_cols << std::endl;
        file << mine_count << std::endl;
        file.close();
    }
}

void load_settings(int& board_rows, int& board_cols, int& mine_count) {
    std::ifstream file("minesweeper_settings.txt");
    if (file.is_open()) {
        file >> board_rows;
        file >> board_cols;
        file >> mine_count;
        file.close();
    }
    // If file doesn't exist or reading fails, variables keep their default values
}