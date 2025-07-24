#include <SDL2/SDL.h>
#include "util_renderimage.h"

#pragma region GAME

float calc_aspect_ratio(int w,int h){
    return float(w) / float(h);
}

int calc_correct_height(int width, float aspect_ratio){
    return width / aspect_ratio;
}

int calc_correct_width(int height, float aspect_ratio){
    return height * aspect_ratio;
}

int calc_startup_width(int board_cols, int slotsize, int margin, int sidebar) {
    return (2 * margin) + (board_cols * slotsize) + sidebar;
}

int calc_startup_height(int board_rows, int slotsize, int margin) {
    return (2 * margin) + (board_rows * slotsize);
}

float calc_scale_factor(int original_width, int original_height, int new_width, int new_height) {
    //this function determines if a measurement should be scaled according to width or height.
    float width_scale = (float)new_width / (float)original_width;
    float height_scale = (float)new_height / (float)original_height;
    
    // Use the smaller scale to ensure everything fits
    return (width_scale < height_scale) ? width_scale : height_scale;
}

int apply_scale(int original_value, float scale_factor) {
    return (int)(original_value * scale_factor);
}

#pragma region resolution_scaling
void get_screen_resolution(int& screen_width, int& screen_height) { //int& is the key here - these are essentially "global" variables.
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    screen_width = display_mode.w;
    screen_height = display_mode.h;
}

int calc_min_window_width(int screen_width) {
    return screen_width / 4;  // 25% of screen width
}

int calc_max_window_width(int screen_width) {
    return (int)(screen_width * 0.8f);  // 80% of screen width
}

int calc_min_window_height(int screen_height) {
    return screen_height / 4;  // 25% of screen height
}

int calc_max_window_height(int screen_height) {
    return (int)(screen_height * 0.69f);  // 69% of screen height - idk why but this actually looks good, don't @ me
}

int clamp_width(int width, int min_width, int max_width) { //clamp = limit the ability to scale past those values.
    if (width < min_width) return min_width;
    if (width > max_width) return max_width;
    return width;
}

int clamp_height(int height, int min_height, int max_height) {
    if (height < min_height) return min_height;
    if (height > max_height) return max_height;
    return height;
}

int calc_smart_window_y_position(int board_rows) {
    int base_y = 400; // Base distance from top
    int row_factor = board_rows * 16;
    return base_y - row_factor;
}

#pragma endregion


void handle_window_resize(SDL_Window* window, int new_width, int new_height, 
                         float target_aspect_ratio, int original_width, int original_height,
                         int& scaled_margin, int& scaled_slotsize, int& scaled_sidebar) {
    /* This one exists to combine all the scaling steps into one sequence for the 3 &ints it needs to scale.

    the sequence is as follows:
    first calculate the aspect ratio and clamp it if needed using the stuff in resolution_scaling region.
    then use that new aspect ratio to calculate the new width/height with it.
    then pass the new w/h to calc_scale_factor
    and finally use that function's result in apply_scale

    */

    Uint32 window_flags = SDL_GetWindowFlags(window);
    bool is_maximized = (window_flags & SDL_WINDOW_MAXIMIZED);
    // Constants for original sizes
    const int ORIGINAL_MARGIN = 30;
    const int ORIGINAL_SLOTSIZE = 40;
    const int ORIGINAL_SIDEBAR = 200;

    if (is_maximized) {
        //do nothing - i kept this here in case i need it :)
    }
    else{
        int screen_width, screen_height;
        get_screen_resolution(screen_width, screen_height);

        int min_width = calc_min_window_width(screen_width);
        int max_width = calc_max_window_width(screen_width);
        int min_height = calc_min_window_height(screen_height);
        int max_height = calc_max_window_height(screen_height);

        new_width = clamp_width(new_width, min_width, max_width);
        new_height = clamp_height(new_height, min_height, max_height);

        int correct_height = calc_correct_height(new_width, target_aspect_ratio);
        if (new_height != correct_height) {
        SDL_SetWindowSize(window, new_width, correct_height);
        new_height = correct_height;
        }
    }
    
    
    //Calculate scale factor from original dimensions
    float scale_factor = calc_scale_factor(original_width, original_height, new_width, new_height);
    
    //Apply scaling to all elements
    scaled_margin = apply_scale(ORIGINAL_MARGIN, scale_factor);
    scaled_slotsize = apply_scale(ORIGINAL_SLOTSIZE, scale_factor);
    scaled_sidebar = apply_scale(ORIGINAL_SIDEBAR, scale_factor);
}

#pragma endregion


#pragma region UI

SDL_Rect calc_timer_bg_position(int sidebar_width, int margin, int bg_height){
    int x = margin*2;  // Start with margin from left edge
    int y = margin;  // Start with margin from top
    int width = sidebar_width - (2 * margin);  // Full sidebar width minus margins on both sides
    int height = bg_height;
    
    return create_rect(x, y, width, height);
}

int calc_timer_bg_bottom(SDL_Rect* timer){
    return timer->y + timer->h;
}

SDL_Rect calc_mine_count_bg_position(int sidebar_width, int margin, int bg_height, int timer_bg_bottom){
    int x = margin*2;
    int y = timer_bg_bottom + margin;  // Position below timer with margin spacing
    int width = sidebar_width - (2 * margin);
    int height = bg_height;
    
    return create_rect(x, y, width, height);
}

SDL_Rect calc_timer_icon_position(SDL_Rect timer_bg) {

    int icon_size = timer_bg.h;  // Square icon using background height
    int margin = (int)(timer_bg.h * 0.1f);  // Small margin (10% of bg height)

    int x = timer_bg.x - icon_size - margin;  // Small margin from left edge of background
    int y = timer_bg.y + (timer_bg.h - icon_size) / 2;  // Vertically centered in background
    
    return create_rect(x, y, icon_size, icon_size);
}

SDL_Rect calc_mine_icon_position(SDL_Rect mine_bg) {
    int icon_size = mine_bg.h;  // Square icon using background height
    int margin = (int)(mine_bg.h * 0.1f);  // Small margin (10% of bg height)
    
    int x = mine_bg.x - icon_size - margin;  
    int y = mine_bg.y + (mine_bg.h - icon_size) / 2;  // Vertically centered
    
    return create_rect(x, y, icon_size, icon_size);
}

SDL_Rect calc_timer_digit_position(int digit_index, SDL_Rect timer_bg, int digit_width) { //digit_index is going to be 0-7, 3 and 6 are colons.
    int x = timer_bg.x + (digit_index * digit_width);
    int y = timer_bg.y;
    int height = timer_bg.h;  // Digits fill the background height
    
    return create_rect(x, y, digit_width, height);
}

SDL_Rect calc_minecount_digit_position(int digit_index, SDL_Rect timer_bg, int digit_width, int mine_count) { 
    //digit_index is going to be 0-2 for a **centered** maximum of a triple digit int.
    //passing mine_count to determine auto-centering.

    int center = timer_bg.x + (timer_bg.w/2);
    int margin = digit_width/2;
    int offset = 0;
    
    if (mine_count > 99) {
    // 3-digit number: positions should be -1, 0, +1
    if (digit_index == 0) offset = -1;
    else if (digit_index == 1) offset = 0;
    else if (digit_index == 2) offset = 1;
    }
    else if (mine_count > 9) {
        // 2-digit number: positions should be -0.5, +0.5
        if (digit_index == 0) offset = -1;
        else if (digit_index == 1) offset = 1;
    }

    int x = center + ((offset*1.5f) * margin) - (margin*1.7);
    int y = timer_bg.y;
    int height = timer_bg.h;  // Digits fill the background height
    
    return create_rect(x, y, digit_width, height);
}

#pragma endregion