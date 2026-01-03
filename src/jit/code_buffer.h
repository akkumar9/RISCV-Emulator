#ifndef CODE_BUFFER_H
#define CODE_BUFFER_H

#include <cstdint>
#include <vector>
#include <sys/mman.h>
#include <stdexcept>

#ifdef __APPLE__
#include <libkern/OSCacheControl.h>  // ADD THIS
#endif

// Buffer for generated machine code
class CodeBuffer {
public:
    CodeBuffer(size_t size = 64 * 1024) {  // 64KB default
    #ifdef __APPLE__
        // macOS: allocate with read/write first, then make executable
        buffer = static_cast<uint8_t*>(
            mmap(nullptr, size, 
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_JIT, -1, 0)
        );
    #else
        buffer = static_cast<uint8_t*>(
            mmap(nullptr, size, 
                 PROT_READ | PROT_WRITE | PROT_EXEC,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
        );
    #endif
    
    if (buffer == MAP_FAILED) {
        throw std::runtime_error("Failed to allocate executable memory");
    }
    
    capacity = size;
    position = 0;
}
    
    ~CodeBuffer() {
        if (buffer != MAP_FAILED) {
            munmap(buffer, capacity);
        }
    }
    
    // Emit single byte
    void emit_byte(uint8_t byte) {
        if (position >= capacity) {
            throw std::runtime_error("Code buffer overflow");
        }
        buffer[position++] = byte;
    }
    
    // Emit 32-bit value (little-endian)
    void emit_uint32(uint32_t value) {
        emit_byte(value & 0xFF);
        emit_byte((value >> 8) & 0xFF);
        emit_byte((value >> 16) & 0xFF);
        emit_byte((value >> 24) & 0xFF);
    }
    
    // Emit 64-bit value (little-endian)
    void emit_uint64(uint64_t value) {
        emit_uint32(value & 0xFFFFFFFF);
        emit_uint32(value >> 32);
    }
    
    // Get function pointer to generated code
    template<typename Func>
    Func get_function() const {
        return reinterpret_cast<Func>(buffer);
    }
    
    // Get current position
    uint8_t* get_current_ptr() const {
        return buffer + position;
    }
    
    size_t get_position() const { return position; }
    size_t get_capacity() const { return capacity; }
    
    // Reset buffer
    void reset() { position = 0; }
    // Make buffer executable (call after writing code)
void make_executable() {
    #ifdef __APPLE__
        // On Apple Silicon, we need to flush instruction cache
        // Use pthread_jit_write_protect_np if available
        __builtin___clear_cache((char*)buffer, (char*)(buffer + position));
        
        if (mprotect(buffer, capacity, PROT_READ | PROT_EXEC) != 0) {
            throw std::runtime_error("Failed to make buffer executable");
        }
    #endif
}
    
private:
    uint8_t* buffer;
    size_t capacity;
    size_t position;
};

#endif // CODE_BUFFER_H