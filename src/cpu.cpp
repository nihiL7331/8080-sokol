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
  case 0x09: { // DAD B
    uint16_t HL = (H << 8) | L;
    uint16_t BC = (B << 8) | C;
    HL += BC;
    H = HL >> 8;
    L = HL & 0xFF;
    SetFlag(FLAG_CY, HL >> 8);
    break;
  }
  case 0x0A: { // LDAX B
    uint16_t BC = (B << 8) | L;
    A = bus.Read(BC);
    break;
  }
  case 0x0B: { // DCX B
    uint16_t BC = (B << 8) | L;
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
    uint16_t DE = (D << 8) | L;
    bus.Write(DE, A);
    break;
  }
  case 0x13: { // INX D
    uint16_t DE = (D << 8) | L;
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
    HL += DE;
    SetFlag(FLAG_CY, HL >> 8);
    break;
  }
  case 0x1A: { // LDAX D
    uint16_t DE = (D << 8) | E;
    A = bus.Read(DE);
    break;
  }
  case 0x1B: { // DCX D
    uint16_t DE = (D << 8) | L;
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
    uint8_t prev = A & 0x01;
    A = A >> 1;
    A |= (A & 0x40) << 1;
    SetFlag(FLAG_CY, prev);
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
    uint8_t val = A & 0xF;
    if (val > 0x09) {
      val += 0x06;

      SetFlag(FLAG_AC, val & 0x10);
    }
    val += (A & 0xF0);
    if ((val & 0xF0) > 0x90) {
      val += 0x60;
    }
    SetFlag(FLAG_CY, val > 0xFF);
    break;
  }
  case 0x29: { // DAD H
    uint16_t HL = (H << 8) | L;
    HL += HL;
    H = HL >> 8;
    L = HL & 0xFF;
    SetFlag(FLAG_CY, HL >> 8);
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
    HL += SP;
    H = HL >> 8;
    L = HL & 0xFF;
    SetFlag(FLAG_CY, HL >> 8);
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
  }
  case 0x5F: { // MOV E,A
    E = A;
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
  case 0x89: { // ADC C
    uint8_t CY = !!(F & FLAG_CY);
    uint16_t res = A + C + CY;
    UpdateFlags(res & 0xFF, res > 0xFF, (A ^ C ^ res) & 0x10);
    A = res & 0xFF;
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
  case 0xC0: { // RNZ
    if (!(F & FLAG_Z)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
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
  case 0xD0: { // RNC
    if (!(F & FLAG_CY)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
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
    UpdateFlags(res, A < byte, (A ^ byte ^ res) & 0x10);
    A = res;
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
    // TODO:
    A = bus.InPort(bus.Read(PC++));
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
    UpdateFlags(res, A < sub, (A ^ byte ^ res) & 0x10);
    A = res & 0xFF;
    break;
  }
  case 0xE0: { // RPO
    if (!(F & FLAG_P)) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
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
  case 0xE8: { // RPE
    if (F & FLAG_P) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
    }
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
    }
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
  case 0xF8: { // RM
    if (F & FLAG_S) {
      uint8_t lo = bus.Read(SP++);
      uint8_t hi = bus.Read(SP++);
      PC = (hi << 8) | lo;
    }
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
    UpdateFlags(res & 0xFF, A < byte, (A ^ byte ^ res) & 0x10);
    break;
  }
  }
}
