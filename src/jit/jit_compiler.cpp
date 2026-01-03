#include "jit_compiler.h"
#include <iostream>

ARM64Reg JITCompiler::map_riscv_reg(uint8_t riscv_reg) {
    if (riscv_reg >= 8) {
        throw std::runtime_error("Only x0-x7 supported in JIT for now");
    }
    return static_cast<ARM64Reg>(9 + riscv_reg);  // X9-X16
}

void JITCompiler::compile_basic_block(CPU& cpu, uint32_t start_pc) {
    std::cout << "JIT: Compiling basic block at 0x" << std::hex << start_pc << std::dec << std::endl;
    
    auto buffer = std::make_unique<CodeBuffer>();
    ARM64Assembler asm_(*buffer);
    
    // X0 = pointer to register array - USE IT DIRECTLY (don't save to X19!)
    
    // Load RISC-V registers x0-x7
    asm_.mov_reg_imm(ARM64Reg::X9, 0);  // x0 = 0
    
    // Load x1-x7 from memory using X0 as base
    for (int i = 1; i < 8; i++) {
        ARM64Reg arm_reg = map_riscv_reg(i);
        asm_.ldr_reg_mem(arm_reg, ARM64Reg::X0, i * 4);
    }
    
    uint32_t pc = start_pc;
    int instructions_compiled = 0;
    
    while (instructions_compiled < 10) {
        try {
            uint32_t raw_inst = cpu.read_word(pc);
            Instruction inst = Decoder::decode(raw_inst);
            
            bool regs_ok = true;
            if (inst.rd < 32 && inst.rd >= 8) regs_ok = false;
            
            if (inst.type == InstructionType::R_TYPE || 
                inst.type == InstructionType::I_TYPE ||
                inst.type == InstructionType::S_TYPE ||
                inst.type == InstructionType::B_TYPE) {
                if (inst.rs1 >= 8) regs_ok = false;
            }
            
            if (inst.type == InstructionType::R_TYPE ||
                inst.type == InstructionType::S_TYPE ||
                inst.type == InstructionType::B_TYPE) {
                if (inst.rs2 >= 8) regs_ok = false;
            }
            
            if (!regs_ok) {
                break;
            }
            
            if (!compile_instruction(asm_, inst)) {
                break;
            }
            
            instructions_compiled++;
            pc += 4;
            
            if (inst.type == InstructionType::B_TYPE || 
                inst.type == InstructionType::J_TYPE ||
                inst.opcode == 0x67 || inst.opcode == 0x73) {
                break;
            }
            
        } catch (const std::exception& e) {
            std::cout << "JIT: " << e.what() << std::endl;
            break;
        }
    }
    
    if (instructions_compiled == 0) {
        std::cout << "JIT: No instructions compiled" << std::endl;
        return;
    }
    
    // Store x1-x7 back using X0 as base
    for (int i = 1; i < 8; i++) {
        ARM64Reg arm_reg = map_riscv_reg(i);
        asm_.str_reg_mem(arm_reg, ARM64Reg::X0, i * 4);
    }
    
    asm_.ret();
    
    buffer->make_executable();
    
    CompiledFunc func = buffer->get_function<CompiledFunc>();
    compiled_cache[start_pc] = func;
    buffers.push_back(std::move(buffer));
    
    std::cout << "JIT: Successfully compiled " << instructions_compiled << " instructions" << std::endl;
}

bool JITCompiler::compile_instruction(ARM64Assembler& asm_, const Instruction& inst) {
    if (inst.type == InstructionType::R_TYPE && inst.opcode == 0x33) {
        ARM64Reg rd = map_riscv_reg(inst.rd);
        ARM64Reg rs1 = map_riscv_reg(inst.rs1);
        ARM64Reg rs2 = map_riscv_reg(inst.rs2);
        
        switch (inst.funct3) {
            case 0x0:
                if (inst.funct7 == 0x00) {
                    asm_.add_reg_reg_reg(rd, rs1, rs2);
                } else if (inst.funct7 == 0x20) {
                    asm_.sub_reg_reg_reg(rd, rs1, rs2);
                } else {
                    return false;
                }
                break;
            case 0x4:
                asm_.eor_reg_reg_reg(rd, rs1, rs2);
                break;
            case 0x6:
                asm_.orr_reg_reg_reg(rd, rs1, rs2);
                break;
            case 0x7:
                asm_.and_reg_reg_reg(rd, rs1, rs2);
                break;
            default:
                return false;
        }
        return true;
    }
    
    if (inst.type == InstructionType::I_TYPE && inst.opcode == 0x13 && inst.funct3 == 0x0) {
        ARM64Reg rd = map_riscv_reg(inst.rd);
        ARM64Reg rs1 = map_riscv_reg(inst.rs1);
        
        uint32_t imm_unsigned = static_cast<uint32_t>(inst.imm);
        
        if (imm_unsigned <= 0xFFFF || (inst.imm >= 0 && inst.imm <= 65535)) {
            asm_.mov_reg_imm(ARM64Reg::X8, imm_unsigned & 0xFFFF);
            asm_.add_reg_reg_reg(rd, rs1, ARM64Reg::X8);
            return true;
        }
    }
    
    return false;
}