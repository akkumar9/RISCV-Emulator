#ifndef JIT_COMPILER_H
#define JIT_COMPILER_H

#include "../core/cpu.h"
#include "../core/decoder.h"
#include "code_buffer.h"
#include "arm64_assembler.h"
#include <unordered_map>
#include <memory>
#include <cstdint>

// Compiled function signature: void (*)(uint32_t* registers)
typedef void (*CompiledFunc)(uint32_t*);

class JITCompiler {
public:
    JITCompiler() : compilation_threshold(50) {}
    
    // Check if a PC has compiled code
    bool has_compiled_code(uint32_t pc) const {
        return compiled_cache.find(pc) != compiled_cache.end();
    }
    
    // Get compiled function for a PC
    CompiledFunc get_compiled_code(uint32_t pc) const {
        auto it = compiled_cache.find(pc);
        if (it != compiled_cache.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Compile a basic block starting at PC
    void compile_basic_block(CPU& cpu, uint32_t start_pc);
    
    // Set compilation threshold
    void set_threshold(uint64_t threshold) { compilation_threshold = threshold; }
    
    // Statistics
    size_t get_compiled_blocks() const { return compiled_cache.size(); }
    
private:
    std::unordered_map<uint32_t, CompiledFunc> compiled_cache;
    std::vector<std::unique_ptr<CodeBuffer>> buffers;
    uint64_t compilation_threshold;
    
    // Compile a single RISC-V instruction to ARM64
    bool compile_instruction(ARM64Assembler& asm_, const Instruction& inst);
    
    // Register mapping
    static ARM64Reg map_riscv_reg(uint8_t riscv_reg);
};

#endif // JIT_COMPILER_H