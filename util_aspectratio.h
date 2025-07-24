#pragma once
#include <SDL2/SDL.h>

float calc_aspect_ratio(int w,int h);

int calc_correct_height(int width, float aspect_ratio);

int calc_correct_width(int height, float aspect_ratio);

int calc_startup_width(int board_cols, int slotsize, int margin, int sidebar);

int calc_startup_height(int board_rows, int slotsize, int margin);

float calc_scale_factor(int original_width, int original_height, int new_width, int new_height);

int apply_scale(int original_value, float scale_factor);

void get_screen_resolution(int& screen_width, int& screen_height);

int calc_min_window_width(int screen_width);

int calc_max_window_width(int screen_width);

int calc_min_window_height(int screen_height);

int calc_max_window_height(int screen_height);

int clamp_width(int width, int min_width, int max_width);

int clamp_height(int height, int min_height, int max_height);

void handle_window_resize(SDL_Window* window, int new_width, int new_height, 
                         float target_aspect_ratio, int original_width, int original_height,
                         int& scaled_margin, int& scaled_slotsize, int& scaled_sidebar);

SDL_Rect calc_timer_bg_position(int sidebar_width, int margin, int bg_height);

int calc_timer_bg_bottom(SDL_Rect* timer);

SDL_Rect calc_mine_count_bg_position(int sidebar_width, int margin, int bg_height, int timer_bg_bottom);

SDL_Rect calc_timer_icon_position(SDL_Rect timer_bg);
SDL_Rect calc_mine_icon_position(SDL_Rect mine_bg);

SDL_Rect calc_timer_digit_position(int digit_index, SDL_Rect timer_bg, int digit_width);

SDL_Rect calc_minecount_digit_position(int digit_index, SDL_Rect timer_bg, int digit_width, int mine_count);

int calc_smart_window_y_position(int board_rows);