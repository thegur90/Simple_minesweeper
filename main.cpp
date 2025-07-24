//minesweeper in c++ by thegur90 - [https://github.com/thegur90/Simple_minesweeper]
#pragma region IMPORTS_INCLUDES

#include <iostream>
#include <stdio.h>
#include <string>
#include <unordered_map>

#define SDL_MAIN_HANDLED

#include "util_renderimage.h"
#include "util_modifystringdata.h"
#include "util_aspectratio.h"
#include "config_window.h"
#include "util_settings.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>



#pragma endregion

#pragma region HELPERS

bool point_in_rect(int x, int y, SDL_Rect* rect) { //helper function to clean up main. collapsed in vscode by default
    return (x >= rect->x &&
          x < rect->x + rect->w &&
          y >= rect->y && 
          y < rect->y + rect->h);
}



static bool init() {
if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    fprintf(stderr, "could not initialize sdl2_image: %s\n", IMG_GetError());
    return false;
   }
  return true;
}


#pragma endregion

#pragma region BOARD_DATA



SDL_Rect get_slot_rect(int index,int testing_board_rows, int testing_board_cols,int margin, int slotsize, int sidebar){

    int slot_rect_point_x = sidebar + margin + slotsize*((index%testing_board_cols));
    int slot_rect_point_y = margin + slotsize*((index/testing_board_cols));

    return create_rect(slot_rect_point_x,slot_rect_point_y,slotsize,slotsize);
}

//the big one
char calc_visible_state(SDL_Rect* current_rect ,int x, int y, std::string& board, int cols, int click_mouse_x, int click_mouse_y,bool left_mouse_down, bool right_mouse_down){
    //mouse_x, mouse_y are passed every CLICK, so those aren't absolute mouse pos detection. i need to set new variables for that. lm_state = left mouse button state.
    int current_mouse_x, current_mouse_y;
    SDL_GetMouseState(&current_mouse_x, &current_mouse_y);
    char current = get_visible_state(x, y, board, cols);
    char current_value = get_slot_value(x,y,board,cols);
   
    
    if (current == 'n'){

        if (point_in_rect(current_mouse_x, current_mouse_y, current_rect)){
            //click detection goes here
            if(point_in_rect(click_mouse_x, click_mouse_y, current_rect) && left_mouse_down){
                return set_visible_state(x,y,board,cols,'c');
            }
            else{
                return set_visible_state(x,y,board,cols,'h');
            }
            

        }

    }
    else if (current == 'h'){

        if (point_in_rect(current_mouse_x, current_mouse_y, current_rect)){
            if(point_in_rect(click_mouse_x, click_mouse_y, current_rect) && left_mouse_down){
                return set_visible_state(x,y,board,cols,'c');
            }
            else if(point_in_rect(click_mouse_x, click_mouse_y, current_rect) && right_mouse_down){
                return set_visible_state(x,y,board,cols,'H');
            }
        }
        else{
            return set_visible_state(x,y,board,cols,'n');
        }


    }
    else if (current == 'c'){
        if (left_mouse_down == false){
            if (point_in_rect(current_mouse_x, current_mouse_y, current_rect)){ //this is where you reveal a slot.

                if (current_value == 'm'){
                    reveal_all_mines(board);
                    set_slot_value(x,y,board,cols,'x');
                }

                int current_index = get_index_from_xy(x,y,cols);
                flood_reveal(current_index,board,cols); //see modifystringdata.cpp - this one is a recursive mind melter
                return set_visible_state(x,y,board,cols,'r');
                
            }
            else{
                return set_visible_state(x,y,board,cols,'n');
            }

        }


    }
    //for flagged slots - i don't want a reveal nor do i want a click. only normal or hover.
    else if(current == 'N'){
        if(point_in_rect(click_mouse_x, click_mouse_y, current_rect) && right_mouse_down){
            return set_visible_state(x,y,board,cols,'n');
        }
        else if((point_in_rect(click_mouse_x, click_mouse_y, current_rect)) == true){ //hover detection when flagged
            return set_visible_state(x,y,board,cols,'H');
        }
    }

    else if(current == 'H'){
        if(point_in_rect(click_mouse_x, click_mouse_y, current_rect) && right_mouse_down){
            return set_visible_state(x,y,board,cols,'h');
        }
        else if((point_in_rect(click_mouse_x, click_mouse_y, current_rect)) == false){ //hover detection when flagged
            return set_visible_state(x,y,board,cols,'N');
        }
    }
    else{
        return current;
    }
    return current;
}


void handle_shift_left_click(int index, std::string& board, int cols){
    int x = index % cols;
    int y = index / cols;

    char cell_value = get_slot_value(x, y, board, cols);

    if ((cell_value - '0') == count_flags_around_index(index, board ,cols)){ //cursed char/int conversion to cell value, if it works it's not stupid :)
        chord_reveal(index,board,cols);
    }
    //else do nothing at all :)
}

void handle_shift_right_click(int index, std::string& board, int cols){ //cursed char/int conversion goes brrrrrrrrrr in here
    int x = index % cols;
    int y = index / cols;

    char cell_value = get_slot_value(x, y, board, cols); 
    int required_flags = cell_value - '0';
    int current_flags = count_flags_around_index(index, board, cols);
    int remaining_flags_needed = required_flags - current_flags;

    int unrevealed_neighbors = count_unrevealed_neighbors(index, board, cols);

    if (remaining_flags_needed > 0 && unrevealed_neighbors == remaining_flags_needed) { 
        //flag chording should only work when there's an exact amount of flags to put around the clicked slot.
        flag_chord(index, board, cols);
    }
}


#pragma endregion

#pragma region RESET_GAME

void reset_everything( //:feelsstrongman:
    bool& game_still_going,
    bool& temp_game_still_going,
    bool& win,
    bool& lose,
    Uint64& game_start_time,
    bool& timer_started,
    int& current_minutes,
    int& current_seconds,
    int& current_centiseconds,
    bool& first_click_of_the_game,
    std::string& board_data,
    int& remaining_mines,
    int& mouse_x,
    int& mouse_y,
    bool& left_mouse_down,
    bool& right_mouse_down,
    bool& right_click_this_frame,  
    int& stored_press_x,
    int& stored_press_y,
    int& board_rows, //values might change.
    int& board_cols,
    int& mine_count
) {
    // Reset all the passed-by-reference variables to their initial values
    game_still_going = true;
    temp_game_still_going = true;
    win = false;
    lose = false;
    game_start_time = 0;
    timer_started = false;
    current_minutes = 0;
    current_seconds = 0;
    current_centiseconds = 0;
    first_click_of_the_game = true;
    mouse_x = -1;
    mouse_y = -1;
    // ... reset other input state variables ...
    left_mouse_down = false;
    right_mouse_down = false;
    right_click_this_frame = false;
    stored_press_x = -1;
    stored_press_y = -1;

    // Regenerate the board data
    board_data = generate_random_board(board_rows, board_cols, mine_count);
    remaining_mines = mine_count; 
}

#pragma endregion

int SDL_main(int argc, char *argv[])

{
#pragma region INIT

    bool running = true;
    bool game_still_going = true;
    bool temp_game_still_going = true;
    bool win = false;
    bool lose = false;

    Uint64 Perf_Freq = SDL_GetPerformanceFrequency();
    Uint64 game_start_time = 0; //this is for the run timer
    bool timer_started = false;
    int current_minutes = 0;
    int current_seconds = 0;
    int current_centiseconds = 0;

    bool first_click_of_the_game = true; //to generate board properly and not RNG start on a mine. - and to tell the timer when to start.

    //default values follow standard minesweeper conventions
    int board_rows = 9; // defaults
    int board_cols = 9;
    int mine_count = 10;
    load_settings(board_rows, board_cols, mine_count); // load saved settings if they exist
    int total_slots = board_cols* board_rows; // calculate after loading

    int remaining_mines = mine_count;

    const int max_slots = 690; //23 rows * 30 columns

    std::string board_data = generate_random_board(board_rows,board_cols,mine_count); //generate board once just for init - i will regenerate board if needed.

    int margin = 30;
    int slotsize = 40;
    int sidebar = 200;
    

    int SCREEN_WIDTH = calc_startup_width(board_cols, slotsize, margin, sidebar);
    int SCREEN_HEIGHT = calc_startup_height(board_rows, slotsize, margin);

    int mouse_x = -1; //init
    int mouse_y = -1;

    int num_slots = board_data.length() / 2; //initialize an array of rects for each slot on the board
    SDL_Rect rect_array[max_slots];
    for (int i = 0;i < num_slots;i++){
        rect_array[i] = get_slot_rect(i, board_rows, board_cols, margin, slotsize,sidebar);
    }

    bool left_mouse_down = false;
    bool right_mouse_down = false;
    bool right_click_this_frame = false;
    int stored_press_x = -1; //default to invalid position
    int stored_press_y = -1;

    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;
    SDL_Event e;


    SDL_Init(SDL_INIT_EVERYTHING);
    

    if (!init()) return 1;

    

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI, &Window, &Renderer);
    SDL_SetWindowTitle(Window,"Minesweeper");
    SetMinesweeperWindowIcon(Window);

#pragma endregion

    
#pragma region MAP_TEXTURES

    std::unordered_map<char, SDL_Texture*> slot_tex; //all the game slot textures go in a single unordered map that is passed to every slot render function.

    slot_tex['n'] = AssignImageToTexture(Renderer,"assets/slots/slot_covered.png"); //n = normal
    slot_tex['h'] = AssignImageToTexture(Renderer,"assets/slots/slot_hover.png"); // h = hover
    slot_tex['c'] = AssignImageToTexture(Renderer,"assets/slots/slot_click.png"); // c = clicked
    slot_tex['r'] = AssignImageToTexture(Renderer,"assets/slots/slot_empty.png"); // r = revealed
    slot_tex['m'] = AssignImageToTexture(Renderer,"assets/slots/slot_mine.png"); //m = mine
    slot_tex['x'] = AssignImageToTexture(Renderer,"assets/slots/slot_mine_x.png"); // x = mine but with a red "x" on top

    slot_tex['N'] = AssignImageToTexture(Renderer,"assets/slots/slot_covered.png"); //flagged, normal
    slot_tex['H'] = AssignImageToTexture(Renderer,"assets/slots/slot_hover.png"); //flagged, hovered

    slot_tex['F'] = AssignImageToTexture(Renderer,"assets/slots/flag.png"); //flag overlay

    slot_tex['1'] = AssignImageToTexture(Renderer,"assets/slots/num_1.png");
    slot_tex['2'] = AssignImageToTexture(Renderer,"assets/slots/num_2.png");
    slot_tex['3'] = AssignImageToTexture(Renderer,"assets/slots/num_3.png");
    slot_tex['4'] = AssignImageToTexture(Renderer,"assets/slots/num_4.png");
    slot_tex['5'] = AssignImageToTexture(Renderer,"assets/slots/num_5.png");
    slot_tex['6'] = AssignImageToTexture(Renderer,"assets/slots/num_6.png");
    slot_tex['7'] = AssignImageToTexture(Renderer,"assets/slots/num_7.png");
    slot_tex['8'] = AssignImageToTexture(Renderer,"assets/slots/num_8.png");

    std::unordered_map<char, SDL_Texture*> ui_tex; //and another unordered map for the non-slot ui elements.

    ui_tex['c'] = AssignImageToTexture(Renderer,"assets/ui/clock.png");
    ui_tex['m'] = AssignImageToTexture(Renderer,"assets/ui/mine_icon.png");
    ui_tex['b'] = AssignImageToTexture(Renderer,"assets/ui/stat_bg.png");
    ui_tex['i'] = AssignImageToTexture(Renderer,"assets/ui/controls.png");

    ui_tex['0'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/0.png");
    ui_tex['1'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/1.png");
    ui_tex['2'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/2.png");
    ui_tex['3'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/3.png");
    ui_tex['4'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/4.png");
    ui_tex['5'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/5.png");
    ui_tex['6'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/6.png");
    ui_tex['7'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/7.png");
    ui_tex['8'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/8.png");
    ui_tex['9'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/9.png");
    ui_tex['C'] = AssignImageToTexture(Renderer,"assets/ui/timer_digits/colon.png");

    //yes i know you can for loop over the numbers to make this code more compact, but when pragma region exists - who cares :p

#pragma endregion

#pragma region MAIN_LOOP
    while (running) {
        right_click_this_frame = false;

        while (SDL_PollEvent(&e)){ 
            const Uint8* keystate = SDL_GetKeyboardState(NULL); //thank you boots, the documentation here is nested in narnia
            bool shift_pressed = keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]; //also narnia
            if (e.type == SDL_QUIT){
                running = false;
            }

            else if(e.type == SDL_WINDOWEVENT){
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {

                    int new_width = e.window.data1;
                    int new_height = e.window.data2;

                    float target_aspect_ratio = calc_aspect_ratio(SCREEN_WIDTH, SCREEN_HEIGHT);

                    handle_window_resize(Window, new_width, new_height, target_aspect_ratio, 
                           SCREEN_WIDTH, SCREEN_HEIGHT, 
                           margin, slotsize, sidebar); //see util_aspectratio.cpp for details, this one's chonky.

                    // Updating rect array sizes here.
                    for (int i = 0; i < num_slots; i++){
                        rect_array[i] = get_slot_rect(i, board_rows, board_cols, margin, slotsize,sidebar);
                    }
                }
            }

            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                mouse_x = e.button.x;
                mouse_y = e.button.y;

                if (e.button.button == SDL_BUTTON_MIDDLE) { //options window

                    if (handle_config_window(Window,Renderer,board_rows,board_cols,mine_count) == true){
                        reset_everything(game_still_going,temp_game_still_going,win,lose,game_start_time,
                        timer_started,current_minutes,current_seconds,
                        current_centiseconds,first_click_of_the_game,board_data,remaining_mines,mouse_x,mouse_y,left_mouse_down,right_mouse_down,
                        right_click_this_frame,stored_press_x,stored_press_y,board_rows,board_cols,mine_count);

                        margin = 30;      // Reset to original window size
                        slotsize = 40;    
                        sidebar = 200;   
                        
                        //logic to make new board size work:
                        int temp_screen_width = calc_startup_width(board_cols, slotsize, margin, sidebar);
                        int temp_screen_height = calc_startup_height(board_rows, slotsize, margin);
                        SDL_SetWindowSize(Window, temp_screen_width, temp_screen_height);

                        SCREEN_WIDTH = temp_screen_width;
                        SCREEN_HEIGHT = temp_screen_height;



                        // Debug output
                        
                        int actual_w, actual_h;
                        SDL_GetWindowSize(Window, &actual_w, &actual_h);
                        

                        //position window correctly....ish. enough for it to not go outside the monitor :p
                        SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, calc_smart_window_y_position(board_rows));

                        num_slots = board_cols*board_rows;
                        for (int i = 0; i < num_slots; i++){
                            rect_array[i] = get_slot_rect(i, board_rows, board_cols, margin, slotsize,sidebar);
                        }

                    }

                }
                
                else if (e.button.button == SDL_BUTTON_LEFT && !right_mouse_down) { //will only activate when right mouse button isn't pressed.
                    left_mouse_down = true;
                    if (first_click_of_the_game){ //regardless if you hold shift or not
                            game_start_time = SDL_GetPerformanceCounter(); //start timer
                            // Find which slot was clicked and handle board generation that cares about your first press.
                            for (int index = 0; index < num_slots; index++) { //index and not i because it helps me recognize this loop :)
                                if (point_in_rect(mouse_x, mouse_y, &rect_array[index])) {
                                    first_click_of_the_game = false;
                                    timer_started = true;
                                    int fx = index % board_cols;
                                    int fy = index / board_cols;
                                    while (get_slot_value(fx,fy,board_data,board_cols) != '0'){ 
                                        //will only break out once you click on an empty slot so you can properly start the game.
                                        board_data = generate_random_board(board_rows,board_cols,mine_count);
                                    }
                                    break;  // Found the slot, no need to check others
                                }
                            }
                        }
                    if (shift_pressed) {
                        std::cout << "shift_left_click at "; //auto reveal when possible

                        // Find which slot was clicked and handle chording
                        for (int i = 0; i < num_slots; i++) {
                            if (point_in_rect(mouse_x, mouse_y, &rect_array[i])) {
                                handle_shift_left_click(i, board_data, board_cols);
                                break;  // Found the slot, no need to check others
                            }
                        }

                    }
                    else{
                        std::cout << "left_click at "; //standard click on a slot
                        //CRITICAL: !!!!!valid slot click logic SHOULD NOT be here, that's in SDL_MOUSEBUTTONUP!!!!!
                        //get position - generate random board
                    }
                    std::cout << "(" << mouse_x << ", " << mouse_y << ")"; //co-ords printing
                    std::cout << ", ";

                    stored_press_x = mouse_x; //store the press co-ords on left click for cancel detection.
                    stored_press_y = mouse_y;
                }
                if (e.button.button == SDL_BUTTON_RIGHT && !left_mouse_down) { //will only activate when left mouse button isn't pressed.
                    right_mouse_down = true;
                    right_click_this_frame = true;
                    if (shift_pressed) {
                        std::cout << "shift_right_click at "; //auto flag when possible

                        // Find which slot was clicked and handle flag chording
                        for (int i = 0; i < num_slots; i++) {
                            if (point_in_rect(mouse_x, mouse_y, &rect_array[i])) {
                                handle_shift_right_click(i, board_data, board_cols);
                                break;  // Found the slot, no need to check others
                            }
                        }
                    }
                    else{
                        std::cout << "right_click at "; //flag normally
                    }
                    std::cout << "(" << mouse_x << ", " << mouse_y << ")"; //co-ords printing
                    std::cout << ", ";


                    
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                mouse_x = e.button.x;
                mouse_y = e.button.y;
                //here, i don't care about shift pressing or even right click. the only action that should be reversible is the stawndard left click.
                if (e.button.button == SDL_BUTTON_LEFT && left_mouse_down){
                    left_mouse_down = false;
                    std::cout << "left_click_release at "; //left click release - this is used for the main slot opening instead of left click press.
                    std::cout << "(" << mouse_x << ", " << mouse_y << ")";
                    SDL_Rect area_check = create_rect(stored_press_x - 20, stored_press_y - 20, 40, 40); //temporary line, these rects will be predetermined
                    if (point_in_rect(mouse_x, mouse_y, &area_check)){
                        std::cout << "." << std::endl;
                       }
                    else{
                        // Cancelled move detection goes here if needed
                        std::cout << ". Cancel Detected!" << std::endl;
                    }
                    stored_press_x = -1; //reset to invalid position
                    stored_press_y = -1;
                }
                if (e.button.button == SDL_BUTTON_RIGHT && right_mouse_down){
                    right_mouse_down = false;
                    std::cout << "right_click_release at "; //left click release - this is used for the main slot opening instead of left click press.
                    std::cout << "(" << mouse_x << ", " << mouse_y << ")";
                    std::cout << "." << std::endl;

                }
            }
        }

       //time calculations go here

       if (timer_started && game_still_going){
            Uint64 current_time = SDL_GetPerformanceCounter() - game_start_time;
            current_minutes = (int)(current_time/(Perf_Freq*60));
            current_seconds = (int)((current_time/Perf_Freq)%60);
            current_centiseconds = (int)((current_time/(Perf_Freq/100))%100);
       }

       //mine count display logic here:
       remaining_mines = mine_count - calc_active_flags(board_data);


       //all rendering goes here

       SDL_SetRenderDrawColor(Renderer,150,160,170,255); // background color
       SDL_RenderClear(Renderer);

       SDL_Rect timer_bg = calc_timer_bg_position(sidebar,margin,margin);
       SDL_Rect timer_icon = calc_timer_icon_position(timer_bg);
       SDL_Rect minecount_bg = calc_mine_count_bg_position(sidebar,margin,margin,timer_bg.y + timer_bg.h);
       SDL_Rect minecount_icon = calc_mine_icon_position(minecount_bg);
       SDL_Rect controls_display = create_rect_with_aspect_ratio(minecount_icon.x,minecount_icon.y + minecount_icon.h,1.5f*minecount_bg.w,"assets/ui/controls.png");
       int digit_width = slotsize*0.4;


       render_timer(current_minutes,current_seconds,current_centiseconds,timer_bg,timer_icon,digit_width,ui_tex,Renderer);
       render_minecount(remaining_mines,minecount_bg,minecount_icon,digit_width,ui_tex,Renderer);
       RenderImage(ui_tex['i'],&controls_display,Renderer);


       for (int i = 0; i < num_slots; i++) {
           int x = i % board_cols;
           int y = i / board_cols;

           if (game_still_going == true){
            //i'm still surprised how much bullcrap one can hide by simply not rendering it when one variable happens
            RenderSlotInRect(get_slot_value(x,y,board_data,board_cols),
            calc_visible_state(&rect_array[i],
                x,y,board_data,board_cols,mouse_x,mouse_y, left_mouse_down,right_click_this_frame),
                 &rect_array[i],slot_tex,Renderer);

           }
           else{ //we still render the board at the ending bit.

            RenderSlotInRect(get_slot_value(x,y,board_data,board_cols),get_visible_state(x,y,board_data,board_cols),
                 &rect_array[i],slot_tex,Renderer);

           }
           

           if(get_slot_value(x,y,board_data,board_cols) == 'x'){ //detect loss
            lose = true;
            temp_game_still_going = false; //i'll let the loop finish to render everything one last time - that's why the temp exists.
           }
           if(detect_win(board_data)){
            win = true;
            temp_game_still_going = false;
           }

           


       }
       game_still_going = temp_game_still_going;

       SDL_RenderPresent(Renderer);
       SDL_Delay(16);

       // all rendering ends here

       if (game_still_going == false && (win == true || lose == true)){

            //to really have that old school feel to it, i'm using a message box popup.

            SDL_MessageBoxButtonData new_game_buttons[] = { 
                { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT , 0, "New Game" },
                { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT , 1, "Quit" }
            };

            const char* message_win = "You win :) Start a new game?";
            const char* message_lose = "Game over :( Start a new game?";

            const char* message_to_display = message_win;
            if (lose){
                message_to_display = message_lose;
            }
            else if (win){
                message_to_display = message_win;
            }

            SDL_MessageBoxData new_game_box_data = {
                SDL_MESSAGEBOX_INFORMATION  , Window,"Minesweeper",message_to_display,2,new_game_buttons,NULL
            };

          int mbox_state = 0;
          SDL_ShowMessageBox(&new_game_box_data,&mbox_state);
          if (mbox_state == 1){
            running = false;
          }
          else if(mbox_state == 0){
            
            reset_everything(game_still_going,temp_game_still_going,win,lose,game_start_time,
                timer_started,current_minutes,current_seconds,
                current_centiseconds,first_click_of_the_game,board_data,remaining_mines,mouse_x,mouse_y,left_mouse_down,right_mouse_down,
                right_click_this_frame,stored_press_x,stored_press_y,board_rows,board_cols,mine_count);
            }

       }

    }
    #pragma endregion
    
    for (auto& entry : slot_tex) { //destroys all textures in the map
    SDL_DestroyTexture(entry.second);
    }
    for (auto& entry : ui_tex) { //destroys all textures in the ui
    SDL_DestroyTexture(entry.second);
    }
    SDL_DestroyRenderer(Renderer); Renderer = NULL;
    SDL_DestroyWindow(Window); Window = NULL;
    SDL_Quit();
    return 0;

}
