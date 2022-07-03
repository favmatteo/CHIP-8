#include <iostream>
#include <fstream>

#include "Chip8.hpp"
#include "SDL2/SDL_mixer.h"

Chip8::Chip8() : m_PC {START_ADDRESS}
               , mt {randomGenerator()}
			   , m_chunk { Mix_LoadWAV("audio/beep.wav"), Mix_FreeChunk }
{
	// load fonts
	for(auto i = 0; i < FONT_ELEMENT_SIZE; ++i){
		m_memory[FONTSET_START_ADDRESS + i] = FONTSET[i];
	}

	// load sounds
	if(!m_chunk){
		std::cout << "Sound could not be loaded" << std::endl;
	}
	Mix_VolumeChunk(m_chunk.get(), MIX_MAX_VOLUME / 2);
}

// Load game to memory (from 0x200)
void Chip8::loadGame(const std::string& path) {
	std::ifstream game {path, std::ios::in | std::ios::binary | std::ios::ate};

	if(game.is_open()){
		// Get size of game
		std::streampos size = game.tellg();
		char* buffer { new char[size]};

		// Go back to beggining of the file
		game.seekg(0, std::ios::beg);
		game.read(buffer, size);
		game.close();

		// copy buffer to memory
		for(long i = 0; i < size; ++i){
			m_memory[START_ADDRESS + i] = buffer[i];
		}

		delete[] buffer;
	}
}

void Chip8::emulateCycle() {
	// Fetch Opcode
	m_opcode = (m_memory[m_PC] << 8) | m_memory[m_PC + 1];

	m_PC += 2;

	// Decode and execute Opcode
	fromOpcodeToFunction();

	// Update timers
	if(m_delayTimer > 0) m_delayTimer--;

	if(m_soundTimer > 0) {
		if (m_soundTimer == 1) Mix_PlayChannel(-1, m_chunk.get(), 0);
		m_soundTimer--;
	}

}

// Call the correct function according to OPCODE
void Chip8::fromOpcodeToFunction() {
	uint8_t firstHexDigit = (m_opcode & 0xF000) >> 12;
	uint8_t lastHexDigit = (m_opcode & 0x000F);
	uint8_t lastTwoHexDigit = (m_opcode & 0x00FF);
	switch (firstHexDigit) {
		case 0x1: OPCODE_1nnn(); break;
		case 0x2: OPCODE_2nnn(); break;
		case 0x3: OPCODE_3xkk(); break;
		case 0x4: OPCODE_4xkk(); break;
		case 0x5: OPCODE_5xy0(); break;
		case 0x6: OPCODE_6xkk(); break;
		case 0x7: OPCODE_7xkk(); break;
		case 0x9: OPCODE_9xy0(); break;
		case 0xA: OPCODE_Annn(); break;
		case 0xB: OPCODE_Bnnn(); break;
		case 0xC: OPCODE_Cxkk(); break;
		case 0xD: OPCODE_Dxyn(); break;
		case 0x8:
			switch (lastHexDigit) {
				case 0x0: OPCODE_8xy0(); break;
				case 0x1: OPCODE_8xy1(); break;
				case 0x2: OPCODE_8xy2(); break;
				case 0x3: OPCODE_8xy3(); break;
				case 0x4: OPCODE_8xy4(); break;
				case 0x5: OPCODE_8xy5(); break;
				case 0x6: OPCODE_8xy6(); break;
				case 0x7: OPCODE_8xy7(); break;
				case 0xE: OPCODE_8xyE(); break;
				default: OPCODE_INVALID();
			} break;
		case 0x0:
			switch(lastHexDigit){
				case 0x0: OPCODE_00E0(); break;
				case 0xE: OPCODE_00EE(); break;
				default: OPCODE_INVALID();
			} break;
		case 0xE:
		case 0xF:
			switch (lastTwoHexDigit){
				case 0xA1: OPCODE_ExA1(); break;
				case 0x9E: OPCODE_Ex9E(); break;
				case 0x07: OPCODE_Fx07(); break;
				case 0x0A: OPCODE_Fx0A(); break;
				case 0x15: OPCODE_Fx15(); break;
				case 0x18: OPCODE_Fx18(); break;
				case 0x1E: OPCODE_Fx1E(); break;
				case 0x29: OPCODE_Fx29(); break;
				case 0x33: OPCODE_Fx33(); break;
				case 0x55: OPCODE_Fx55(); break;
				case 0x65: OPCODE_Fx65(); break;
				default: OPCODE_INVALID();
			}
			break;
		default: OPCODE_INVALID();
	}
}

// Clear the display
void Chip8::OPCODE_00E0() {
	m_graphics.fill(0);
}

//  The interpreter sets the program counter to the address at the top of the stack,
//  then subtracts 1 from the stack pointer.
void Chip8::OPCODE_00EE() {
	m_SP--;
	m_PC = m_stack[m_SP];
}

// Jump to location nnn.
void Chip8::OPCODE_1nnn() {
	uint16_t nnn = m_opcode & 0x0FFF;
	m_PC = nnn;
}

//  The interpreter increments the stack pointer, then puts the current PC on the top of the stack.
//  The PC is then set to nnn.
void Chip8::OPCODE_2nnn() {
	uint16_t nnn = m_opcode & 0x0FFF;
	m_stack[m_SP] = m_PC;
	m_SP++;
	m_PC = nnn;
}

//  The interpreter compares register Vx to kk, and if they are equal,
//  increments the program counter by 2.
void Chip8::OPCODE_3xkk() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t kk = m_opcode & 0x00FF;
	if(m_registers[Vx] == kk) m_PC += 2;

}

//  The interpreter compares register Vx to kk, and if they are not equal,
//  increments the program counter by 2.
void Chip8::OPCODE_4xkk() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t kk = m_opcode & 0x00FF;
	if(m_registers[Vx] != kk) m_PC += 2;
}

//  The interpreter compares register Vx to register Vy, and if they are equal,
//  increments the program counter by 2.
void Chip8::OPCODE_5xy0() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	if(m_registers[Vx] == m_registers[Vy]) m_PC += 2;
}

// The interpreter puts the value kk into register Vx.
void Chip8::OPCODE_6xkk() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t kk = m_opcode & 0x00FF;
	m_registers[Vx] = kk;
}

//  Adds the value kk to the value of register Vx, then stores the result in Vx.
void Chip8::OPCODE_7xkk() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t kk = m_opcode & 0x00FF;
	m_registers[Vx] = m_registers[Vx] + kk;
}

// Stores the value of register Vy in register Vx.
void Chip8::OPCODE_8xy0() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	m_registers[Vx] = m_registers[Vy];
}

// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
void Chip8::OPCODE_8xy1() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	m_registers[Vx] = m_registers[Vx] | m_registers[Vy];
}

// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
void Chip8::OPCODE_8xy2() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	m_registers[Vx] = m_registers[Vx] & m_registers[Vy];
}

// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
void Chip8::OPCODE_8xy3() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	m_registers[Vx] = m_registers[Vx] ^ m_registers[Vy];
}

// Set Vx = Vx + Vy, set VF = carry.
// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,)
// VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
void Chip8::OPCODE_8xy4() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	uint16_t sum = m_registers[Vx] + m_registers[Vy];
	m_registers[0xF] = (sum > 255 ? 1 : 0);
	m_registers[Vx] = sum & 0xFF;
}

// Set Vx = Vx - Vy, set VF = NOT borrow.
// If Vx > Vy, then VF is set to 1, otherwise 0.
// Then Vy is subtracted from Vx, and the results stored in Vx.
void Chip8::OPCODE_8xy5() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	uint16_t sub = m_registers[Vx] - m_registers[Vy];
	m_registers[0xF] = (m_registers[Vx] > m_registers[Vy] ? 1 : 0);
	m_registers[Vx] = sub;
}

// Set Vx = Vx SHR 1.
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void Chip8::OPCODE_8xy6() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	m_registers[0xF] = (m_registers[Vx] & 0x1);
	m_registers[Vx] = m_registers[Vx] >> 1;
}

// Set Vx = Vy - Vx, set VF = NOT borrow.
// If Vy > Vx, then VF is set to 1, otherwise 0.
// Then Vx is subtracted from Vy, and the results stored in Vx.
void Chip8::OPCODE_8xy7() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	m_registers[0xF] = (m_registers[Vy] > m_registers[Vx] ? 1 : 0);
	m_registers[Vx] = m_registers[Vy] - m_registers[Vx];
}

// Set Vx = Vx SHL 1.
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
// Then Vx is multiplied by 2.
void Chip8::OPCODE_8xyE() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	m_registers[0xF] = (m_registers[Vx] & 0x80) >> 7;
	m_registers[Vx] = m_registers[Vx] << 1;
}

// Skip next instruction if Vx != Vy.
// The values of Vx and Vy are compared, and if they are not equal,
// the program counter is increased by 2.
void Chip8::OPCODE_9xy0() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	if(m_registers[Vx] != m_registers[Vy]) m_PC += 2;
}

// The value of register I is set to nnn.
void Chip8::OPCODE_Annn() {
	uint16_t nnn = m_opcode & 0x0FFF;
	m_RI = nnn;
}

// Jump to location nnn + V0.
// The program counter is set to nnn plus the value of V0.
void Chip8::OPCODE_Bnnn() {
	uint16_t nnn = m_opcode & 0x0FFF;
	m_PC = nnn + m_registers[0];
}

// Set Vx = random byte AND kk.
// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
// The results are stored in Vx. See instruction 8xy2 for more information on AND.
void Chip8::OPCODE_Cxkk() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t kk = m_opcode & 0x00FF;
	uint8_t random = dist(mt);
	m_registers[Vx] = (random & kk);
}

// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
// The interpreter reads n bytes from memory, starting at the address stored in I.
// These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
// Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1,
// otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of
// the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more
// information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
void Chip8::OPCODE_Dxyn() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t Vy = (m_opcode & 0x00F0) >> 4;
	uint8_t n = m_opcode & 0x000F;

	uint8_t xPos = m_registers[Vx] % DISPLAY_WIDTH;
	uint8_t yPos = m_registers[Vy] % DISPLAY_HEIGHT;

	m_registers[0xF] = 0;

	for(uint row {}; row < n; ++row){
		uint8_t spriteByte = m_memory[m_RI + row];
		for(uint col {}; col < 8; ++col){
			uint8_t spritePixel = spriteByte & (0x80 >> col);
			uint32_t& screenPixel = m_graphics[(yPos + row) * DISPLAY_WIDTH + (xPos + col)];
			if(spritePixel) {
				if (screenPixel == 0xFFFFFFFF) m_registers[0xF] = 1;
				screenPixel = screenPixel ^ 0xFFFFFFFF;
			}
		}
	}
}

// Skip next instruction if key with the value of Vx is pressed.
// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position,
// PC is increased by 2.
void Chip8::OPCODE_Ex9E() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t key = m_registers[Vx];
	if(m_keypad[key]) m_PC += 2;
}

// Skip next instruction if key with the value of Vx is not pressed.
void Chip8::OPCODE_ExA1() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t key = m_registers[Vx];
	if(!m_keypad[key]) m_PC += 2;
}

// Set Vx = delay timer value.
void Chip8::OPCODE_Fx07() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	m_registers[Vx] = m_delayTimer;
}

// Wait for a key press, store the value of the key in Vx.
// All execution stops until a key is pressed, then the value of that key is stored in Vx.
void Chip8::OPCODE_Fx0A() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	bool keyPressed {false};
	for(size_t i = 0; i < m_keypad.size(); ++i){
		if(m_keypad[i]){
			m_registers[Vx] = i;
			keyPressed = true;
			break;
		}
	}
	if(!keyPressed) {
		m_PC -= 2;
	}
}

// Set delay timer = Vx
void Chip8::OPCODE_Fx15() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	m_delayTimer = m_registers[Vx];
}

// Set sound timer = Vx.
void Chip8::OPCODE_Fx18() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	m_soundTimer = m_registers[Vx];
}

// Set I = I + Vx.
void Chip8::OPCODE_Fx1E() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	m_RI = m_RI + m_registers[Vx];
}

// Set I = location of sprite for digit Vx.
// The value of I is set to the location for the hexadecimal sprite corresponding
// to the value of Vx.
void Chip8::OPCODE_Fx29() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t digit = m_registers[Vx];
	m_RI = FONTSET_START_ADDRESS + (5 * digit);
}

// Store BCD representation of Vx in memory locations I, I+1, and I+2.
// The interpreter takes the decimal value of Vx, and places the hundreds digit
// in memory at location in I, the tens digit at location I+1, and the ones digit
// at location I+2.
void Chip8::OPCODE_Fx33() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	uint8_t value = m_registers[Vx];
	for(int i = 2; i >= 0; --i){
		m_memory[m_RI+i] = value % 10;
		value /= 10;
	}
}

// Store registers V0 through Vx in memory starting at location I.
// The interpreter copies the values of registers V0 through Vx into memory,
// starting at the address in I.
void Chip8::OPCODE_Fx55() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	for(uint8_t i = 0; i <= Vx; ++i){
		m_memory[m_RI + i] = m_registers[i];
	}
}

// Read registers V0 through Vx from memory starting at location I.
// The interpreter reads values from memory starting at location I
// into registers V0 through Vx.
void Chip8::OPCODE_Fx65() {
	uint8_t Vx = (m_opcode & 0x0F00) >> 8;
	for(uint8_t i = 0; i <= Vx; ++i){
		m_registers[i] = m_memory[m_RI + i];
	}
}

// Invalid OPCODE it does nothing
void Chip8::OPCODE_INVALID() {}

const std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT>& Chip8::getGraphics() const {
	return m_graphics;
}

const std::array<uint8_t, CHAR>& Chip8::getKeypad() const {
	return m_keypad;
}