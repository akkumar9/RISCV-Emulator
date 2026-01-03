#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <vector>
#include <cstring>

// RISC-V has 32 general-purpose registers
constexpr size_t NUM_REGISTERS = 32;
constexpr size_t MEMORY_SIZE = 128 * 1024 * 1024; // 64MB

// Instruction formats
enum class InstructionType {
    R_TYPE,  // Register: add, sub, and, or, xor, sll, srl, sra
    I_TYPE,  // Immediate: addi, lw, jalr
    S_TYPE,  // Store: sw, sb, sh
    B_TYPE,  // Branch: beq, bne, blt, bge
    U_TYPE,  // Upper immediate: lui, auipc
    J_TYPE   // Jump: jal
};

// Decoded instruction
struct Instruction {
    uint32_t raw;           // Raw 32-bit instruction
    InstructionType type;
    uint8_t opcode;
    uint8_t rd;             // Destination register
    uint8_t rs1;            // Source register 1
    uint8_t rs2;            // Source register 2
    uint8_t funct3;
    uint8_t funct7;
    int32_t imm;            // Immediate value (sign-extended)
};

class CPU {
public:
    CPU() : pc(0), memory(MEMORY_SIZE, 0) {
        // x0 is hardwired to 0
        registers.fill(0);
    }

    // Register access
    uint32_t get_register(uint8_t reg) const {
        return registers[reg];
    }

    void set_register(uint8_t reg, uint32_t value) {
        if (reg != 0) {  // x0 is always 0
            registers[reg] = value;
        }
    }

    // Memory access
    uint32_t read_word(uint32_t addr) const {
        if (addr + 3 >= memory.size()) {
            throw std::runtime_error("Memory read out of bounds");
        }
        uint32_t value;
        std::memcpy(&value, &memory[addr], sizeof(uint32_t));
        return value;
    }

    void write_word(uint32_t addr, uint32_t value) {
        if (addr + 3 >= memory.size()) {
            throw std::runtime_error("Memory write out of bounds");
        }
        std::memcpy(&memory[addr], &value, sizeof(uint32_t));
    }

    uint8_t read_byte(uint32_t addr) const {
        if (addr >= memory.size()) {
            throw std::runtime_error("Memory read out of bounds");
        }
        return memory[addr];
    }

    void write_byte(uint32_t addr, uint8_t value) {
        if (addr >= memory.size()) {
            throw std::runtime_error("Memory write out of bounds");
        }
        memory[addr] = value;
    }

    // Load program into memory
    void load_program(const std::vector<uint8_t>& program, uint32_t start_addr = 0x1000) {
        if (start_addr + program.size() > memory.size()) {
            throw std::runtime_error("Program too large for memory");
        }
        std::memcpy(&memory[start_addr], program.data(), program.size());
        pc = start_addr;
    }

    // Program counter
    uint32_t get_pc() const { return pc; }
    void set_pc(uint32_t new_pc) { pc = new_pc; }
    void increment_pc() { pc += 4; }

    // Debug
    void dump_registers() const;
    void dump_memory(uint32_t start, uint32_t length) const;
    uint32_t* get_register_ptr() {
        return registers.data();
    }
private:
    std::array<uint32_t, NUM_REGISTERS> registers;
    uint32_t pc;  // Program counter
    std::vector<uint8_t> memory;
};

#endif // CPU_H