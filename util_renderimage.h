#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>

SDL_Texture* AssignImageToTexture(SDL_Renderer* screenRender,std::string path);

SDL_Rect create_rect(int x, int y, int w, int h);

SDL_Rect create_rect_with_aspect_ratio(int x, int y, int w, const char* image_path);

void RenderImage(SDL_Texture* texture, const SDL_Rect* dest, SDL_Renderer* renderer);

void RenderSlotInRect(char num, char status, const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> slot_tex, SDL_Renderer* renderer);

void RenderFlag (const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> slot_tex, SDL_Renderer* renderer);

//ui

void Render_MineIcon_UI(const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer);

void Render_Timer_UI(const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer);

void Render_StatBG_UI(const SDL_Rect* dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer);

void Render_Digit_UI(char digit,const SDL_Rect& dest, const std::unordered_map<char, SDL_Texture*> ui_tex, SDL_Renderer* renderer);

void render_timer_digits(int minutes, int seconds, int centiseconds, SDL_Rect timer_bg, int digit_width,
                 const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer);

void render_timer(int minutes, int seconds, int centiseconds, SDL_Rect timer_bg, SDL_Rect timer_icon_pos, int digit_width,
                 const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer);

void render_minecount(int mine_count, SDL_Rect minecount_bg, SDL_Rect minecount_icon_pos, int digit_width,
                     const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer);

void SetMinesweeperWindowIcon(SDL_Window* window);

void RenderNumberFromTextureDigits(int value, SDL_Rect target_bg_rect, int digit_width,
                                   const std::unordered_map<char, SDL_Texture*>& ui_tex, SDL_Renderer* renderer);


