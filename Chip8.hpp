#pragma once

#include <iostream>
#include <array>
#include <memory>
#include <random>

#include "SDL2/SDL_mixer.h"

constexpr uint16_t START_ADDRESS {0x200};
constexpr uint16_t FONTSET_START_ADDRESS {0x50};
constexpr uint8_t CHAR {16};
constexpr uint8_t BYTE_IN_CHAR {5};
constexpr uint8_t FONT_ELEMENT_SIZE {CHAR * BYTE_IN_CHAR}; // 16 character represented by 5 bytes each
constexpr uint8_t DISPLAY_WIDTH {64};
constexpr uint8_t DISPLAY_HEIGHT {32};
constexpr uint16_t MEMORY_SIZE {4096};
constexpr uint8_t REGISTERS {16};
constexpr uint8_t STACK_DEPTH {16};
constexpr std::array<uint8_t, FONT_ELEMENT_SIZE> FONTSET {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8 {
  public:
	Chip8();

	void loadGame(const std::string& path);
	void emulateCycle();
	void fromOpcodeToFunction();

	[[nodiscard]] const std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT>& getGraphics() const;
	[[nodiscard]] const std::array<uint8_t, CHAR>& getKeypad() const;

  private:
	// Memory
	uint16_t m_opcode {}; // 35 OPCODE_INVALID
	std::array<uint8_t, MEMORY_SIZE> m_memory {}; // 4096 byte
	std::array<uint8_t, REGISTERS> m_registers {}; // 16 Registers (V0-VF) 8bit
	uint16_t m_RI {}; // Index Register used to store memory addresses for use in operations
	uint16_t m_PC {}; // Program counter, register that contains the address of the next instruction.

	// Graphics
	std::array<uint32_t, DISPLAY_WIDTH * DISPLAY_HEIGHT> m_graphics {}; // 64 pixel wide x 32 pixel high

	// Timer
	uint8_t m_delayTimer {}; // 60 to 0
	uint8_t m_soundTimer {}; // 60 to 0

	// Stack
	std::array<uint16_t, STACK_DEPTH> m_stack {}; // stack that contains addresses before a jump to another function
	uint16_t m_SP {}; // sp is the pointer to the right level of stack

	// Keypad
	std::array<uint8_t, CHAR> m_keypad {}; // key from 0 to F

	// Random
	std::random_device randomGenerator;
	std::mt19937 mt;
	std::uniform_int_distribution<int> dist {0, 255};

	// Audio
	std::unique_ptr<Mix_Chunk, void (*)(Mix_Chunk *)> m_chunk;

	// OPCODE Implementations http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
	void OPCODE_00E0(); // CLS
	void OPCODE_00EE(); // RET
	void OPCODE_1nnn(); // JP addr
	void OPCODE_2nnn(); // CALL addr
	void OPCODE_3xkk(); // SE Vx, byte
	void OPCODE_4xkk(); // SNE Vx, byte
	void OPCODE_5xy0(); // SE Vx, Vy
	void OPCODE_6xkk(); // LD Vx, byte
	void OPCODE_7xkk(); // ADD Vx, byte
	void OPCODE_8xy0(); // LD Vx, Vy
	void OPCODE_8xy1(); // OR Vx, Vy
	void OPCODE_8xy2(); // AND Vx, Vy
	void OPCODE_8xy3(); // XOR Vx, Vy
	void OPCODE_8xy4(); // ADD Vx, Vy
	void OPCODE_8xy5(); // SUB Vx, Vy
	void OPCODE_8xy6(); // SHR Vx {, Vy}
	void OPCODE_8xy7(); // SUBN Vx, Vy
	void OPCODE_8xyE(); // SHL Vx {, Vy}
	void OPCODE_9xy0(); // SNE Vx, Vy
	void OPCODE_Annn(); // LD I, addr
	void OPCODE_Bnnn(); // JP V0, addr
	void OPCODE_Cxkk(); // RND Vx, byte
	void OPCODE_Dxyn(); // DRW Vx, Vy, nibble
	void OPCODE_Ex9E(); // SKP Vx
	void OPCODE_ExA1(); // SKNP Vx
	void OPCODE_Fx07(); // LD Vx, DT
	void OPCODE_Fx0A(); // LD Vx, K
	void OPCODE_Fx15(); // LD DT, Vx
	void OPCODE_Fx18(); // LD ST, Vx
	void OPCODE_Fx1E(); // ADD I, Vx
	void OPCODE_Fx29(); // LD F, Vx
	void OPCODE_Fx33(); // LD B, Vx
	void OPCODE_Fx55(); // LD [I], Vx
	void OPCODE_Fx65(); // LD Vx, [I]
	void OPCODE_INVALID(); // Invalid OPCODE
};