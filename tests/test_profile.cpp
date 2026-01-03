#include "cpu.h"
#include "interpreter.h"
#include "elf_loader.h"
#include <iostream>

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
        
        std::cout << "\n=== Starting execution with profiling ===" << std::endl;
        
        // Run
        interp.run(100000);
        
        std::cout << "\n=== Execution complete ===" << std::endl;
        std::cout << "Return value (a0): " << cpu.get_register(10) << std::endl;
        std::cout << "Instructions executed: " << interp.get_instructions_executed() << std::endl;
        
        // Print profile
        interp.get_profiler().print_profile();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}