#pragma once

#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"

const std::array SDL_KEY_TYPES {SDLK_ESCAPE, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
								SDLK_q, SDLK_w, SDLK_e, SDLK_r,
								SDLK_a, SDLK_s, SDLK_d, SDLK_f,
								SDLK_z, SDLK_x, SDLK_c, SDLK_v};

class Platform
{
  public:
	Platform(std::string_view name, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~Platform();

	void update(void const* buffer, int pitch);
	bool processInput(std::array<uint8_t, 16>& keys);

  private:
	SDL_Window* window {};
	SDL_Renderer* renderer {};
	SDL_Texture* texture {};

};

