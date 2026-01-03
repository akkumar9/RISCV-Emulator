# RISC-V CPU Emulator with JIT Compiler

A functional RISC-V RV32I emulator written in C++ that executes real compiled binaries and includes a JIT compiler for runtime performance optimization.

## Features

**Core Emulator**
- Full RV32I base instruction set implementation
- ELF binary loader for executing compiled programs
- 128MB addressable memory space
- System call interface (exit, write)
- Cycle-accurate execution tracking

**JIT Compiler**
- Runtime translation of RISC-V instructions to native ARM64 code
- Hot path detection through execution profiling
- Cross-architecture code generation
- Compiles frequently executed code for significant speedup

## Architecture

The emulator uses a two-tier execution model:
- Interpreter for initial execution and profiling
- JIT compiler for hot code paths (frequently executed instructions)

When a code section is identified as hot, the JIT compiler translates RISC-V instructions directly to ARM64 machine code and executes them natively.

## Building
```bash
mkdir build && cd build
cmake ..
make
```

## Running Tests

Basic interpreter test:
```bash
./test_interpreter
```

Execute a compiled binary:
```bash
./test_elf ../binaries/loop
```

Test JIT compilation:
```bash
./test_jit_riscv
```

Expected output:
```
x1 = 10
x2 = 20
x3 = 30
RISC-V JIT COMPILER WORKS!
```

## Performance Profiling

The emulator includes execution profiling to identify performance bottlenecks:
```bash
./test_profile ../binaries/loop
```

This shows:
- Total instructions executed
- Hot instruction addresses
- Execution time breakdown
- Candidates for JIT compilation

## Implementation Details

**Language:** C++17  
**Target ISA:** RISC-V RV32I  
**JIT Target:** ARM64 (Apple Silicon)  
**Memory:** 128MB virtual address space  
**Registers:** 32 general-purpose (x0-x31)  

The JIT compiler currently supports:
- Arithmetic operations (ADD, SUB, ADDI)
- Logical operations (AND, OR, XOR)
- Basic blocks up to 10 instructions

## Current Status

Working:
- Interpreter for full RV32I instruction set
- ELF loading and execution
- JIT compilation for arithmetic/logic operations
- Hot path detection and profiling

In Progress:
- Extended JIT support for memory operations
- Branch/jump compilation
- Automatic hot path compilation in interpreter

## Technical Notes

The JIT compiler generates ARM64 machine code at runtime using mmap with executable permissions. On macOS, this requires proper instruction cache invalidation (__builtin___clear_cache) and memory protection changes (mprotect).

Register mapping for JIT:
- RISC-V x0-x7 map to ARM64 X9-X16
- X0 holds the register array pointer
- X8 used as scratch register for immediates

## Example Programs

Included test programs:
- `hello.c` - Simple arithmetic (10 + 20)
- `loop.c` - Iterative sum (1+2+...+100)
- `fib.c` - Fibonacci calculation

Compile new programs:
```bash
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -static \
  -Wl,-Ttext=0x1000 -o program program.c
```

## Benchmarks

For the loop test (sum of 1-100):
- Interpreter: 1013 instructions executed
- 80% of time spent in 10 hot instructions
- Potential 10-100x speedup with full JIT compilation