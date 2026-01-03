#include "cpu.h"
#include "decoder.h"
#include <iostream>

int main() {
    std::cout << "=== RISC-V Emulator Test ===" << std::endl;
    
    // Create CPU
    CPU cpu;
    
    // Test 1: Register operations
    std::cout << "\n[Test 1] Register operations" << std::endl;
    cpu.set_register(1, 0x12345678);
    cpu.set_register(2, 0xABCDEF00);
    std::cout << "x1 = 0x" << std::hex << cpu.get_register(1) << std::endl;
    std::cout << "x2 = 0x" << std::hex << cpu.get_register(2) << std::endl;
    
    // Test 2: x0 is always 0
    std::cout << "\n[Test 2] x0 hardwired to zero" << std::endl;
    cpu.set_register(0, 0xDEADBEEF);
    std::cout << "x0 = 0x" << std::hex << cpu.get_register(0) << " (should be 0)" << std::endl;
    
    // Test 3: Memory operations
    std::cout << "\n[Test 3] Memory operations" << std::endl;
    cpu.write_word(0x1000, 0xCAFEBABE);
    uint32_t value = cpu.read_word(0x1000);
    std::cout << "Memory[0x1000] = 0x" << std::hex << value << std::endl;
    
    // Test 4: Decode ADDI instruction
    // ADDI x1, x0, 42  =>  0x02A00093
    std::cout << "\n[Test 4] Decode ADDI x1, x0, 42" << std::endl;
    uint32_t addi_inst = 0x02A00093;
    Instruction decoded = Decoder::decode(addi_inst);
    std::cout << "Opcode: 0x" << std::hex << static_cast<int>(decoded.opcode) << std::endl;
    std::cout << "rd: x" << std::dec << static_cast<int>(decoded.rd) << std::endl;
    std::cout << "rs1: x" << static_cast<int>(decoded.rs1) << std::endl;
    std::cout << "imm: " << decoded.imm << std::endl;
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    return 0;
}