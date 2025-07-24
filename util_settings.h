#pragma once

#include <string>
#include <iostream>
#include <fstream>

void save_settings(int board_rows, int board_cols, int mine_count);

void load_settings(int& board_rows, int& board_cols, int& mine_count);