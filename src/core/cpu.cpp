#include "cpu.h"
#include <iostream>
#include <iomanip>

void CPU::dump_registers() const {
    std::cout << "=== Register Dump ===" << std::endl;
    const char* reg_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };
    
    for (size_t i = 0; i < NUM_REGISTERS; i++) {
        std::cout << "x" << std::setw(2) << i << " (" << std::setw(4) << reg_names[i] << "): "
                  << "0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << registers[i] << std::dec << std::endl;
    }
    std::cout << "PC: 0x" << std::hex << pc << std::dec << std::endl;
}

void CPU::dump_memory(uint32_t start, uint32_t length) const {
    std::cout << "=== Memory Dump [0x" << std::hex << start 
              << " - 0x" << (start + length) << "] ===" << std::dec << std::endl;
    
    for (uint32_t i = 0; i < length; i += 16) {
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << (start + i) << ": ";
        
        for (uint32_t j = 0; j < 16 && (i + j) < length; j++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(memory[start + i + j]) << " ";
        }
        std::cout << std::dec << std::endl;
    }
}