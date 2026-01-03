#include "interpreter.h"
#include <iostream>
#include <cstdlib>

void Interpreter::step() {
    // Fetch instruction
    uint32_t pc = cpu.get_pc();
    uint32_t raw_inst = cpu.read_word(pc);
    
    // Decode
    Instruction inst = Decoder::decode(raw_inst);
    
    // Execute based on type
    switch (inst.type) {
        case InstructionType::R_TYPE:
            execute_r_type(inst);
            break;
        case InstructionType::I_TYPE:
            execute_i_type(inst);
            break;
        case InstructionType::S_TYPE:
            execute_s_type(inst);
            break;
        case InstructionType::B_TYPE:
            execute_b_type(inst);
            break;
        case InstructionType::U_TYPE:
            execute_u_type(inst);
            break;
        case InstructionType::J_TYPE:
            execute_j_type(inst);
            break;
    }
    
    instructions_executed++;
}

void Interpreter::run(uint64_t max_instructions) {
    try {
        while (instructions_executed < max_instructions) {
            step();
        }
        std::cout << "Reached max instruction limit" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Execution stopped: " << e.what() << std::endl;
    }
}

void Interpreter::execute_r_type(const Instruction& inst) {
    uint32_t rs1_val = cpu.get_register(inst.rs1);
    uint32_t rs2_val = cpu.get_register(inst.rs2);
    uint32_t result = 0;
    
    // Opcode 0x33 = R-type arithmetic
    if (inst.opcode == 0x33) {
        switch (inst.funct3) {
            case 0x0: // ADD/SUB
                if (inst.funct7 == 0x00) {
                    result = rs1_val + rs2_val; // ADD
                } else if (inst.funct7 == 0x20) {
                    result = rs1_val - rs2_val; // SUB
                }
                break;
            case 0x4: // XOR
                result = rs1_val ^ rs2_val;
                break;
            case 0x6: // OR
                result = rs1_val | rs2_val;
                break;
            case 0x7: // AND
                result = rs1_val & rs2_val;
                break;
            case 0x1: // SLL (shift left logical)
                result = rs1_val << (rs2_val & 0x1F);
                break;
            case 0x5: // SRL/SRA
                if (inst.funct7 == 0x00) {
                    result = rs1_val >> (rs2_val & 0x1F); // SRL
                } else if (inst.funct7 == 0x20) {
                    result = static_cast<int32_t>(rs1_val) >> (rs2_val & 0x1F); // SRA
                }
                break;
            case 0x2: // SLT (set less than)
                result = static_cast<int32_t>(rs1_val) < static_cast<int32_t>(rs2_val) ? 1 : 0;
                break;
            case 0x3: // SLTU (set less than unsigned)
                result = rs1_val < rs2_val ? 1 : 0;
                break;
        }
    }
    
    cpu.set_register(inst.rd, result);
    cpu.increment_pc();
}

void Interpreter::execute_i_type(const Instruction& inst) {
    uint32_t rs1_val = cpu.get_register(inst.rs1);
    uint32_t result = 0;
    
    if (inst.opcode == 0x13) {
        // I-type arithmetic
        switch (inst.funct3) {
            case 0x0: // ADDI
                result = rs1_val + inst.imm;
                break;
            case 0x4: // XORI
                result = rs1_val ^ inst.imm;
                break;
            case 0x6: // ORI
                result = rs1_val | inst.imm;
                break;
            case 0x7: // ANDI
                result = rs1_val & inst.imm;
                break;
            case 0x1: // SLLI
                result = rs1_val << (inst.imm & 0x1F);
                break;
            case 0x5: // SRLI/SRAI
                if ((inst.imm & 0x400) == 0) {
                    result = rs1_val >> (inst.imm & 0x1F); // SRLI
                } else {
                    result = static_cast<int32_t>(rs1_val) >> (inst.imm & 0x1F); // SRAI
                }
                break;
            case 0x2: // SLTI
                result = static_cast<int32_t>(rs1_val) < inst.imm ? 1 : 0;
                break;
            case 0x3: // SLTIU
                result = rs1_val < static_cast<uint32_t>(inst.imm) ? 1 : 0;
                break;
        }
        cpu.set_register(inst.rd, result);
        cpu.increment_pc();
        
    } else if (inst.opcode == 0x03) {
        // Load instructions
        uint32_t addr = rs1_val + inst.imm;
        
        switch (inst.funct3) {
            case 0x0: // LB (load byte)
                result = static_cast<int8_t>(cpu.read_byte(addr));
                break;
            case 0x1: // LH (load halfword)
                result = static_cast<int16_t>(cpu.read_word(addr) & 0xFFFF);
                break;
            case 0x2: // LW (load word)
                result = cpu.read_word(addr);
                break;
            case 0x4: // LBU (load byte unsigned)
                result = cpu.read_byte(addr);
                break;
            case 0x5: // LHU (load halfword unsigned)
                result = cpu.read_word(addr) & 0xFFFF;
                break;
        }
        cpu.set_register(inst.rd, result);
        cpu.increment_pc();
        
    } else if (inst.opcode == 0x67) {
        // JALR (jump and link register)
        uint32_t target = (rs1_val + inst.imm) & ~1;
        cpu.set_register(inst.rd, cpu.get_pc() + 4);
        cpu.set_pc(target);
        
    } else if (inst.opcode == 0x73) {
        // ECALL/EBREAK
        if (inst.imm == 0) {
            handle_ecall();
        } else {
            throw std::runtime_error("EBREAK encountered");
        }
        cpu.increment_pc();
    }
}

void Interpreter::execute_s_type(const Instruction& inst) {
    uint32_t rs1_val = cpu.get_register(inst.rs1);
    uint32_t rs2_val = cpu.get_register(inst.rs2);
    uint32_t addr = rs1_val + inst.imm;
    
    switch (inst.funct3) {
        case 0x0: // SB (store byte)
            cpu.write_byte(addr, rs2_val & 0xFF);
            break;
        case 0x1: // SH (store halfword)
            cpu.write_word(addr, rs2_val & 0xFFFF);
            break;
        case 0x2: // SW (store word)
            cpu.write_word(addr, rs2_val);
            break;
    }
    
    cpu.increment_pc();
}

void Interpreter::execute_b_type(const Instruction& inst) {
    int32_t rs1_val = cpu.get_register(inst.rs1);
    int32_t rs2_val = cpu.get_register(inst.rs2);
    bool take_branch = false;
    
    switch (inst.funct3) {
        case 0x0: // BEQ
            take_branch = (rs1_val == rs2_val);
            break;
        case 0x1: // BNE
            take_branch = (rs1_val != rs2_val);
            break;
        case 0x4: // BLT
            take_branch = (rs1_val < rs2_val);
            break;
        case 0x5: // BGE
            take_branch = (rs1_val >= rs2_val);
            break;
        case 0x6: // BLTU
            take_branch = (static_cast<uint32_t>(rs1_val) < static_cast<uint32_t>(rs2_val));
            break;
        case 0x7: // BGEU
            take_branch = (static_cast<uint32_t>(rs1_val) >= static_cast<uint32_t>(rs2_val));
            break;
    }
    
    if (take_branch) {
        cpu.set_pc(cpu.get_pc() + inst.imm);
    } else {
        cpu.increment_pc();
    }
}

void Interpreter::execute_u_type(const Instruction& inst) {
    if (inst.opcode == 0x37) {
        // LUI (load upper immediate)
        cpu.set_register(inst.rd, inst.imm);
    } else if (inst.opcode == 0x17) {
        // AUIPC (add upper immediate to PC)
        cpu.set_register(inst.rd, cpu.get_pc() + inst.imm);
    }
    cpu.increment_pc();
}

void Interpreter::execute_j_type(const Instruction& inst) {
    // JAL (jump and link)
    cpu.set_register(inst.rd, cpu.get_pc() + 4);
    cpu.set_pc(cpu.get_pc() + inst.imm);
}

void Interpreter::handle_ecall() {
    // System call convention:
    // a7 (x17) = syscall number
    // a0-a6 (x10-x16) = arguments
    // Result in a0
    
    uint32_t syscall_num = cpu.get_register(17); // a7
    uint32_t arg0 = cpu.get_register(10);        // a0
    uint32_t arg1 = cpu.get_register(11);        // a1
    uint32_t arg2 = cpu.get_register(12);        // a2
    
    switch (syscall_num) {
        case 93: // exit
            std::cout << "Program exited with code " << arg0 << std::endl;
            throw std::runtime_error("Exit syscall");
            break;
            
        case 64: // write
            if (arg0 == 1) { // stdout
                // arg1 = buffer address, arg2 = length
                for (uint32_t i = 0; i < arg2; i++) {
                    std::cout << static_cast<char>(cpu.read_byte(arg1 + i));
                }
                cpu.set_register(10, arg2); // Return bytes written
            }
            break;
            
        default:
            std::cout << "Unknown syscall: " << syscall_num << std::endl;
            break;
    }
}