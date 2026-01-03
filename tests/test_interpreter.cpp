#include "cpu.h"
#include "interpreter.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== RISC-V Interpreter Test ===" << std::endl;
    
    CPU cpu;
    Interpreter interp(cpu);
    
    // Test program:
    // ADDI x1, x0, 10   # x1 = 10
    // ADDI x2, x0, 20   # x2 = 20
    // ADD  x3, x1, x2   # x3 = x1 + x2 = 30
    
    std::vector<uint32_t> program = {
        0x00A00093,  // ADDI x1, x0, 10
        0x01400113,  // ADDI x2, x0, 20
        0x002081B3   // ADD  x3, x1, x2
    };
    
    // Load into memory as bytes
    std::vector<uint8_t> program_bytes;
    for (uint32_t inst : program) {
        program_bytes.push_back(inst & 0xFF);
        program_bytes.push_back((inst >> 8) & 0xFF);
        program_bytes.push_back((inst >> 16) & 0xFF);
        program_bytes.push_back((inst >> 24) & 0xFF);
    }
    
    cpu.load_program(program_bytes, 0x1000);
    
    // Execute 3 instructions
    for (int i = 0; i < 3; i++) {
        std::cout << "\n--- Instruction " << (i+1) << " ---" << std::endl;
        std::cout << "PC: 0x" << std::hex << cpu.get_pc() << std::dec << std::endl;
        interp.step();
        std::cout << "x1 = " << cpu.get_register(1) << std::endl;
        std::cout << "x2 = " << cpu.get_register(2) << std::endl;
        std::cout << "x3 = " << cpu.get_register(3) << std::endl;
    }
    
    std::cout << "\n=== Final Register State ===" << std::endl;
    cpu.dump_registers();
    
    std::cout << "\nInstructions executed: " << interp.get_instructions_executed() << std::endl;
    
    return 0;
}