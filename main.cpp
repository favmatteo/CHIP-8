#include <iostream>
#include <chrono>

#include "Chip8.hpp"
#include "Platform.hpp"

int main() {
	Chip8 chip {};
	int videoScale = 10;
	int cycleDelay = 5;
	const std::string path = "rom/tetris.ch8";

	Platform platform {"Chip-8 Emulator", DISPLAY_WIDTH * videoScale, DISPLAY_HEIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HEIGHT};

	chip.loadGame(path);

	auto& keyboards = const_cast<std::array<uint8_t, 16>&>(chip.getKeypad());
	auto& graphics = chip.getGraphics();

	int videoPitch = sizeof(graphics[0]) * DISPLAY_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while(!quit)
	{
		quit = platform.processInput(keyboards);
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip.emulateCycle();

			platform.update(&graphics[0], videoPitch);
		}

	}

	return 0;
}
