#include "game.hpp"
#include <iostream>
#include "constants.hpp"

int main() {
  Game game;

  Uint32 frame_start;
  int frame_time;

  int output = game.init("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Window::length, Window::length);
  if (output != 0) return 1;

  while (game.running()) {
    frame_start = SDL_GetTicks();

    game.handleEvents();
    game.update();
    game.render();

    frame_time = SDL_GetTicks() - frame_start;

    if (FPS::frame_delay > frame_time) {
      SDL_Delay(FPS::frame_delay - frame_time);
    }
  }

  game.clean();
 
  return 0;
}