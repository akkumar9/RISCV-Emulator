#ifndef DECODER_H
#define DECODER_H

#include "cpu.h"

class Decoder {
public:
    static Instruction decode(uint32_t raw_instruction);
    
private:
    static int32_t sign_extend(uint32_t value, int bits);
    static InstructionType get_type(uint8_t opcode);
};

#endif // DECODER_H