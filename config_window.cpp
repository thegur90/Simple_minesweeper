#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>

#include "util_renderimage.h"
#include "util_settings.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

bool check_point_in_rect(int x, int y, SDL_Rect* rect) { //copied from main, didn't fit in any other file :p
    return (x >= rect->x &&
          x < rect->x + rect->w &&
          y >= rect->y && 
          y < rect->y + rect->h);
}

int get_max_mines(int cols,int rows){
    int result = (rows*cols)*0.24f;
    if (result < 165){
        return result;
    }
    return 165;

}

void unfocus_custom(bool& r, bool& c, bool&m){
    r = false;
    c = false;
    m = false;
};

// This function will handle input, update the string, and return true if its value changed
// It also needs the current event and focus state
bool handle_numeric_input_field(
    std::string& current_input_str, // The string to modify (passed by reference)
    SDL_Rect& field_rect,     // The area of the input field
    int max_value,                  // Max allowed value for this field
    SDL_Event& e_config,            // The current SDL event
    bool& is_focused,               // True if THIS field is currently active (passed by reference)
    SDL_Renderer* renderer,         // For rendering if needed (e.g., cursor)
    const std::unordered_map<char, SDL_Texture*>& ui_tex // For rendering digits
){

    if (e_config.type != SDL_MOUSEBUTTONDOWN && 
        e_config.type != SDL_TEXTINPUT && 
        e_config.type != SDL_KEYDOWN) {
        return false;  // No change for irrelevant events
    }

    bool value_changed_this_call = false;

    int mouse_x = -1;
    int mouse_y = -1;
    int current_val_int = -1;

    if (e_config.type == SDL_MOUSEBUTTONDOWN){
        mouse_x = e_config.button.x;
        mouse_y = e_config.button.y;
        if (check_point_in_rect(mouse_x,mouse_y,&field_rect)){
            is_focused = true;
        }
        else{
            is_focused = false;
        }
    }


    if (is_focused){
        if (e_config.type == SDL_TEXTINPUT){
            char typed_char = e_config.text.text[0];
            if (isdigit(typed_char)) { // Check if the typed character is a digit
                // Prevent leading zero unless it's the only digit typed
                if (current_input_str.empty() && typed_char == '0') {
                    // Do nothing, don't add a leading zero if string is empty
                } else {
                    current_input_str += typed_char; // Append the digit
                    value_changed_this_call = true;
                }
            }
        }
        if (e_config.type == SDL_KEYDOWN) { // This event type is for special keys like Backspace
            if (e_config.key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                if (!current_input_str.empty()) {
                    current_input_str.pop_back(); // Remove the last character
                    value_changed_this_call = true;
                }
            }
        }
    
    

        //clamping
        if (!current_input_str.empty()){
            current_val_int = std::stoi(current_input_str);
            if (current_val_int > max_value){
                current_val_int = max_value;
                current_input_str = std::to_string(max_value); // Update string to clamped value
                value_changed_this_call = true;
            }
            if (current_val_int < 1){
                current_val_int = 1;
                current_input_str = "1";
                value_changed_this_call = true;
            }
        }
        else { // If the string is empty after backspacing
            current_val_int = 1; // Default to 1 for rendering/logic, but keep string empty for input
            value_changed_this_call = true;
        }
    }
    

    //render cursor if i feel like it in the future.

    return value_changed_this_call;
    
    
}


bool handle_config_window(SDL_Window* Window, SDL_Renderer* Renderer, int& board_rows, int& board_cols, int& mine_count){

    bool should_regenerate_board  = false;

    int options_window_w = 378;
    int options_window_h = 579; //pain

    int chosen_rows = board_rows;
    int chosen_cols = board_cols;
    int chosen_mines = mine_count;

    std::string rows_input_str = std::to_string(chosen_rows);
    std::string cols_input_str = std::to_string(chosen_cols);
    std::string mines_input_str = std::to_string(chosen_mines);

    //for custom setting
    bool rows_is_focused = false;
    bool cols_is_focused = false;
    bool mines_is_focused = false;
    
    bool config_window_running  = true;
    bool temp_config_window_running = true;

    SDL_Window* configWindow = nullptr;
    SDL_Renderer* configRenderer = nullptr;
    SDL_CreateWindowAndRenderer(options_window_w,options_window_h,SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_POPUP_MENU ,&configWindow,&configRenderer);
    if(configRenderer == NULL){
        std::cout << "NULL RENDERER" << std::endl;
    }

    std::unordered_map<char, SDL_Texture*> config_ui_tex;

    config_ui_tex['0'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/0.png");
    config_ui_tex['1'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/1.png");
    config_ui_tex['2'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/2.png");
    config_ui_tex['3'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/3.png");
    config_ui_tex['4'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/4.png");
    config_ui_tex['5'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/5.png");
    config_ui_tex['6'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/6.png");
    config_ui_tex['7'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/7.png");
    config_ui_tex['8'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/8.png");
    config_ui_tex['9'] = AssignImageToTexture(configRenderer,"assets/ui/timer_digits/9.png");

    config_ui_tex['B'] = AssignImageToTexture(configRenderer,"assets/ui/options_basic.png");
    config_ui_tex['I'] = AssignImageToTexture(configRenderer,"assets/ui/options_int.png");
    config_ui_tex['A'] = AssignImageToTexture(configRenderer,"assets/ui/options_adv.png");
    config_ui_tex['C'] = AssignImageToTexture(configRenderer,"assets/ui/options_custom_board.png");
    config_ui_tex['N'] = AssignImageToTexture(configRenderer,"assets/ui/options_newgame.png");

    config_ui_tex['m'] = AssignImageToTexture(configRenderer,"assets/ui/red_rect.png"); //red marker


    //these values right here are why i didn't choose frontend school. please someone commit better values thanks..
    SDL_Rect Basic = create_rect_with_aspect_ratio(0,-10,378,"assets/ui/options_basic.png");
    SDL_Rect Intermediate = create_rect_with_aspect_ratio(-1,110,379,"assets/ui/options_int.png");
    SDL_Rect Advanced = create_rect_with_aspect_ratio(0,208,378,"assets/ui/options_adv.png");
    SDL_Rect Custom = create_rect_with_aspect_ratio(0,310,378,"assets/ui/options_custom_board.png");
    SDL_Rect NewGame = create_rect_with_aspect_ratio(0,493,378,"assets/ui/options_newgame.png");

    SDL_Rect option_rows = create_rect(27,456,100,50);
    SDL_Rect option_cols = create_rect(139,456,100,50);
    SDL_Rect option_minecount = create_rect(252,456,100,50);

    SDL_Rect Selector[] = {create_rect(0,100,378,10),create_rect(0,208,378,10),create_rect(0,315,378,10),create_rect(0,497,378,10)};
    int marker = 0; //0-3

    SDL_Event e_config;

    int mouse_x = -1; //init
    int mouse_y = -1;

    SDL_SetWindowTitle(configWindow,"Options");
    SetMinesweeperWindowIcon(configWindow);

    while (config_window_running ){

        while (SDL_PollEvent(&e_config)){
            if (e_config.type == SDL_QUIT){
                // This will still catch global quit events, good to keep
                temp_config_window_running = false;
            }
            else if (e_config.type == SDL_WINDOWEVENT){
                if(e_config.window.event == SDL_WINDOWEVENT_CLOSE ){
                    if (e_config.window.windowID == SDL_GetWindowID(configWindow)){
                        temp_config_window_running = false;   
                       }
                    else{
                        SDL_FlashWindow(configWindow,SDL_FLASH_UNTIL_FOCUSED);
                    }
                    
                }
            }
            else if  (e_config.type == SDL_KEYDOWN){
                if (e_config.key.keysym.scancode == SDL_SCANCODE_ESCAPE) { // Check the specific key
                    temp_config_window_running = false;
                }
            }
            else if(e_config.type == SDL_MOUSEBUTTONDOWN){
                mouse_x = e_config.button.x;
                mouse_y = e_config.button.y;
                if (e_config.button.button == SDL_BUTTON_LEFT){
                    if (check_point_in_rect(mouse_x,mouse_y,&NewGame)){
                        temp_config_window_running = false;
                        board_cols = chosen_cols;
                        board_rows = chosen_rows;
                        mine_count = chosen_mines;
                        should_regenerate_board  = true;
                    }
                    //I probably should have made this part an array as well, oh well. it works and it's very readable.
                    if (check_point_in_rect(mouse_x,mouse_y,&Basic)){
                        marker = 0;
                        
                            chosen_rows = 9;
                            chosen_cols = 9;
                            chosen_mines = 10;
                            rows_input_str = std::to_string(chosen_rows);
                            cols_input_str = std::to_string(chosen_cols);
                            mines_input_str = std::to_string(chosen_mines);
                            unfocus_custom(rows_is_focused,cols_is_focused,mines_is_focused);
                        
                        
                    }
                    else if (check_point_in_rect(mouse_x,mouse_y,&Intermediate)){
                        marker = 1;
                        
                            chosen_rows = 16;
                            chosen_cols = 16;
                            chosen_mines = 40;
                            rows_input_str = std::to_string(chosen_rows);
                            cols_input_str = std::to_string(chosen_cols);
                            mines_input_str = std::to_string(chosen_mines);
                            unfocus_custom(rows_is_focused,cols_is_focused,mines_is_focused);
                        
                    }
                    else if (check_point_in_rect(mouse_x,mouse_y,&Advanced)){
                        marker = 2;
                        
                            chosen_rows = 16;
                            chosen_cols = 30;
                            chosen_mines = 99;
                            rows_input_str = std::to_string(chosen_rows);
                            cols_input_str = std::to_string(chosen_cols);
                            mines_input_str = std::to_string(chosen_mines);
                            unfocus_custom(rows_is_focused,cols_is_focused,mines_is_focused);
                        
                    }
                    else if (check_point_in_rect(mouse_x,mouse_y,&Custom)){
                        marker = 3;
                        
                            //custom stuff
                        
                    }
                }
            }
            // 3 calls to handle_numeric_input_field, one for each int.
                        if (marker == 3) { // Only process input for custom fields if Custom is selected
                // Call handle_numeric_input_field for rows
                if (handle_numeric_input_field(rows_input_str, option_rows, 23, e_config, rows_is_focused, configRenderer, config_ui_tex)) {
                    // If rows_input_str changed, convert it to an int and update chosen_rows
                    if (!rows_input_str.empty()) {
                        chosen_rows = std::stoi(rows_input_str);
                    } else {
                        chosen_rows = 1; // Default to 1 if empty
                    }
                }

                // Call handle_numeric_input_field for cols
                if (handle_numeric_input_field(cols_input_str, option_cols, 30, e_config, cols_is_focused, configRenderer, config_ui_tex)) {
                    // If cols_input_str changed, convert it to an int and update chosen_cols
                    if (!cols_input_str.empty()) {
                        chosen_cols = std::stoi(cols_input_str);
                    } else {
                        chosen_cols = 1; // Default to 1 if empty
                    }
                }

                // Call handle_numeric_input_field for mines
                if (handle_numeric_input_field(mines_input_str, option_minecount, get_max_mines(chosen_rows,chosen_cols), e_config, mines_is_focused, configRenderer, config_ui_tex)) {
                    // If mines_input_str changed, convert it to an int and update chosen_mines
                    if (!mines_input_str.empty()) {
                        chosen_mines = std::stoi(mines_input_str);
                    } else {
                        chosen_mines = 1; // Default to 1 if empty
                    }
                }
            }
        }

       SDL_SetRenderDrawColor(configRenderer,150,160,170,255); // background color
       SDL_RenderClear(configRenderer);

       //render here

       RenderImage(config_ui_tex.at('B'),&Basic,configRenderer);
       RenderImage(config_ui_tex.at('I'),&Intermediate,configRenderer);
       RenderImage(config_ui_tex.at('A'),&Advanced,configRenderer);
       RenderImage(config_ui_tex.at('C'),&Custom,configRenderer);
       RenderImage(config_ui_tex.at('N'),&NewGame,configRenderer);

       //Draw rectangles around input fields
        SDL_SetRenderDrawColor(configRenderer, 200, 200, 200, 255); // color for for unfocused
        if (!rows_is_focused) SDL_RenderDrawRect(configRenderer, &option_rows);
        if (!cols_is_focused) SDL_RenderDrawRect(configRenderer, &option_cols);
        if (!mines_is_focused)SDL_RenderDrawRect(configRenderer, &option_minecount);

        SDL_SetRenderDrawColor(configRenderer, 100, 100, 180, 255); // color for focused
        if (rows_is_focused) SDL_RenderDrawRect(configRenderer, &option_rows);
        if (cols_is_focused) SDL_RenderDrawRect(configRenderer, &option_cols);
        if (mines_is_focused)SDL_RenderDrawRect(configRenderer, &option_minecount);


       RenderNumberFromTextureDigits(chosen_rows, option_rows, 30, config_ui_tex, configRenderer);
       RenderNumberFromTextureDigits(chosen_cols, option_cols, 30, config_ui_tex, configRenderer);
       RenderNumberFromTextureDigits(chosen_mines, option_minecount, 30, config_ui_tex, configRenderer);

       RenderImage(config_ui_tex.at('m'),&Selector[marker],configRenderer);

       //end render here

       SDL_RenderPresent(configRenderer);
       SDL_Delay(16);
       config_window_running = temp_config_window_running;
       }


    for (auto& entry : config_ui_tex) {
        if (entry.second) { // Check if texture is not NULL before destroying
            SDL_DestroyTexture(entry.second);
        }
    }

    SDL_DestroyRenderer(configRenderer); configRenderer = NULL;
    SDL_DestroyWindow(configWindow); configWindow = NULL;

    save_settings(board_rows, board_cols, mine_count); // Save the new settings

    return should_regenerate_board;
    }
