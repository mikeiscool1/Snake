#include "game.hpp"
#include <SDL2/SDL.h>
#include "constants.hpp"
#include "iostream"

using namespace Window;

Game::Game(): 
  is_running(false),
  game_over(true),
  snake({ { (rows / 2 - 1) * tile_size, (rows / 2 - 1) * tile_size, tile_size, tile_size } }), 
  direction(Direction::RIGHT)
 {}

int Game::init(const char* title, int x, int y, int w, int h) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    
    return 1;
  }

  window = SDL_CreateWindow(title, x, y, w, h, SDL_WINDOW_SHOWN);

  if (!window) {
    SDL_Log("Window creation failed! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();

    return 1;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    SDL_Log("Renderer creation failed! SDL_Error: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 1;
  }

  SDL_SetRenderDrawColor(renderer, Colors_white);

  SDL_RenderClear(renderer);

  renderBackground();
  renderSnake();

  SDL_RenderPresent(renderer);

  is_running = true;
  return 0;
}

void Game::handleEvents() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        is_running = false;
        running();
        break;

      case SDL_KEYDOWN:
        if (pending_directions.size() > 4) break;

        switch (event.key.keysym.sym) {
          case SDLK_UP: case SDLK_w:
            if (!game_over && direction != Direction::DOWN) {
              direction = Direction::UP;
              pending_directions.push_back(Direction::UP);
            }
            break;
          case SDLK_DOWN: case SDLK_s:
            if (!game_over && direction != Direction::UP) {
              direction = Direction::DOWN;
              pending_directions.push_back(Direction::DOWN);
            }
            break;
          case SDLK_RIGHT: case SDLK_d:
            if (!game_over && direction != Direction::LEFT) {
              direction = Direction::RIGHT;
              pending_directions.push_back(Direction::RIGHT);
            }
            break;
          case SDLK_LEFT: case SDLK_a:
            if (!game_over && direction != Direction::RIGHT)  {
              direction = Direction::LEFT;
              pending_directions.push_back(Direction::LEFT);
            }
            break;
          case SDLK_SPACE:
            if (game_over) {
              snake = { { (rows / 2 - 1) * tile_size, (rows / 2 - 1) * tile_size, tile_size, tile_size } };
              direction = Direction::RIGHT;
              pending_directions = {};
              updateFood();

              game_over = false;
            }

            break;
        }

        break;
    }
  }
}

void Game::update() {
  if (!game_over) updateSnake();
}

void Game::render() {
  if (game_over) return;

  SDL_RenderClear(renderer);

  renderBackground();
  renderFood();
  renderSnake();

  SDL_RenderPresent(renderer);
}

void Game::clean() {
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
}

void Game::renderBackground() {
  // used to keep track of the current tile to tell if the next one should be dark or light.
  bool isLight = true;

  SDL_SetRenderDrawColor(renderer, Colors_white);

  for (int row = 0; row < rows; row++) {
    for (int column = 0; column < rows; column++) {
      SDL_Rect tile = { row * tile_size, column * tile_size, tile_size, tile_size };
      SDL_RenderFillRect(renderer, &tile);

      // The beginning of a new row is the same as the end of the previous row.
      // So, only update if it's not the end of the row.
      if (column != rows - 1) {
        if (isLight) SDL_SetRenderDrawColor(renderer, Colors_black);
        else SDL_SetRenderDrawColor(renderer, Colors_white);

        isLight = !isLight;
      }
    }
  }
}

void Game::updateSnake() {
  SDL_Rect& head = snake.back();

  SDL_Rect tail_copy = snake.front();
  SDL_Rect tail = snake.front();

  if (!pending_directions.empty()) {
    direction = pending_directions.front();
    pending_directions.pop_front();
  }

  tail = head;
  
  switch (direction) {
    case Direction::UP:
      tail.y -= tile_size;
      break;
    case Direction::DOWN:
      tail.y += tile_size;
      break;
    case Direction::RIGHT:
      tail.x += tile_size;
      break;
    case Direction::LEFT:
      tail.x -= tile_size;
      break;
  }

  snake.pop_front();
  snake.push_back(tail);

  // check game over
  if (snake.size() == rows * rows) {
    game_over = true;
    return;
  }

  // let tail be the new head
  //check for touching boundaries
  if (tail.x < 0 || tail.x + tile_size > Window::length || tail.y < 0 || tail.y + tile_size > Window::length)
    game_over = true;
  // check for snake touching tail
  else {
    for (int i = 0; i < snake.size() - 1; i++) {
      SDL_Rect& rect = snake[i];

      if (rect.x == tail.x && rect.y == tail.y) {
        game_over = true;
        break;
      }
    }
  }

  if (game_over) return;

  // check if food was acquired
  if (head.x == food.x && head.y == food.y) {
    snake.push_front(tail_copy);
    updateFood();
  }
}

void Game::renderSnake() {
  SDL_SetRenderDrawColor(renderer, Colors_snake);
  for (const SDL_Rect& rect : snake) {
    SDL_RenderFillRect(renderer, &rect);
  }
}

void Game::updateFood() {
  srand(time(0));

  while (true) {
    int random_row = (rand() % rows) * tile_size;
    int random_col = (rand() % rows) * tile_size;

    // You may think, worst case scenario, there is a .039% (1/256 or (1/amount of tiles) chance of the only valid square being chosen.
    // However, this chance means nothing compared to the speed of C++.
    // In fact, you can lower the chance required to break out of a loop to something MUCH lower and it would remain instant
    // And considering this function isn't called every frame, it doesn't really matter.
    bool available = true;
    for (const SDL_Rect& rect : snake)
      if (rect.x == random_row && rect.y == random_col) {
        available = false;
        break;
      }

    if (!available) continue;
    food = { random_row, random_col, tile_size, tile_size };
    break;
  }
}

void Game::renderFood() {
  SDL_SetRenderDrawColor(renderer, Colors_food);
  SDL_RenderFillRect(renderer, &food);
}