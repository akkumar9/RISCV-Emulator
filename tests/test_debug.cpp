#include "cpu.h"
#include "interpreter.h"
#include "decoder.h"
#include "elf_loader.h"
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <elf-file>" << std::endl;
        return 1;
    }
    
    try {
        CPU cpu;
        Interpreter interp(cpu);
        
        // Load ELF
        ELFLoader::load(argv[1], cpu);
        
        std::cout << "\n=== Stepping through with debug output ===" << std::endl;
        
        // Step through and show each instruction
        for (int i = 0; i < 20; i++) {
            uint32_t pc = cpu.get_pc();
            uint32_t raw_inst = cpu.read_word(pc);
            
            std::cout << "\n[" << i << "] PC: 0x" << std::hex << pc 
                      << " | Inst: 0x" << std::setw(8) << std::setfill('0') << raw_inst 
                      << std::dec << std::setfill(' ') << std::endl;
            
            try {
                Instruction decoded = Decoder::decode(raw_inst);
                std::cout << "    Opcode: 0x" << std::hex << static_cast<int>(decoded.opcode)
                          << " | Type: " << static_cast<int>(decoded.type)
                          << " | rd: x" << std::dec << static_cast<int>(decoded.rd)
                          << " | rs1: x" << static_cast<int>(decoded.rs1)
                          << " | imm: " << decoded.imm << std::endl;
                
                interp.step();
                
                std::cout << "    a0: " << cpu.get_register(10)
                          << " | a1: " << cpu.get_register(11) << std::endl;
            } catch (const std::exception& e) {
                std::cout << "    ERROR: " << e.what() << std::endl;
                break;
            }
        }
        
        std::cout << "\n=== Final State ===" << std::endl;
        std::cout << "Return value (a0): " << cpu.get_register(10) << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}