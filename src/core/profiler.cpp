#include "profiler.h"
#include <iostream>
#include <iomanip>

void Profiler::print_profile() const {
    std::cout << "\n=== Execution Profile ===" << std::endl;
    std::cout << "Total instructions: " << total_instructions << std::endl;
    std::cout << "Unique PCs: " << instruction_counts.size() << std::endl;
    
    auto hot_instructions = get_hot_instructions(20);
    
    std::cout << "\n=== Top 20 Hottest Instructions ===" << std::endl;
    std::cout << std::left << std::setw(12) << "PC"
              << std::setw(15) << "Count"
              << std::setw(12) << "Percentage" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    for (const auto& entry : hot_instructions) {
        double percentage = (static_cast<double>(entry.count) / total_instructions) * 100.0;
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << entry.pc
                  << std::dec << std::setfill(' ')
                  << "  " << std::setw(13) << entry.count
                  << "  " << std::fixed << std::setprecision(2) << std::setw(10) << percentage << "%"
                  << std::endl;
    }
    
    // Detect hot loops
    auto hot_loops = detect_hot_loops(100);
    if (!hot_loops.empty()) {
        std::cout << "\n=== Hot Loops (>100 executions) ===" << std::endl;
        for (uint32_t pc : hot_loops) {
            std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << pc
                      << std::dec << " - executed " << instruction_counts.at(pc) << " times"
                      << std::endl;
        }
    }
}