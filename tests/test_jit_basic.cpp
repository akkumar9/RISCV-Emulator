#include "../src/jit/code_buffer.h"
#include "../src/jit/arm64_assembler.h"
#include <iostream>

int main() {
    std::cout << "=== ARM64 JIT Basic Test ===" << std::endl;
    
    // Test 1: return 42
    {
        CodeBuffer buffer;
        ARM64Assembler asm_(buffer);
        
        asm_.mov_reg_imm(ARM64Reg::X0, 42);
        asm_.ret();
        
        buffer.make_executable();
        
        typedef int (*TestFunc)();
        TestFunc func = buffer.get_function<TestFunc>();
        
        int result = func();
        
        std::cout << "Result: " << result << std::endl;
        std::cout << "Expected: 42" << std::endl;
        
        if (result == 42) {
            std::cout << "✅ ARM64 JIT compilation works!" << std::endl;
        } else {
            std::cout << "❌ JIT compilation failed" << std::endl;
        }
    }
    
    // Test 2: Add two numbers (fresh buffer)
    {
        CodeBuffer buffer;
        ARM64Assembler asm_(buffer);
        
        asm_.add_reg_reg_reg(ARM64Reg::X0, ARM64Reg::X0, ARM64Reg::X1);
        asm_.ret();
        
        buffer.make_executable();
        
        typedef int (*AddFunc)(int, int);
        AddFunc add_func = buffer.get_function<AddFunc>();
        
        int add_result = add_func(10, 20);
        std::cout << "\nAdd test: 10 + 20 = " << add_result << std::endl;
        
        if (add_result == 30) {
            std::cout << "✅ ARM64 JIT addition works!" << std::endl;
        } else {
            std::cout << "❌ JIT addition failed (got " << add_result << ")" << std::endl;
        }
    }
    
    return 0;
}