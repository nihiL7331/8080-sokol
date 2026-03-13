#ifndef DISASM_HPP
#define DISASM_HPP

#include <array>
#include <cstddef>

int Disassemble8080Op(const std::array<uint8_t, 0x10000> &code_buffer,
                      size_t pc);

#endif
