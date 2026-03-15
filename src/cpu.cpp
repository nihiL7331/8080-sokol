#include "cpu.hpp"
#include "disassembler.hpp"
#include <array>
#include <utility>

static const std::array<uint8_t, 256> OP_CYCLES = {
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 0x00
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 0x10
    4, 10, 16, 5,  5,  5,  7,  4,  4, 10, 16, 5,  5,  5,  7, 4,  // 0x20
    4, 10, 13, 5,  10, 10, 10, 4,  4, 10, 13, 5,  5,  5,  7, 4,  // 0x30
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 0x40
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 0x50
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 0x60
    7, 7,  7,  7,  7,  7,  7,  7,  5, 5,  5,  5,  5,  5,  7, 5,  // 0x70
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 0x80
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 0x90
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 0xA0
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 0xB0
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // 0xC0
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // 0xD0
    5, 10, 10, 18, 11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11, // 0xE0
    5, 10, 10, 4,  11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11  // 0xF0
};

int CPU::Step() {
  if (log)
    Disassemble8080Op(bus.GetMem(), PC);
  uint8_t opcode = bus.Read(PC);
  int cycles = OP_CYCLES[opcode];

  PC++;

  // REG PAIRS
  // HIGH / LOW
  //  B   /  C
  //  D   /  E
  //  H   /  L
  //  A   /  F (PSW)
  switch (opcode) {
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
  case 0x09: { // DAD B
    uint16_t HL = (H << 8) | L;
    uint16_t BC = (B << 8) | C;
    uint32_t res = HL + BC;
    H = res >> 8;
    L = res & 0xFF;
    SetFlag(FLAG_CY, res > 0xFFFF);
    break;
  }
  case 0x0A: { // LDAX B
    uint16_t BC = (B << 8) | C;
    A = bus.Read(BC);
    break;
  }
  case 0x0B: { // DCX B
    uint16_t BC = (B << 8) | C;
    BC--;
    B = BC >> 8;
    C = BC & 0xFF;
    break;
  }
  case 0x0C: { // INR C
    uint8_t old = C;
    C++;
    UpdateFlagsZSP(C);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x0D: { // DCR C
    C--;
    UpdateFlagsZSP(C);
    SetFlag(FLAG_AC, (C & 0x0F) != 0x0F);
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
  case 0x11: { // LXI D,D16
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    D = hi;
    E = lo;
    break;
  }
  case 0x12: { // STAX D
    uint16_t DE = (D << 8) | E;
    bus.Write(DE, A);
    break;
  }
  case 0x13: { // INX D
    uint16_t DE = (D << 8) | E;
    DE++;
    D = DE >> 8;
    E = DE & 0xFF;
    break;
  }
  case 0x14: { // INR D
    uint8_t old = D;
    D++;
    UpdateFlagsZSP(D);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x15: { // DCR D
    D--;
    UpdateFlagsZSP(D);
    SetFlag(FLAG_AC, (D & 0x0F) != 0x0F);
    break;
  }
  case 0x16: { // MVI D, D8
    uint8_t byte = bus.Read(PC++);
    D = byte;
    break;
  }
  case 0x17: { // RAL
    uint8_t prev = (A >> 7);
    A = A << 1;
    A |= !!(F & FLAG_CY);
    SetFlag(FLAG_CY, prev);
    break;
  }
  case 0x19: { // DAD D
    uint16_t HL = (H << 8) | L;
    uint16_t DE = (D << 8) | E;
    uint32_t res = HL + DE;
    H = res >> 8;
    L = res & 0xFF;
    SetFlag(FLAG_CY, res > 0xFFFF);
    break;
  }
  case 0x1A: { // LDAX D
    uint16_t DE = (D << 8) | E;
    A = bus.Read(DE);
    break;
  }
  case 0x1B: { // DCX D
    uint16_t DE = (D << 8) | E;
    DE--;
    D = DE >> 8;
    E = DE & 0xFF;
    break;
  }
  case 0x1C: { // INR E
    uint8_t old = E;
    E++;
    UpdateFlagsZSP(E);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x1D: { // DCR E
    E--;
    UpdateFlagsZSP(E);
    SetFlag(FLAG_AC, (E & 0x0F) != 0x0F);
    break;
  }
  case 0x1E: { // MVI E,D8
    uint8_t byte = bus.Read(PC++);
    E = byte;
    break;
  }
  case 0x1F: { // RAR
    uint8_t old_b = !!(F & FLAG_CY);
    uint8_t new_b = A & 0x01;
    A = (A >> 1) | (old_b << 7);
    SetFlag(FLAG_CY, new_b);
    break;
  }
  case 0x20: // RIM (NOOP)
    break;
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
  case 0x24: { // INR H
    uint8_t old = H;
    H++;
    UpdateFlagsZSP(H);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x25: { // DCR H
    H--;
    UpdateFlagsZSP(H);
    SetFlag(FLAG_AC, (H & 0x0F) != 0x0F);
    break;
  }
  case 0x26: { // MVI H,D8
    uint8_t byte = bus.Read(PC++);
    H = byte;
    break;
  }
  case 0x27: { // DAA (special)
    uint8_t add = 0;
    uint8_t carry = !!(F & FLAG_CY);
    if ((A & 0x0F) > 0x09 || (F & FLAG_AC))
      add += 0x06;
    uint8_t hi = A >> 4;
    if (hi > 0x09 || carry || (hi >= 0x09 && (A & 0x0F) > 0x09)) {
      add += 0x60;
      carry = 1;
    }
    uint16_t res = A + add;
    UpdateFlagsZSP(res & 0xFF);
    SetFlag(FLAG_CY, carry);
    SetFlag(FLAG_AC, (A ^ add ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x29: { // DAD H
    uint16_t HL = (H << 8) | L;
    uint32_t res = HL + HL;
    H = res >> 8;
    L = res & 0xFF;
    SetFlag(FLAG_CY, res > 0xFFFF);
    break;
  }
  case 0x2A: { // LHLD adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    L = bus.Read(addr++);
    H = bus.Read(addr);
    break;
  }
  case 0x2B: { // DCX H
    uint16_t HL = (H << 8) | L;
    HL--;
    H = HL >> 8;
    L = HL & 0xFF;
    break;
  }
  case 0x2C: { // INR L
    uint8_t old = L;
    L++;
    UpdateFlagsZSP(L);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
    break;
  }
  case 0x2D: { // DCR L
    L--;
    UpdateFlagsZSP(L);
    SetFlag(FLAG_AC, (L & 0x0F) != 0x0F);
    break;
  }
  case 0x2E: { // MVI L, D8
    uint8_t byte = bus.Read(PC++);
    L = byte;
    break;
  }
  case 0x2F: { // CMA
    A = ~A;
    break;
  }
  case 0x30: { // SIM (special)
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
  case 0x33: { // INX SP
    SP++;
    break;
  }
  case 0x34: { // INR M
    uint16_t HL = (H << 8) | L;
    uint8_t val = bus.Read(HL);
    uint8_t old = val;
    bus.Write(HL, ++val);
    UpdateFlagsZSP(val);
    SetFlag(FLAG_AC, (old & 0x0F) == 0x0F);
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
  case 0x36: { // MVI M,D8
    uint8_t byte = bus.Read(PC++);
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, byte);
    break;
  }
  case 0x37: { // STC
    SetFlag(FLAG_CY, true);
    break;
  }
  case 0x39: { // DAD SP
    uint16_t HL = (H << 8) | L;
    uint32_t res = HL + SP;
    H = res >> 8;
    L = res & 0xFF;
    SetFlag(FLAG_CY, res > 0xFFFF);
    break;
  }
  case 0x3A: { // LDA adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    A = bus.Read(addr);
    break;
  }
  case 0x3B: { // DCX SP
    SP--;
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
  case 0x3F: { // CMC
    SetFlag(FLAG_CY, !(F & FLAG_CY));
    break;
  }
  case 0x40: { // MOV B,B
    break;
  }
  case 0x41: { // MOV B,C
    B = C;
    break;
  }
  case 0x42: { // MOV B,D
    B = D;
    break;
  }
  case 0x43: { // MOV B,E
    B = E;
    break;
  }
  case 0x44: { // MOV B,H
    B = H;
    break;
  }
  case 0x45: { // MOV B,L
    B = L;
    break;
  }
  case 0x46: { // MOV B,M
    uint16_t HL = (H << 8) | L;
    B = bus.Read(HL);
    break;
  }
  case 0x47: { // MOV B,A
    B = A;
    break;
  }
  case 0x48: { // MOV C,B
    C = B;
    break;
  }
  case 0x49: { // MOV C,C
    break;
  }
  case 0x4A: { // MOV C,D
    C = D;
    break;
  }
  case 0x4B: { // MOV C,E
    C = E;
    break;
  }
  case 0x4C: { // MOV C,H
    C = H;
    break;
  }
  case 0x4D: { // MOV C,L
    C = L;
    break;
  }
  case 0x4E: { // MOV C,M
    uint16_t HL = (H << 8) | L;
    C = bus.Read(HL);
    break;
  }
  case 0x4F: { // MOV C,A
    C = A;
    break;
  }
  case 0x50: { // MOV D,B
    D = B;
    break;
  }
  case 0x51: { // MOV D,C
    D = C;
    break;
  }
  case 0x52: { // MOV D,D
    break;
  }
  case 0x53: { // MOV D,E
    D = E;
    break;
  }
  case 0x54: { // MOV D,H
    D = H;
    break;
  }
  case 0x55: { // MOV D,L
    D = L;
    break;
  }
  case 0x56: { // MOV D,M
    uint16_t HL = (H << 8) | L;
    D = bus.Read(HL);
    break;
  }
  case 0x57: { // MOV D,A
    D = A;
    break;
  }
  case 0x58: { // MOV E,B
    E = B;
    break;
  }
  case 0x59: { // MOV E,C
    E = C;
    break;
  }
  case 0x5A: { // MOV E,D
    E = D;
    break;
  }
  case 0x5B: { // MOV E,E
    break;
  }
  case 0x5C: { // MOV E,H
    E = H;
    break;
  }
  case 0x5D: { // MOV E,L
    E = L;
    break;
  }
  case 0x5E: { // MOV E,M
    uint16_t HL = (H << 8) | L;
    E = bus.Read(HL);
    break;
  }
  case 0x5F: { // MOV E,A
    E = A;
    break;
  }
  case 0x60: { // MOV H,B
    H = B;
    break;
  }
  case 0x61: { // MOV H,C
    H = C;
    break;
  }
  case 0x62: { // MOV H,D
    H = D;
    break;
  }
  case 0x63: { // MOV H,E
    H = E;
    break;
  }
  case 0x64: { // MOV H,H
    break;
  }
  case 0x65: { // MOV H,L
    H = L;
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
  case 0x68: { // MOV L,B
    L = B;
    break;
  }
  case 0x69: { // MOV L,C
    L = C;
    break;
  }
  case 0x6A: { // MOV L,D
    L = D;
    break;
  }
  case 0x6B: { // MOV L,E
    L = E;
    break;
  }
  case 0x6C: { // MOV L,H
    L = H;
    break;
  }
  case 0x6D: { // MOV L,L
    break;
  }
  case 0x6E: { // MOV L,M
    uint16_t HL = (H << 8) | L;
    L = bus.Read(HL);
    break;
  }
  case 0x6F: { // MOV L,A
    L = A;
    break;
  }
  case 0x70: { // MOV M,B
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, B);
    break;
  }
  case 0x71: { // MOV M,C
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, C);
    break;
  }
  case 0x72: { // MOV M,D
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, D);
    break;
  }
  case 0x73: { // MOV M,E
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, E);
    break;
  }
  case 0x74: { // MOV M,H
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, H);
    break;
  }
  case 0x75: { // MOV M,L
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, L);
    break;
  }
  case 0x76: { // HLT (special)
    break;
  }
  case 0x77: { // MOV M,A
    uint16_t HL = (H << 8) | L;
    bus.Write(HL, A);
    break;
  }
  case 0x78: { // MOV A,B
    A = B;
    break;
  }
  case 0x79: { // MOV A,C
    A = C;
    break;
  }
  case 0x7A: { // MOV A,D
    A = D;
    break;
  }
  case 0x7B: { // MOV A,E
    A = E;
    break;
  }
  case 0x7C: { // MOV A,H
    A = H;
    break;
  }
  case 0x7D: { // MOV A,L
    A = L;
    break;
  }
  case 0x7E: { // MOV A,M
    uint16_t HL = (H << 8) | L;
    A = bus.Read(HL);
    break;
  }
  case 0x7F: { // MOV A,A
    break;
  }
  case 0x80: { // ADD B
    uint16_t res = A + B;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ B ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x81: { // ADD C
    uint16_t res = A + C;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ C ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x82: { // ADD D
    uint16_t res = A + D;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ D ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x83: { // ADD E
    uint16_t res = A + E;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ E ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x84: { // ADD H
    uint16_t res = A + H;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ H ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x85: { // ADD L
    uint16_t res = A + L;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ L ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x86: { // ADD M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint16_t res = A + M;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ M ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x87: { // ADD A
    uint16_t res = A + A;
    UpdateFlags(res & 0xFF, res > 0xFF, res & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x88: { // ADC B
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + B + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ B ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x89: { // ADC C
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + C + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ C ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x8A: { // ADC D
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + D + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ D ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x8B: { // ADC E
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + E + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ E ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x8C: { // ADC H
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + H + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ H ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x8D: { // ADC L
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + L + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ L ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x8E: { // ADC M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + M + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ M ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x8F: { // ADC A
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + A + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, res & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x90: { // SUB B
    uint8_t res = A - B;
    // 8080 flips the subtracted value on AC flag check
    UpdateFlags(res, B > A, (A ^ (~B) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x91: { // SUB C
    uint8_t res = A - C;
    UpdateFlags(res, C > A, (A ^ (~C) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x92: { // SUB D
    uint8_t res = A - D;
    UpdateFlags(res, D > A, (A ^ (~D) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x93: { // SUB E
    uint8_t res = A - E;
    UpdateFlags(res, E > A, (A ^ (~E) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x94: { // SUB H
    uint8_t res = A - H;
    UpdateFlags(res, H > A, (A ^ (~H) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x95: { // SUB L
    uint8_t res = A - L;
    UpdateFlags(res, L > A, (A ^ (~L) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x96: { // SUB M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t res = A - M;
    UpdateFlags(res, M > A, (A ^ (~M) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0x97: { // SUB A
    UpdateFlags(0x00, false, true);
    A = 0x00;
    break;
  }
  case 0x98: { // SBB B
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = B + CY;
    uint8_t res = A - sub;
    // 8080 flips the subtracted value on AC flag check
    UpdateFlags(res, A < sub, (A ^ (~B) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x99: { // SBB C
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = C + CY;
    uint8_t res = A - sub;
    UpdateFlags(res, A < sub, (A ^ (~C) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x9A: { // SBB D
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = D + CY;
    uint8_t res = A - sub;
    UpdateFlags(res, A < sub, (A ^ (~D) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x9B: { // SBB E
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = E + CY;
    uint8_t res = A - sub;
    UpdateFlags(res, A < sub, (A ^ (~E) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x9C: { // SBB H
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = H + CY;
    uint8_t res = A - sub;
    UpdateFlags(res, A < sub, (A ^ (~H) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x9D: { // SBB L
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = L + CY;
    uint8_t res = A - sub;
    UpdateFlags(res, A < sub, (A ^ (~L) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x9E: { // SBB M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = M + CY;
    uint8_t res = A - sub;
    // 8080 flips the subtracted value on AC flag check
    UpdateFlags(res, A < sub, (A ^ (~M) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0x9F: { // SBB A
    uint8_t CY = !!(F & FLAG_CY);
    UpdateFlags(-CY, CY, !CY);
    A = -CY;
    break;
  }
  case 0xA0: { // ANA B
    uint8_t res = A & B;
    // AC is bitwise OR of bit 3 of the inputs on 8080
    UpdateFlags(res, false, (A | B) & 0x08);
    A = res;
    break;
  }
  case 0xA1: { // ANA C
    uint8_t res = A & C;
    UpdateFlags(res, false, (A | C) & 0x08);
    A = res;
    break;
  }
  case 0xA2: { // ANA D
    uint8_t res = A & D;
    UpdateFlags(res, false, (A | D) & 0x08);
    A = res;
    break;
  }
  case 0xA3: { // ANA E
    uint8_t res = A & E;
    UpdateFlags(res, false, (A | E) & 0x08);
    A = res;
    break;
  }
  case 0xA4: { // ANA H
    uint8_t res = A & H;
    UpdateFlags(res, false, (A | H) & 0x08);
    A = res;
    break;
  }
  case 0xA5: { // ANA L
    uint8_t res = A & L;
    UpdateFlags(res, false, (A | L) & 0x08);
    A = res;
    break;
  }
  case 0xA6: { // ANA M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t res = A & M;
    UpdateFlags(res, false, (A | M) & 0x08);
    A = res;
    break;
  }
  case 0xA7: { // ANA A
    UpdateFlags(A, false, A & 0x08);
    break;
  }
  case 0xA8: { // XRA B
    uint8_t res = A ^ B;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xA9: { // XRA C
    uint8_t res = A ^ C;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xAA: { // XRA D
    uint8_t res = A ^ D;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xAB: { // XRA E
    uint8_t res = A ^ E;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xAC: { // XRA H
    uint8_t res = A ^ H;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xAD: { // XRA L
    uint8_t res = A ^ L;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xAE: { // XRA M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t res = A ^ M;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xAF: { // XRA A
    UpdateFlags(0x00, false, false);
    A = 0x00;
    break;
  }
  case 0xB0: { // ORA B
    uint8_t res = A | B;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB1: { // ORA C
    uint8_t res = A | C;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB2: { // ORA D
    uint8_t res = A | D;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB3: { // ORA E
    uint8_t res = A | E;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB4: { // ORA H
    uint8_t res = A | H;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB5: { // ORA L
    uint8_t res = A | L;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB6: { // ORA M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t res = A | M;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xB7: { // ORA A
    UpdateFlags(A, false, false);
    break;
  }
  case 0xB8: { // CMP B
    uint8_t res = A - B;
    // 8080 flips the subtracted value on AC flag check
    UpdateFlags(res, B > A, (A ^ (~B) ^ res) & 0x10);
    break;
  }
  case 0xB9: { // CMP C
    uint8_t res = A - C;
    UpdateFlags(res, C > A, (A ^ (~C) ^ res) & 0x10);
    break;
  }
  case 0xBA: { // CMP D
    uint8_t res = A - D;
    UpdateFlags(res, D > A, (A ^ (~D) ^ res) & 0x10);
    break;
  }
  case 0xBB: { // CMP E
    uint8_t res = A - E;
    UpdateFlags(res, E > A, (A ^ (~E) ^ res) & 0x10);
    break;
  }
  case 0xBC: { // CMP H
    uint8_t res = A - H;
    UpdateFlags(res, H > A, (A ^ (~H) ^ res) & 0x10);
    break;
  }
  case 0xBD: { // CMP L
    uint8_t res = A - L;
    UpdateFlags(res, L > A, (A ^ (~L) ^ res) & 0x10);
    break;
  }
  case 0xBE: { // CMP M
    uint16_t HL = (H << 8) | L;
    uint8_t M = bus.Read(HL);
    uint8_t res = A - M;
    UpdateFlags(res, M > A, (A ^ (~M) ^ res) & 0x10);
    break;
  }
  case 0xBF: { // CMP A
    UpdateFlags(0x00, false, true);
    break;
  }
  case 0xC0: { // RNZ
    if (!(F & FLAG_Z)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
      cycles += 6;
    }
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
  case 0xC4: { // CNZ adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (!(F & FLAG_Z)) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
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
    UpdateFlags(res, res < A, (A ^ byte ^ res) & 0x10);
    A = res;
    break;
  }
  case 0xC7: { // RST 0
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0000;
    break;
  }
  case 0xC8: { // RZ
    if (F & FLAG_Z) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
    }
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
  case 0xCC: { // CZ adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (F & FLAG_Z) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
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
  case 0xCE: { // ACI D8
    uint8_t byte = bus.Read(PC++);
    uint16_t res = A + byte + !!(F & FLAG_CY);
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ byte ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0xCF: { // RST 1
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0008;
    break;
  }
  case 0xD0: { // RNC
    if (!(F & FLAG_CY)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
      cycles += 6;
    }
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
  case 0xD3: { // OUT D8
    uint8_t port = bus.Read(PC++);
    bus.OutPort(port, A);
    break;
  }
  case 0xD4: { // CNC adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (!(F & FLAG_CY)) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
    break;
  }
  case 0xD5: { // PUSH D
    bus.Write(--SP, D);
    bus.Write(--SP, E);
    break;
  }
  case 0xD6: { // SUI D8
    uint8_t byte = bus.Read(PC++);
    uint8_t res = A - byte;
    // 8080 flips the subtracted value on AC flag check
    UpdateFlags(res, A < byte, (A ^ (~byte) ^ res) & 0x10);
    A = res;
    break;
  }
  case 0xD7: { // RST 2
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0010;
    break;
  }
  case 0xD8: { // RC
    if (F & FLAG_CY) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
    }
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
    uint8_t port = bus.Read(PC++);
    A = bus.InPort(port);
    break;
  }
  case 0xDC: { // CC adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (F & FLAG_CY) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
    break;
  }
  case 0xDE: { // SBI D8
    uint8_t byte = bus.Read(PC++);
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t sub = byte + CY;
    uint16_t res = A - sub;
    // 8080 flips the subtracted value on AC flag check
    UpdateFlags(res, A < sub, (A ^ (~byte) ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0xDF: { // RST 3
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0018;
    break;
  }
  case 0xE0: { // RPO
    if (!(F & FLAG_P)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
      cycles += 6;
    }
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
  case 0xE3: { // XTHL
    uint8_t lo = bus.Read(SP);
    uint8_t hi = bus.Read(SP + 1);
    bus.Write(SP, L);
    bus.Write(SP + 1, H);
    L = lo;
    H = hi;
    break;
  }
  case 0xE4: { // CPO adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (!(F & FLAG_P)) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
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
  case 0xE7: { // RST 4
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0020;
    break;
  }
  case 0xE8: { // RPE
    if (F & FLAG_P) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
    }
    break;
  }
  case 0xE9: { // PCHL
    uint16_t HL = (H << 8) | L;
    PC = HL;
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
  case 0xEC: { // CPE adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (F & FLAG_P) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
    break;
  }
  case 0xEE: { // XRI D8
    uint8_t byte = bus.Read(PC++);
    uint8_t res = A ^ byte;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xEF: { // RST 5
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0028;
    break;
  }
  case 0xF0: { // RP
    if (!(F & FLAG_S)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
      cycles += 6;
    }
    break;
  }
  case 0xF1: { // POP PSW
    F = bus.Read(SP++);
    A = bus.Read(SP++);
    F = (F & 0xD7) | 0x02;
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
  case 0xF3: { // DI (special)
    I = false;
    break;
  }
  case 0xF4: { // CP adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (!(F & FLAG_S)) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
    break;
  }
  case 0xF5: { // PUSH PSW
    bus.Write(--SP, A);
    bus.Write(--SP, F);
    break;
  }
  case 0xF6: { // ORI D8
    uint8_t byte = bus.Read(PC++);
    uint8_t res = A | byte;
    UpdateFlags(res, false, false);
    A = res;
    break;
  }
  case 0xF7: { // RST 6
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0030;
    break;
  }
  case 0xF8: { // RM
    if (F & FLAG_S) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
    }
    break;
  }
  case 0xF9: { // SPHL
    uint16_t HL = (H << 8) | L;
    SP = HL;
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
    I = true;
    break;
  }
  case 0xFC: { // CM adr
    uint8_t lo = bus.Read(PC++);
    uint8_t hi = bus.Read(PC++);
    uint16_t addr = (hi << 8) | lo;
    if (F & FLAG_S) {
      uint8_t pclo = PC & 0xFF;
      uint8_t pchi = PC >> 8;
      bus.Write(--SP, pchi);
      bus.Write(--SP, pclo);
      PC = addr;
    }
    break;
  }
  case 0xFE: { // CPI D8
    uint8_t byte = bus.Read(PC++);
    uint16_t res = A - byte;
    UpdateFlags(res & 0xFF, A < byte, (A ^ (~byte) ^ res) & 0x10);
    break;
  }
  case 0xFF: { // RST 7
    uint8_t pclo = PC & 0xFF;
    uint8_t pchi = PC >> 8;
    bus.Write(--SP, pchi);
    bus.Write(--SP, pclo);
    PC = 0x0038;
    break;
  }
  }

  return cycles;
}
