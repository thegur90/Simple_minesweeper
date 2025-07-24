#pragma once

#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>

#include "util_renderimage.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

bool check_point_in_rect(int x, int y, SDL_Rect* rect);
int get_max_mines(int cols,int rows);
void unfocus_custom(bool& r, bool& c, bool&m);

bool handle_config_window(SDL_Window* Window, SDL_Renderer* Renderer, int& board_rows, int& board_cols, int& mine_count);