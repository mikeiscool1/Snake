#pragma once

#include <SDL2/SDL.h>
#include "constants.hpp"
#include <deque>

class Game {
public:
  Game();

  int init(const char* title, int x, int y, int w, int h);

  void handleEvents();
  void update();
  void render();
  void clean();

  void renderBackground();

  void renderSnake();
  void renderFood();
  
  void updateSnake();
  void updateFood();

  inline bool running() { return is_running; };
  inline bool is_over() { return game_over; }
private:
  bool is_running;
  bool game_over;
  SDL_Window *window;
  SDL_Renderer *renderer;

  std::deque<SDL_Rect> snake;
  Direction direction;
  std::deque<Direction> pending_directions;
  
  SDL_Rect food;
};