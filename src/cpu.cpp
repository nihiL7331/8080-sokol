#include "cpu.hpp"
#include "disassembler.hpp"
#include <cstdlib>
#include <utility>

void CPU::Step() {
  Disassemble8080Op(bus.GetMem(), PC);
  uint8_t opcode = bus.Read(PC);

  PC++;

  // REG PAIRS
  // HIGH / LOW
  //  B   /  C
  //  D   /  E
  //  H   /  L
  //  A   /  F (PSW)
  switch (opcode) {
  case 0x00: { // NOP
    break;
  }
  case 0x01: { // LXI B,D16
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    C = lo;
    B = hi;
    break;
  }
  case 0x02: { // STAX B
    uint16_t BC = (B << 8) | C;
    bus.Write(BC, A);
    break;
  }
  case 0x03: { // INX B
    uint16_t BC = (B << 8) | C;
    BC++;
    B = BC >> 8;
    C = BC & 0xFF;
    break;
  }
  case 0x04: { // INR B
    uint8_t old = B;
    B++;
    UpdateFlagsZSP(B);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x05: { // DCR B
    B--;
    UpdateFlagsZSP(B);
    SetFlag(FLAG_AC, (B & 0x0F) != 0x0F);
    break;
  }
  case 0x06: { // MVI B, D8
    uint8_t byte = bus.Read(PC++);
    B = byte;
    break;
  }
  case 0x07: { // RLC
    uint8_t prev = A >> 7;
    A = (A << 1) | prev;
    SetFlag(FLAG_CY, prev);
    break;
  }
  case 0x08: { // NOOP
    break;
  }
  case 0x0E: { // MVI C,D8
    uint8_t byte = bus.Read(PC++);
    C = byte;
    break;
  }
  case 0x0F: { // RRC
    uint8_t prev = A << 7;
    A = (A >> 1) | prev;
    SetFlag(FLAG_CY, prev);
    break;
  }
  case 0x21: { // LXI H,D16
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    L = lo;
    H = hi;
    break;
  }
  case 0x22: { // SHLD adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    bus.Write(addr, L);
    bus.Write(addr + 1, H);
    break;
  }
  case 0x23: { // INX H
    uint16_t HL = (H << 8) | L;
    HL++;
    H = HL >> 8;
    L = HL & 0xFF;
    break;
  }
  case 0x27: { // DAA (special)
    break;
  }
  case 0x2B: { // DCX H
    uint16_t HL = (H << 8) | L;
    HL--;
    H = HL >> 8;
    L = HL & 0xFF;
    break;
  }
  case 0x31: { // LXI SP, D16
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    SP = (hi << 8) | lo;
    break;
  }
  case 0x32: { // STA adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    bus.Write(addr, A);
    break;
  }
  case 0x35: { // DCR M
    uint16_t HL = (H << 8) | L;
    uint8_t val = bus.Read(HL);
    bus.Write(HL, --val);
    UpdateFlagsZSP(val);
    SetFlag(FLAG_AC, (val & 0x0F) != 0x0F);
    break;
  }
  case 0x3A: { // LDA adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    A = bus.Read(addr);
    break;
  }
  case 0x3C: { // INR A
    uint8_t old = A;
    A++;
    UpdateFlagsZSP(A);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x3D: { // DCR A
    A--;
    UpdateFlagsZSP(A);
    SetFlag(FLAG_AC, (A & 0x0F) != 0x0F);
    break;
  }
  case 0x3E: { // MVI A,D8
    uint8_t byte = bus.Read(PC++);
    A = byte;
    break;
  }
  case 0x66: { // MOV H,M
    uint16_t HL = (H << 8) | L;
    H = bus.Read(HL);
    break;
  }
  case 0x67: { // MOV H,A
    H = A;
    break;
  }
  case 0x6F: { // MOV L,A
    L = A;
    break;
  }
  case 0x7E: { // MOV A,M
    uint16_t HL = (H << 8) | L;
    A = bus.Read(HL);
    break;
  }
  case 0xA7: { // ANA A
    UpdateFlags(A, false, A & 0x08);
    break;
  }
  case 0xAF: { // XRA A
    A = 0x00;
    UpdateFlags(A, false, false);
    break;
  }
  case 0xC1: { // POP B
    C = bus.Read(SP++);
    B = bus.Read(SP++);
    break;
  }
  case 0xC2: { // JNZ adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t Z = !!(F & FLAG_Z);
    PC = (!Z * addr) + (Z * PC);
    break;
  }
  case 0xC3: { // JMP adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    PC = addr;
    break;
  }
  case 0xC5: { // PUSH B
    bus.Write(--SP, B);
    bus.Write(--SP, C);
    break;
  }
  case 0xC6: { // ADI D8
    uint8_t byte = bus.Read(PC++);
    uint8_t res = A + byte;
    UpdateFlags(res, res < A, ((A ^ byte ^ res) & 0x10));
    A = res;
    break;
  }
  case 0xC9: { // RET
    uint8_t lo = bus.Read(SP++);
    uint8_t hi = bus.Read(SP++);
    PC = (hi << 8) | lo;
    break;
  }
  case 0xCA: { // JZ adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t Z = !!(F & FLAG_Z);
    PC = (Z * addr) + (!Z * PC);
    break;
  }
  case 0xCD: { // CALL adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = addr;
    break;
  }
  case 0xD1: { // POP D
    E = bus.Read(SP++);
    D = bus.Read(SP++);
    break;
  }
  case 0xD2: { // JNC adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t NCY = !(F & FLAG_CY);
    PC = (NCY * addr) + (!NCY * PC);
    break;
  }
  case 0xD5: { // PUSH D
    bus.Write(--SP, D);
    bus.Write(--SP, E);
    break;
  }
  case 0xDA: { // JC adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t CY = !!(F & FLAG_CY);
    PC = (CY * addr) + (!CY * PC);
    break;
  }
  case 0xDB: { // IN D8 (special)
    break;
  }
  case 0xE1: { // POP H
    L = bus.Read(SP++);
    H = bus.Read(SP++);
    break;
  }
  case 0xE2: { // JPO adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t PO = !(F & FLAG_P);
    PC = (PO * addr) + (!PO * PC);
    break;
  }
  case 0xE5: { // PUSH H
    bus.Write(--SP, H);
    bus.Write(--SP, L);
    break;
  }
  case 0xE6: { // ANI D8
    uint8_t byte = bus.Read(PC++);
    uint8_t res = A & byte;
    UpdateFlags(res, false, (A | byte) & 0x08);
    A = res;
    break;
  }
  case 0xEA: { // JPE adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t PE = !!(F & FLAG_P);
    PC = (PE * addr) + (!PE * PC);
    break;
  }
  case 0xEB: { // XCHG
    std::swap(H, D);
    std::swap(L, E);
    break;
  }
  case 0xF1: { // POP PSW
    F = bus.Read(SP++);
    A = bus.Read(SP++);
    break;
  }
  case 0xF2: { // JP adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t S = !(F & FLAG_S);
    PC = (S * addr) + (!S * PC);
    break;
  }
  case 0xF5: { // PUSH PSW
    bus.Write(--SP, A);
    bus.Write(--SP, F);
    break;
  }
  case 0xFA: { // JM adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    uint8_t NS = !!(F & FLAG_S);
    PC = (NS * addr) + (!NS * PC);
    break;
  }
  case 0xFB: { // EI (special)
    break;
  }
  case 0xFE: { // CPI D8
    uint8_t byte = bus.Read(PC++);
    uint16_t res = A - byte;
    UpdateFlags(res & 0xFF, A < byte, ((A ^ byte ^ res) & 0x10));
    break;
  }
  }
}
