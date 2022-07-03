//
// Created by matteo on 02/07/22.
//

#pragma once

#include <string>

#include "SDL2/SDL.h"

const std::array SDL_KEY_TYPES {SDLK_ESCAPE, SDLK_x, SDLK_1, SDLK_2, SDLK_3,
								SDLK_q, SDLK_w, SDLK_e, SDLK_a, SDLK_s, SDLK_d,
								SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_r, SDLK_f,
								SDLK_v};

class Platform
{
  public:
	Platform(const std::string& name, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
	~Platform();

	void update(void const* buffer, int pitch);
	bool processInput(std::array<uint8_t, 16>& keys);

  private:
	SDL_Window* window {};
	SDL_Renderer* renderer {};
	SDL_Texture* texture {};

};

