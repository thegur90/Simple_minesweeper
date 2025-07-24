
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "util_renderimage.h"
#include "util_aspectratio.h"


SDL_Texture* AssignImageToTexture(SDL_Renderer* screenRender,std::string path) {
  //taken and slightly modified from boots
  SDL_Surface* img = IMG_Load(path.c_str());
  if (img == NULL) {
    fprintf(stderr, "could not load image: %s\n", IMG_GetError());
    fflush(stderr);
    return NULL;
  }
  SDL_Texture* imageTex = SDL_CreateTextureFromSurface(screenRender, img);
  SDL_FreeSurface(img);
  return imageTex;
}

SDL_Rect create_rect(int x, int y, int w, int h){
    SDL_Rect e;
    e.x = x;
    e.y = y;
    e.w = w;
    e.h = h;
    return e;
}

SDL_Rect create_rect_with_aspect_ratio(int x, int y, int w, const char* image_path){
    SDL_Rect e;
    e.x = x;
    e.y = y;
    e.w = w;

    //logic to calculate width based on height
    SDL_Surface* temp_surface = IMG_Load(image_path);
    if (temp_surface == NULL) { //error handling
        fprintf(stderr, "Failed to load image for aspect ratio calculation (%s): %s\n", image_path, IMG_GetError());
        e.w = 0; 
        return e;
    }
    // Calculate the aspect ratio
    float aspect_ratio = (float)temp_surface->w / (float)temp_surface->h;
    // Calculate the new width based on the desired height and original aspect ratio
    e.h = (int)(e.w / aspect_ratio);

    SDL_FreeSurface(temp_surface);
    return e;
}

void RenderImage(SDL_Texture* texture, const SDL_Rect* dest, SDL_Renderer* renderer){ //wrapper function because i don't need NULL here ever.
  SDL_RenderCopy(renderer, texture, NULL, dest);
}


/*
these next two functions include very cursed image stretching that scales to the constant slot sizes in util_aspectratio.cpp
in an ideal world, i won't need those since my assets will just....fit better - but it is good for practice purposes :p

*/
void RenderSlotInRect(char num, char status, const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> slot_tex, SDL_Renderer* renderer){ 
  RenderImage(slot_tex.at(status),dest, renderer);

  if (status == 'N' || status == 'H'){
    RenderFlag(dest,slot_tex,renderer);
  }
  int margin = (int)(dest->w * 0.625f);    // 25/40 = 0.625
  int stretch = (int)(dest->w * 1.5f);     // 60/40 = 1.5
  int inset_x = (int)(dest->w * 0.075f);   // 3/40 = 0.075
  
  if (num >= '1' && num <= '8' && status == 'r'){ //yes, you can compare chars like this - the result will be correct for this use case.
    //i'm also checking if status is revealed because i have number values on covered slots which do not need rendering.
    const SDL_Rect num_rect = create_rect(dest->x - margin - inset_x ,dest->y - margin, dest->w + stretch, dest->h + stretch); //picture adjustment for numbers.
    RenderImage(slot_tex.at(num),&num_rect, renderer);
  }
  else if ((num == 'm' || num =='x') && status == 'r'){
    RenderImage(slot_tex.at(num),dest, renderer);
  }
}

void RenderFlag (const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> slot_tex, SDL_Renderer* renderer){ //i render the flags seperately because it's easier to read

  int flag_offset_x = (int)(dest->w * 0.25f);  // 10/40 = 0.25
  int flag_offset_y = (int)(dest->h * 0.05f);  // 2/40 = 0.05
  int flag_inset = (int)(dest->w * 0.5f);      // 20/40 = 0.5
  int flag_inset_y = (int)(dest->h * 0.1f);    // 4/40 = 0.1
   
  const SDL_Rect flag_rect = create_rect(dest->x + flag_offset_x,dest->y + flag_offset_y, dest->w -flag_inset, dest->h - flag_inset_y);
  RenderImage(slot_tex.at('F'),&flag_rect, renderer);

}

void Render_Timer_UI(const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer){
  RenderImage(ui_tex.at('c'),dest, renderer);
}

void Render_MineIcon_UI(const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer){
  RenderImage(ui_tex.at('m'),dest, renderer);
}

void Render_StatBG_UI(const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer){
  RenderImage(ui_tex.at('b'),dest, renderer);
}

void Render_Digit_UI(char digit,const SDL_Rect& dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer){

  int digit_offset_x = (int)(dest.w * 0.1f);
  int digit_offset_y = (int)(dest.h * 0.05f);
  int digit_inset_x = (int)(dest.w * 1.0f);  
  int digit_inset_y = (int)(dest.h * 0.1f);

  const SDL_Rect digit_rect_adjusted  = create_rect(dest.x - digit_offset_x,dest.y - digit_offset_y, dest.w + digit_inset_x, dest.h + digit_inset_y);

  RenderImage(ui_tex.at(digit),&digit_rect_adjusted , renderer);
}

void render_timer_digits(int minutes, int seconds, int centiseconds, SDL_Rect timer_bg, int digit_width,
                 const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer) {
    
    // Render MM:ss:mm format using calc_timer_digit_position. looping this is unfortunately worse.

    
    Render_Digit_UI('0' + (minutes / 10), calc_timer_digit_position(0, timer_bg, digit_width), ui_tex, renderer);  // M
    Render_Digit_UI('0' + (minutes % 10), calc_timer_digit_position(1, timer_bg, digit_width), ui_tex, renderer);  // M
    Render_Digit_UI('C', calc_timer_digit_position(2, timer_bg, digit_width), ui_tex, renderer);                    // :
    Render_Digit_UI('0' + (seconds / 10), calc_timer_digit_position(3, timer_bg, digit_width), ui_tex, renderer);  // s
    Render_Digit_UI('0' + (seconds % 10), calc_timer_digit_position(4, timer_bg, digit_width), ui_tex, renderer);  // s
    Render_Digit_UI('C', calc_timer_digit_position(5, timer_bg, digit_width), ui_tex, renderer);                    // :
    Render_Digit_UI('0' + (centiseconds / 10), calc_timer_digit_position(6, timer_bg, digit_width), ui_tex, renderer); // m
    Render_Digit_UI('0' + (centiseconds % 10), calc_timer_digit_position(7, timer_bg, digit_width), ui_tex, renderer); // m
}

void render_minecount(int mine_count, SDL_Rect minecount_bg, SDL_Rect minecount_icon_pos, int digit_width,
                     const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer) {
    
    Render_StatBG_UI(&minecount_bg, ui_tex, renderer);
    Render_MineIcon_UI(&minecount_icon_pos, ui_tex, renderer);
    
    // Determine how many digits to render
    int num_digits;
    if (mine_count > 99) {
        num_digits = 3;
    } else if (mine_count > 9) {
        num_digits = 2;
    } else {
        num_digits = 1;
    }
    
    // Render digits based on count
    if (num_digits >= 3) {
        Render_Digit_UI('0' + (mine_count / 100), calc_minecount_digit_position(0, minecount_bg, digit_width, mine_count), ui_tex, renderer);
        Render_Digit_UI('0' + ((mine_count / 10) % 10), calc_minecount_digit_position(1, minecount_bg, digit_width, mine_count), ui_tex, renderer);
        Render_Digit_UI('0' + (mine_count % 10), calc_minecount_digit_position(2, minecount_bg, digit_width, mine_count), ui_tex, renderer);
    }
    else if (num_digits >= 2) {
        Render_Digit_UI('0' + ((mine_count / 10) % 10), calc_minecount_digit_position(0, minecount_bg, digit_width, mine_count), ui_tex, renderer);
        Render_Digit_UI('0' + (mine_count % 10), calc_minecount_digit_position(1, minecount_bg, digit_width, mine_count), ui_tex, renderer);
    }
    else{
      Render_Digit_UI('0' + (mine_count % 10), calc_minecount_digit_position(0, minecount_bg, digit_width, mine_count), ui_tex, renderer);
    }
    
}

void render_timer(int minutes, int seconds, int centiseconds, SDL_Rect timer_bg, SDL_Rect timer_icon_pos, int digit_width,
                 const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer){

  Render_StatBG_UI(&timer_bg, ui_tex, renderer);
  Render_Timer_UI(&timer_icon_pos, ui_tex, renderer);
  render_timer_digits(minutes,seconds,centiseconds,timer_bg,digit_width,ui_tex,renderer);

}

void SetMinesweeperWindowIcon(SDL_Window* window){
  const char* icon_path = "assets/slots/flag.png";
  SDL_Surface* icon_surface = IMG_Load(icon_path);
    if (icon_surface == NULL) {
        fprintf(stderr, "Could not load window icon %s: %s\n", icon_path, IMG_GetError());
        return;
    }
    SDL_SetWindowIcon(window, icon_surface);
    SDL_FreeSurface(icon_surface); // Free the surface after setting the icon
}

//using the render digit ui because i only need numbers anyway. no need for a text library!
//this is for the options window.
void RenderNumberFromTextureDigits(int value, SDL_Rect target_bg_rect, int digit_width,
                                   const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer) {

    std::string s = std::to_string(value); // Convert the integer to a string

    // Calculate the starting X position for the number to be centered within target_bg_rect
    // This is similar to calc_minecount_digit_position
    int total_number_width = s.length() * digit_width;
    int start_x = target_bg_rect.x + (target_bg_rect.w - total_number_width) / 2;
    int y = target_bg_rect.y;
    int height = target_bg_rect.h * 0.9f; // small margin to fit the digits in the bg rect

    for (int i = 0; i < s.length(); ++i) {
        char digit_char = s[i]; // Get the character for the current digit

        // Ensure the character is indeed a digit we have a texture for
        if (ui_tex.count(digit_char) > 0) {
            SDL_Rect digit_rect;
            digit_rect.x = start_x + (i * (digit_width/1.7f)); // Position each digit
            digit_rect.y = y;
            digit_rect.w = digit_width*2; //fixes stretchy number
            digit_rect.h = height*1.2f;

            // Render the digit texture
            RenderImage(ui_tex.at(digit_char), &digit_rect, renderer);
        } else {
            fprintf(stderr, "Attempted to render non-digit character '%c' using texture digits!\n", digit_char);
        }
    }
}