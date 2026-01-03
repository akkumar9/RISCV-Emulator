#ifndef ARM64_ASSEMBLER_H
#define ARM64_ASSEMBLER_H

#include "code_buffer.h"
#include <cstdint>

// ARM64 register encoding (X0-X30, plus SP/XZR)
enum class ARM64Reg : uint8_t {
    X0 = 0,   X1 = 1,   X2 = 2,   X3 = 3,
    X4 = 4,   X5 = 5,   X6 = 6,   X7 = 7,
    X8 = 8,   X9 = 9,   X10 = 10, X11 = 11,
    X12 = 12, X13 = 13, X14 = 14, X15 = 15,
    X16 = 16, X17 = 17, X18 = 18, X19 = 19,
    X20 = 20, X21 = 21, X22 = 22, X23 = 23,
    X24 = 24, X25 = 25, X26 = 26, X27 = 27,
    X28 = 28, X29 = 29, X30 = 30, SP = 31
};

// ARM64 condition codes
enum class ARM64Cond : uint8_t {
    EQ = 0b0000,  // Equal
    NE = 0b0001,  // Not equal
    LT = 0b1011,  // Less than (signed)
    GE = 0b1010,  // Greater or equal (signed)
    GT = 0b1100,  // Greater than (signed)
    LE = 0b1101   // Less or equal (signed)
};

// Simple ARM64 assembler
class ARM64Assembler {
public:
    ARM64Assembler(CodeBuffer& buffer) : buffer(buffer) {}
    
    // === Data Processing Instructions ===
    
    // MOV Wd, #imm16 (32-bit)
    void mov_reg_imm(ARM64Reg dst, uint32_t imm);
    
    // MOV Wd, Wm (32-bit register to register)
    void mov_reg_reg(ARM64Reg dst, ARM64Reg src);
    
    // ADD Wd, Wn, Wm (32-bit)
    void add_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2);
    
    // SUB Wd, Wn, Wm (32-bit)
    void sub_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2);
    
    // AND Wd, Wn, Wm (32-bit)
    void and_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2);
    
    // ORR Wd, Wn, Wm (32-bit)
    void orr_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2);
    
    // EOR Wd, Wn, Wm (32-bit XOR)
    void eor_reg_reg_reg(ARM64Reg dst, ARM64Reg src1, ARM64Reg src2);
    
    // === Compare and Branch ===
    
    // CMP Wn, Wm (compare 32-bit)
    void cmp_reg_reg(ARM64Reg left, ARM64Reg right);
    
    // B.cond label (conditional branch)
    void b_cond(ARM64Cond cond, int32_t offset);
    
    // B label (unconditional branch)
    void b(int32_t offset);
    
    // RET (return)
    void ret();
    
    // === Memory Operations ===
    
    // LDR Wt, [Xn, #offset] (load 32-bit)
    void ldr_reg_mem(ARM64Reg dst, ARM64Reg base, int32_t offset);
    
    // STR Wt, [Xn, #offset] (store 32-bit)
    void str_reg_mem(ARM64Reg src, ARM64Reg base, int32_t offset);
    
    // Get current position (for calculating branch offsets)
    size_t get_position() const { return buffer.get_position(); }
    
private:
    CodeBuffer& buffer;
    
    static uint8_t reg_num(ARM64Reg reg) {
        return static_cast<uint8_t>(reg);
    }
};

#endif // ARM64_ASSEMBLER_H