#include <iostream>
#include <chrono>
#include <vector>
#include <utility>
#include <filesystem>

#include "Chip8.hpp"
#include "Platform.hpp"

void loadAudio(){
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		std::cout << "Mixer cannot be initialized" << std::endl;
	}
	Mix_AllocateChannels(32);
}

std::string menu(){
	std::vector<std::pair<std::string, uint16_t>> games {};
	const std::string path = "rom/";
	for(const auto& entry : std::filesystem::directory_iterator(path)){
		if(!entry.is_directory()) {
			games.push_back(std::pair {entry.path(), entry.file_size()});
		}
	}
	std::cout << "Games:" << std::endl;
	int i {1};
	for(const auto& game : games){
		std::cout << i << ") "
				  << game.first << " - size: "
				  << game.second << " byte\n";
		i++;
	}
	std::cout << "What games do you want to emulate?: ";
	int choice {};
	do{
		std::cin >> choice;
		if(choice < 1 or choice > (i - 1)) std::cerr << "Invalid... Enter again: ";
	} while (choice < 1 or choice > (i - 1));

	return games[choice - 1].first;
}

int main() {
	loadAudio();
	Chip8 chip {};
	int videoScale = 10;
	int delay = 5;
	const std::string& path = menu();

	Platform platform {"Chip-8 Emulator", DISPLAY_WIDTH * videoScale, DISPLAY_HEIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HEIGHT};

	chip.loadGame(path);

	auto& keyboards = const_cast<std::array<uint8_t, 16>&>(chip.getKeypad());
	auto& graphics = chip.getGraphics();

	int videoPitch = sizeof(graphics[0]) * DISPLAY_WIDTH;

	auto prev = std::chrono::high_resolution_clock::now();
	bool end = false;

	while(!end)
	{
		end = platform.processInput(keyboards);
		auto now = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(now - prev).count();
		if (dt > static_cast<float>(delay))
		{
			prev = now;
			chip.emulateCycle();
			platform.update(&graphics[0], videoPitch);
		}

	}

	return 0;
}
