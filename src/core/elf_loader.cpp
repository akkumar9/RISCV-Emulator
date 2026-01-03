#include "elf_loader.h"
#include <fstream>
#include <iostream>
#include <cstring>

void ELFLoader::load(const std::string& filename, CPU& cpu) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    // Read ELF header
    ELF32_Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    validate_elf(header);
    
    std::cout << "Loading ELF file: " << filename << std::endl;
    std::cout << "Entry point: 0x" << std::hex << header.e_entry << std::dec << std::endl;
    
    // Read program headers
    file.seekg(header.e_phoff);
    
    for (int i = 0; i < header.e_phnum; i++) {
        ELF32_ProgramHeader phdr;
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));
        
        // PT_LOAD = 1 (loadable segment)
        if (phdr.p_type == 1) {
            std::cout << "Loading segment " << i 
                      << " at 0x" << std::hex << phdr.p_vaddr 
                      << " (size: " << std::dec << phdr.p_filesz << " bytes)" << std::endl;
            
            // Read segment data
            std::vector<uint8_t> data(phdr.p_filesz);
            file.seekg(phdr.p_offset);
            file.read(reinterpret_cast<char*>(data.data()), phdr.p_filesz);
            
            // Load into memory
            for (size_t j = 0; j < data.size(); j++) {
                cpu.write_byte(phdr.p_vaddr + j, data[j]);
            }
            
            // Zero-fill remaining bytes (BSS section)
            for (size_t j = phdr.p_filesz; j < phdr.p_memsz; j++) {
                cpu.write_byte(phdr.p_vaddr + j, 0);
            }
        }
    }
    
    // Set PC to entry point
    cpu.set_pc(header.e_entry);
    
    // Set up stack pointer (x2/sp)
    cpu.set_register(2, 0x07FFF000);
    
    std::cout << "ELF loaded successfully" << std::endl;
}

void ELFLoader::validate_elf(const ELF32_Header& header) {
    // Check magic number
    if (header.e_ident[0] != 0x7F || 
        header.e_ident[1] != 'E' ||
        header.e_ident[2] != 'L' ||
        header.e_ident[3] != 'F') {
        throw std::runtime_error("Not a valid ELF file");
    }
    
    // Check class (32-bit)
    if (header.e_ident[4] != 1) {
        throw std::runtime_error("Not a 32-bit ELF file");
    }
    
    // Check machine type (RISC-V = 0xF3)
    if (header.e_machine != 0xF3) {
        throw std::runtime_error("Not a RISC-V binary");
    }
    
    std::cout << "ELF validation passed" << std::endl;
}