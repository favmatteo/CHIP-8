#include <array>

#include "Platform.hpp"

Platform::Platform(std::string_view name, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(name.data(), 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Platform::~Platform() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Platform::update(const void* buffer, int pitch) {
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Platform::processInput(std::array<uint8_t, 16>& keys) {
	bool end = false;

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				end = true;
				break;
			case SDL_KEYDOWN:
				for(int cnt {}; const auto& type : SDL_KEY_TYPES){
					if(event.key.keysym.sym == type){
						keys[cnt] = 1;
						break;
					}
					cnt++;
				}
				break;
			case SDL_KEYUP:
				for(int cnt {}; const auto& type : SDL_KEY_TYPES){
					if(event.key.keysym.sym == type){
						keys[cnt] = 0;
						break;
					}
					cnt++;
				}
				break;
		}
	}
	return end;
}