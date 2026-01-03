#include "arm64_assembler.h"

void ARM64Assembler::mov_reg_imm(ARM64Reg dst, uint32_t imm) {
    // MOVZ Wd, #imm16, LSL #0
    // Encoding: 0101 0010 100 imm16 Rd
    // For simplicity, only handle imm16 (0-65535)
    
    if (imm > 0xFFFF) {
        // Use MOVZ + MOVK for larger immediates
        // MOVZ Wd, #(imm & 0xFFFF), LSL #0
        uint32_t inst = 0x52800000 | ((imm & 0xFFFF) << 5) | reg_num(dst);
        buffer.emit_uint32(inst);
        
        // MOVK Wd, #(imm >> 16), LSL #16
        if (imm >> 16) {
            inst = 0x72A00000 | (((imm >> 16) & 0xFFFF) << 5) | reg_num(dst);
            buffer.emit_uint32(inst);
        }
    } else {
        // Simple MOVZ
        uint32_t inst = 0x52800000 | (imm << 5) | reg_num(dst);
        buffer.emit_uint32(inst);
    }
}

void ARM64Assembler::mov_reg_reg(ARM64Reg dst, ARM64Reg src) {
    // ORR Wd, WZR, Wm (this is the canonical MOV encoding)
    // Encoding: 0010 1010 000 Rm 000000 11111 Rd
    uint32_t inst = 0x2A0003E0 | (reg_num(src) << 16) | reg_num(dst);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::add_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2) {
    // ADD Wd, Wn, Wm
    // Encoding: 0000 1011 000 Rm 000000 Rn Rd
    uint32_t inst = 0x0B000000 | (reg_num(src2) << 16) | (reg_num(src1) << 5) | reg_num(dst);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::sub_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2) {
    // SUB Wd, Wn, Wm
    // Encoding: 0100 1011 000 Rm 000000 Rn Rd
    uint32_t inst = 0x4B000000 | (reg_num(src2) << 16) | (reg_num(src1) << 5) | reg_num(dst);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::and_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2) {
    // AND Wd, Wn, Wm
    // Encoding: 0000 1010 000 Rm 000000 Rn Rd
    uint32_t inst = 0x0A000000 | (reg_num(src2) << 16) | (reg_num(src1) << 5) | reg_num(dst);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::orr_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2) {
    // ORR Wd, Wn, Wm
    // Encoding: 0010 1010 000 Rm 000000 Rn Rd
    uint32_t inst = 0x2A000000 | (reg_num(src2) << 16) | (reg_num(src1) << 5) | reg_num(dst);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::eor_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2) {
    // EOR Wd, Wn, Wm
    // Encoding: 0100 1010 000 Rm 000000 Rn Rd
    uint32_t inst = 0x4A000000 | (reg_num(src2) << 16) | (reg_num(src1) << 5) | reg_num(dst);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::cmp_reg_reg(ARM64Reg left, ARM64Reg right) {
    // CMP Wn, Wm = SUBS WZR, Wn, Wm
    // Encoding: 0110 1011 000 Rm 000000 Rn 11111
    uint32_t inst = 0x6B00001F | (reg_num(right) << 16) | (reg_num(left) << 5);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::b_cond(ARM64Cond cond, int32_t offset) {
    // B.cond label
    // Encoding: 0101 0100 imm19 0 cond
    // Offset is in instructions (divide byte offset by 4)
    int32_t imm19 = offset / 4;
    uint32_t inst = 0x54000000 | ((imm19 & 0x7FFFF) << 5) | static_cast<uint8_t>(cond);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::b(int32_t offset) {
    // B label
    // Encoding: 0001 01 imm26
    int32_t imm26 = offset / 4;
    uint32_t inst = 0x14000000 | (imm26 & 0x3FFFFFF);
    buffer.emit_uint32(inst);
}

void ARM64Assembler::ret() {
    // RET (return to address in X30/LR, but default is RET which uses X30)
    // Encoding: 1101 0110 0101 1111 0000 00 11110 00000
    buffer.emit_uint32(0xD65F03C0);
}

void ARM64Assembler::ldr_reg_mem(ARM64Reg dst, ARM64Reg base, int32_t offset) {
    // LDR Wt, [Xn, #offset]
    // For 32-bit loads: 1011 1001 01 imm12 Rn Rt
    // offset must be 4-byte aligned and divided by 4 for encoding
    
    if (offset < 0 || offset > 16380 || (offset % 4 != 0)) {
        throw std::runtime_error("LDR offset must be 0-16380 and 4-byte aligned");
    }
    
    uint32_t imm12 = offset / 4;
    
    // Use 64-bit base register (X), load to 32-bit dest (W)
    // Encoding: 1011 1001 01 imm12[11:0] Rn[4:0] Rt[4:0]
    uint32_t inst = 0xB9400000;
    inst |= (imm12 & 0xFFF) << 10;
    inst |= (reg_num(base) & 0x1F) << 5;
    inst |= (reg_num(dst) & 0x1F);
    
    buffer.emit_uint32(inst);
}

void ARM64Assembler::str_reg_mem(ARM64Reg src, ARM64Reg base, int32_t offset) {
    // STR Wt, [Xn, #offset]
    // For 32-bit stores: 1011 1001 00 imm12 Rn Rt
    
    if (offset < 0 || offset > 16380 || (offset % 4 != 0)) {
        throw std::runtime_error("STR offset must be 0-16380 and 4-byte aligned");
    }
    
    uint32_t imm12 = offset / 4;
    
    // Encoding: 1011 1001 00 imm12[11:0] Rn[4:0] Rt[4:0]
    uint32_t inst = 0xB9000000;
    inst |= (imm12 & 0xFFF) << 10;
    inst |= (reg_num(base) & 0x1F) << 5;
    inst |= (reg_num(src) & 0x1F);
    
    buffer.emit_uint32(inst);
}