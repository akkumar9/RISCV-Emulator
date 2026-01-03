#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include "cpu.h"
#include <string>
#include <vector>

class ELFLoader {
public:
    static void load(const std::string& filename, CPU& cpu);
    
private:
    struct ELF32_Header {
        uint8_t e_ident[16];
        uint16_t e_type;
        uint16_t e_machine;
        uint32_t e_version;
        uint32_t e_entry;
        uint32_t e_phoff;
        uint32_t e_shoff;
        uint32_t e_flags;
        uint16_t e_ehsize;
        uint16_t e_phentsize;
        uint16_t e_phnum;
        uint16_t e_shentsize;
        uint16_t e_shnum;
        uint16_t e_shstrndx;
    };
    
    struct ELF32_ProgramHeader {
        uint32_t p_type;
        uint32_t p_offset;
        uint32_t p_vaddr;
        uint32_t p_paddr;
        uint32_t p_filesz;
        uint32_t p_memsz;
        uint32_t p_flags;
        uint32_t p_align;
    };
    
    static void validate_elf(const ELF32_Header& header);
};

#endif // ELF_LOADER_H