#include "decoder.h"

Instruction Decoder::decode(uint32_t raw) {
    Instruction inst;
    inst.raw = raw;
    
    // Extract opcode (bits 0-6)
    inst.opcode = raw & 0x7F;
    
    // Extract fields based on instruction type
    inst.rd = (raw >> 7) & 0x1F;
    inst.funct3 = (raw >> 12) & 0x7;
    inst.rs1 = (raw >> 15) & 0x1F;
    inst.rs2 = (raw >> 20) & 0x1F;
    inst.funct7 = (raw >> 25) & 0x7F;
    
    // Determine instruction type
    inst.type = get_type(inst.opcode);
    
    // Decode immediate based on type
    switch (inst.type) {
        case InstructionType::I_TYPE:
            // I-type: imm[11:0] = inst[31:20]
            inst.imm = sign_extend((raw >> 20) & 0xFFF, 12);
            break;
            
        case InstructionType::S_TYPE:
            // S-type: imm[11:5] = inst[31:25], imm[4:0] = inst[11:7]
            inst.imm = sign_extend(
                ((raw >> 25) << 5) | ((raw >> 7) & 0x1F), 
                12
            );
            break;
            
        case InstructionType::B_TYPE:
            // B-type: imm[12|10:5] = inst[31:25], imm[4:1|11] = inst[11:7]
            inst.imm = sign_extend(
                ((raw >> 31) << 12) |           // imm[12]
                (((raw >> 7) & 0x1) << 11) |    // imm[11]
                (((raw >> 25) & 0x3F) << 5) |   // imm[10:5]
                (((raw >> 8) & 0xF) << 1),      // imm[4:1]
                13
            );
            break;
            
        case InstructionType::U_TYPE:
            // U-type: imm[31:12] = inst[31:12]
            inst.imm = raw & 0xFFFFF000;
            break;
            
        case InstructionType::J_TYPE:
            // J-type: imm[20|10:1|11|19:12] = inst[31:12]
            inst.imm = sign_extend(
                ((raw >> 31) << 20) |               // imm[20]
                (((raw >> 12) & 0xFF) << 12) |      // imm[19:12]
                (((raw >> 20) & 0x1) << 11) |       // imm[11]
                (((raw >> 21) & 0x3FF) << 1),       // imm[10:1]
                21
            );
            break;
            
        case InstructionType::R_TYPE:
            inst.imm = 0;  // R-type has no immediate
            break;
    }
    
    return inst;
}

int32_t Decoder::sign_extend(uint32_t value, int bits) {
    // Check if sign bit is set
    uint32_t sign_bit = 1 << (bits - 1);
    if (value & sign_bit) {
        // Sign extend by setting upper bits to 1
        return value | (~0U << bits);
    }
    return value;
}

InstructionType Decoder::get_type(uint8_t opcode) {
    switch (opcode) {
        case 0x33: return InstructionType::R_TYPE;  // ADD, SUB, AND, OR, XOR, etc.
        case 0x13: return InstructionType::I_TYPE;  // ADDI, SLTI, XORI, etc.
        case 0x03: return InstructionType::I_TYPE;  // LW, LH, LB
        case 0x67: return InstructionType::I_TYPE;  // JALR
        case 0x73: return InstructionType::I_TYPE;  // ECALL, EBREAK, CSR  ← ADD THIS LINE
        case 0x23: return InstructionType::S_TYPE;  // SW, SH, SB
        case 0x63: return InstructionType::B_TYPE;  // BEQ, BNE, BLT, BGE
        case 0x37: return InstructionType::U_TYPE;  // LUI
        case 0x17: return InstructionType::U_TYPE;  // AUIPC
        case 0x6F: return InstructionType::J_TYPE;  // JAL
        default:
            throw std::runtime_error("Unknown opcode: " + std::to_string(opcode));
    }
}