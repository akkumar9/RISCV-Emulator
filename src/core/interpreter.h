#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "cpu.h"
#include "decoder.h"
#include <unordered_map>

class Interpreter {
public:
    explicit Interpreter(CPU& cpu) : cpu(cpu), instructions_executed(0) {}
    
    // Execute one instruction at PC
    void step();
    
    // Execute until halt or error
    void run(uint64_t max_instructions = 1000000);
    
    // Statistics
    uint64_t get_instructions_executed() const { return instructions_executed; }
    void reset_stats() { instructions_executed = 0; }
    
private:
    CPU& cpu;
    uint64_t instructions_executed;
    
    // Instruction handlers
    void execute_r_type(const Instruction& inst);
    void execute_i_type(const Instruction& inst);
    void execute_s_type(const Instruction& inst);
    void execute_b_type(const Instruction& inst);
    void execute_u_type(const Instruction& inst);
    void execute_j_type(const Instruction& inst);
    
    // Helper for system calls
    void handle_ecall();
};

#endif // INTERPRETER_H