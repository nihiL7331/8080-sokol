#ifndef CPU_HPP
#define CPU_HPP

#include "bus.hpp"
#include <bit>
#include <cstdint>

const uint8_t FLAG_S = 0x80;
const uint8_t FLAG_Z = 0x40;
const uint8_t FLAG_AC = 0x10;
const uint8_t FLAG_P = 0x04;
const uint8_t FLAG_CY = 0x01;

class CPU {
private:
  uint8_t A, B, C, D, E, H, L; // Registers
  uint16_t PC;
  uint16_t SP;
  // Flag layout
  // - Sign
  // - Zero
  // - Always 0
  // - Auxiliary Carry
  // - Always 0
  // - Parity
  // - Always 1
  // - Carry
  uint8_t F;
  Bus &bus;
  // CPU interrupt
  bool I;
  bool log;

public:
  CPU(class Bus &b) : bus(b), PC(0), SP(0), F(0x02), I(false), log(false) {};
  ~CPU() = default;

  int Step();

  // For debug purposes
  uint16_t GetPC() { return PC; }
  void Log(bool set) { log = set; }

  void SetFlag(const uint8_t mask, bool cond) {
    uint8_t bs = -static_cast<uint8_t>(cond);
    F = (F & ~mask) | (bs & mask);
  }
  void UpdateFlagsZSP(uint8_t res) {
    SetFlag(FLAG_Z, res == 0);
    SetFlag(FLAG_S, res & FLAG_S);
    SetFlag(FLAG_P, (std::popcount(res) & 1) == 0);
  }
  void UpdateFlags(uint8_t res, bool cy, bool ac) {
    UpdateFlagsZSP(res);
    SetFlag(FLAG_CY, cy);
    SetFlag(FLAG_AC, ac);
  }

  void GenerateInterrupt(uint8_t opc) {
    if (I) {
      I = false;
      bus.Write(--SP, PC >> 8);
      bus.Write(--SP, PC & 0xFF);
      if (opc == 0xCF)
        PC = 0x0008;
      else if (opc == 0xD7)
        PC = 0x0010;
    }
  }
};

#endif
