#pragma once

namespace Window {
  constexpr int length = 800;
  constexpr int rows = 16;
  constexpr int tile_size = length / rows;
}

namespace FPS {
  constexpr int fps = 8;
  constexpr int frame_delay = 1000 / fps;
}

enum class Direction {
  STILL,
  UP,
  DOWN,
  LEFT,
  RIGHT
};

#define Colors_black 61, 61, 61, 255
#define Colors_white 92, 92, 92, 255
#define Colors_food 255, 0, 0, 255
#define Colors_snake 0, 255, 0, 255
#define Colors_snake_head 0, 200, 0, 255