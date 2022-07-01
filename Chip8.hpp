//
// Created by matteo on 29/06/22.
//

#ifndef CHIP8__CHIP8_HPP_
#define CHIP8__CHIP8_HPP_

#include <iostream>
#include <array>
#include <random>

constexpr uint16_t START_ADDRESS = 0x200;
constexpr uint16_t FONTSET_START_ADDRESS = 0x50;
constexpr uint8_t CHAR = 16;
constexpr uint8_t BYTE_IN_CHAR = 5;
constexpr uint8_t FONT_ELEMENT_SIZE = CHAR * BYTE_IN_CHAR; // 16 character rappresented by 5 bytes each
constexpr uint8_t DISPLAY_WIDTH = 64;
constexpr uint8_t DISPLAY_HEIGHT = 32;

// FONT
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

  private:
	// Memory
	uint16_t m_opcode {}; // 35 opcode
	std::array<uint8_t, 4096> m_memory {}; // 4096 byte
	std::array<uint8_t, 16> m_registers {}; // 16 Registers (V0-VF) 8bit
	uint16_t m_RI {}; // 16 bit (12 was ok), Index Register used to store memory addresses for use in operations
	uint16_t m_PC {}; // 16 bit (12 was ok), register that contains the address of the next instruction.

	// Graphics
	std::array<uint32_t, 64 * 32> m_graphics {}; // 64 pixel wide x 32 pixel high

	// Timer
	uint8_t m_delayTimer {}; // 60Hz to 0
	uint8_t m_soundTimer {}; // 60Hz to 0

	// Stack
	// TODO: forse meglio sostituire con std::stack
	std::array<uint16_t, 16> m_stack {}; // stack that contains addresses before a jump to another function
	uint16_t m_SP {}; // sp is the pointer to the right level of stack

	// Keypad
	std::array<uint16_t, 16> m_keypad {}; // key from 0 to F

	// Random
	std::random_device randomGenerator;
	std::mt19937 mt;
	std::uniform_int_distribution<int> dist {0, 255};

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

#endif //CHIP8__CHIP8_HPP_

// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
// 0x200-0xFFF - Program ROM and work RAM