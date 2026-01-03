#include "../src/core/cpu.h"
#include "../src/jit/jit_compiler.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== RISC-V JIT COMPILER TEST ===" << std::endl;
    
    CPU cpu;
    JITCompiler jit;
    
    std::vector<uint32_t> program = {
        0x00A00093,  // ADDI x1, x0, 10
        0x01400113,  // ADDI x2, x0, 20
        0x002081B3   // ADD  x3, x1, x2
    };
    
    std::vector<uint8_t> program_bytes;
    for (uint32_t inst : program) {
        program_bytes.push_back(inst & 0xFF);
        program_bytes.push_back((inst >> 8) & 0xFF);
        program_bytes.push_back((inst >> 16) & 0xFF);
        program_bytes.push_back((inst >> 24) & 0xFF);
    }
    
    cpu.load_program(program_bytes, 0x1000);
    
    jit.compile_basic_block(cpu, 0x1000);
    
    if (jit.has_compiled_code(0x1000)) {
        CompiledFunc func = jit.get_compiled_code(0x1000);
        func(cpu.get_register_ptr());
        
        std::cout << "\nResults:" << std::endl;
        std::cout << "x1 = " << cpu.get_register(1) << std::endl;
        std::cout << "x2 = " << cpu.get_register(2) << std::endl;
        std::cout << "x3 = " << cpu.get_register(3) << std::endl;
        
        if (cpu.get_register(3) == 30) {
            std::cout << "\n🔥🔥🔥 RISC-V JIT COMPILER WORKS! 🔥🔥🔥" << std::endl;
            std::cout << "You just compiled RISC-V to ARM64 at runtime!" << std::endl;
        }
    }
    
    return 0;
}