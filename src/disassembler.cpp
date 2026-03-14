#include "disassembler.hpp"
#include <format>
#include <iostream>

int Disassemble8080Op(const std::array<uint8_t, 0x10000> &code_buffer,
                      size_t pc) {
  uint8_t code = code_buffer[pc];
  uint8_t opbytes = 1;

  std::cout << std::format("{:04x} ", pc);

  switch (code) {
  case 0x00:
    std::cout << "NOP";
    break;
  case 0x01:
    std::cout << std::format("LXI  B,${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x02:
    std::cout << "STAX B";
    break;
  case 0x03:
    std::cout << "INX  B";
    break;
  case 0x04:
    std::cout << "INR  B";
    break;
  case 0x05:
    std::cout << "DCR  B";
    break;
  case 0x06:
    std::cout << std::format("MVI  B,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x07:
    std::cout << "RLC";
    break;
  case 0x08:
    std::cout << "NOP";
    break;
  case 0x09:
    std::cout << "DAD  B";
    break;
  case 0x0A:
    std::cout << "LDAX B";
    break;
  case 0x0B:
    std::cout << "DCX  B";
    break;
  case 0x0C:
    std::cout << "INR  C";
    break;
  case 0x0D:
    std::cout << "DCR  C";
    break;
  case 0x0E:
    std::cout << std::format("MVI  C,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x0F:
    std::cout << "RRC";
    break;
  case 0x10:
    std::cout << "NOP";
    break;
  case 0x11:
    std::cout << std::format("LXI  D,${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x12:
    std::cout << "STAX D";
    break;
  case 0x13:
    std::cout << "INX  D";
    break;
  case 0x14:
    std::cout << "INR  D";
    break;
  case 0x15:
    std::cout << "DCR  D";
    break;
  case 0x16:
    std::cout << std::format("MVI  D,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x17:
    std::cout << "RAL";
    break;
  case 0x18:
    std::cout << "NOP";
    break;
  case 0x19:
    std::cout << "DAD  D";
    break;
  case 0x1A:
    std::cout << "LDAX D";
    break;
  case 0x1B:
    std::cout << "DCX  D";
    break;
  case 0x1C:
    std::cout << "INR  E";
    break;
  case 0x1D:
    std::cout << "DCR  E";
    break;
  case 0x1E:
    std::cout << std::format("MVI  E,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x1F:
    std::cout << "RAR";
    break;
  case 0x20:
    std::cout << "RIM";
    break;
  case 0x21:
    std::cout << std::format("LXI  H,${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x22:
    std::cout << std::format("SHLD ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x23:
    std::cout << "INX  H";
    break;
  case 0x24:
    std::cout << "INR  H";
    break;
  case 0x25:
    std::cout << "DCR  H";
    break;
  case 0x26:
    std::cout << std::format("MVI  H,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x27:
    std::cout << "DAA";
    break;
  case 0x28:
    std::cout << "NOP";
    break;
  case 0x29:
    std::cout << "DAD  H";
    break;
  case 0x2A:
    std::cout << std::format("LHLD ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x2B:
    std::cout << "DCX  H";
    break;
  case 0x2C:
    std::cout << "INR  L";
    break;
  case 0x2D:
    std::cout << "DCR  L";
    break;
  case 0x2E:
    std::cout << std::format("MVI  L,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x2F:
    std::cout << "CMA";
    break;
  case 0x30:
    std::cout << "SIM"; // Also an 8085 instruction
    break;
  case 0x31:
    std::cout << std::format("LXI  SP,${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x32:
    std::cout << std::format("STA  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x33:
    std::cout << "INX  SP";
    break;
  case 0x34:
    std::cout << "INR  M";
    break;
  case 0x35:
    std::cout << "DCR  M";
    break;
  case 0x36:
    std::cout << std::format("MVI  M,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x37:
    std::cout << "STC";
    break;
  case 0x38:
    std::cout << "NOP";
    break;
  case 0x39:
    std::cout << "DAD  SP";
    break;
  case 0x3A:
    std::cout << std::format("LDA  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x3B:
    std::cout << "DCX  SP";
    break;
  case 0x3C:
    std::cout << "INR  A";
    break;
  case 0x3D:
    std::cout << "DCR  A";
    break;
  case 0x3E:
    std::cout << std::format("MVI  A,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x3F:
    std::cout << "CMC";
    break;
  case 0x40:
    std::cout << "MOV  B,B";
    break;
  case 0x41:
    std::cout << "MOV  B,C";
    break;
  case 0x42:
    std::cout << "MOV  B,D";
    break;
  case 0x43:
    std::cout << "MOV  B,E";
    break;
  case 0x44:
    std::cout << "MOV  B,H";
    break;
  case 0x45:
    std::cout << "MOV  B,L";
    break;
  case 0x46:
    std::cout << "MOV  B,M";
    break;
  case 0x47:
    std::cout << "MOV  B,A";
    break;
  case 0x48:
    std::cout << "MOV  C,B";
    break;
  case 0x49:
    std::cout << "MOV  C,C";
    break;
  case 0x4A:
    std::cout << "MOV  C,D";
    break;
  case 0x4B:
    std::cout << "MOV  C,E";
    break;
  case 0x4C:
    std::cout << "MOV  C,H";
    break;
  case 0x4D:
    std::cout << "MOV  C,L";
    break;
  case 0x4E:
    std::cout << "MOV  C,M";
    break;
  case 0x4F:
    std::cout << "MOV  C,A";
    break;
  case 0x50:
    std::cout << "MOV  D,B";
    break;
  case 0x51:
    std::cout << "MOV  D,C";
    break;
  case 0x52:
    std::cout << "MOV  D,D";
    break;
  case 0x53:
    std::cout << "MOV  D,E";
    break;
  case 0x54:
    std::cout << "MOV  D,H";
    break;
  case 0x55:
    std::cout << "MOV  D,L";
    break;
  case 0x56:
    std::cout << "MOV  D,M";
    break;
  case 0x57:
    std::cout << "MOV  D,A";
    break;
  case 0x58:
    std::cout << "MOV  E,B";
    break;
  case 0x59:
    std::cout << "MOV  E,C";
    break;
  case 0x5A:
    std::cout << "MOV  E,D";
    break;
  case 0x5B:
    std::cout << "MOV  E,E";
    break;
  case 0x5C:
    std::cout << "MOV  E,H";
    break;
  case 0x5D:
    std::cout << "MOV  E,L";
    break;
  case 0x5E:
    std::cout << "MOV  E,M";
    break;
  case 0x5F:
    std::cout << "MOV  E,A";
    break;
  case 0x60:
    std::cout << "MOV  H,B";
    break;
  case 0x61:
    std::cout << "MOV  H,C";
    break;
  case 0x62:
    std::cout << "MOV  H,D";
    break;
  case 0x63:
    std::cout << "MOV  H,E";
    break;
  case 0x64:
    std::cout << "MOV  H,H";
    break;
  case 0x65:
    std::cout << "MOV  H,L";
    break;
  case 0x66:
    std::cout << "MOV  H,M";
    break;
  case 0x67:
    std::cout << "MOV  H,A";
    break;
  case 0x68:
    std::cout << "MOV  L,B";
    break;
  case 0x69:
    std::cout << "MOV  L,C";
    break;
  case 0x6A:
    std::cout << "MOV  L,D";
    break;
  case 0x6B:
    std::cout << "MOV  L,E";
    break;
  case 0x6C:
    std::cout << "MOV  L,H";
    break;
  case 0x6D:
    std::cout << "MOV  L,L";
    break;
  case 0x6E:
    std::cout << "MOV  L,M";
    break;
  case 0x6F:
    std::cout << "MOV  L,A";
    break;
  case 0x70:
    std::cout << "MOV  M,B";
    break;
  case 0x71:
    std::cout << "MOV  M,C";
    break;
  case 0x72:
    std::cout << "MOV  M,D";
    break;
  case 0x73:
    std::cout << "MOV  M,E";
    break;
  case 0x74:
    std::cout << "MOV  M,H";
    break;
  case 0x75:
    std::cout << "MOV  M,L";
    break;
  case 0x76:
    std::cout << "HLT";
    break;
  case 0x77:
    std::cout << "MOV  M,A";
    break;
  case 0x78:
    std::cout << "MOV  A,B";
    break;
  case 0x79:
    std::cout << "MOV  A,C";
    break;
  case 0x7A:
    std::cout << "MOV  A,D";
    break;
  case 0x7B:
    std::cout << "MOV  A,E";
    break;
  case 0x7C:
    std::cout << "MOV  A,H";
    break;
  case 0x7D:
    std::cout << "MOV  A,L";
    break;
  case 0x7E:
    std::cout << "MOV  A,M";
    break;
  case 0x7F:
    std::cout << "MOV  A,A";
    break;
  case 0x80:
    std::cout << "ADD  B";
    break;
  case 0x81:
    std::cout << "ADD  C";
    break;
  case 0x82:
    std::cout << "ADD  D";
    break;
  case 0x83:
    std::cout << "ADD  E";
    break;
  case 0x84:
    std::cout << "ADD  H";
    break;
  case 0x85:
    std::cout << "ADD  L";
    break;
  case 0x86:
    std::cout << "ADD  M";
    break;
  case 0x87:
    std::cout << "ADD  A";
    break;
  case 0x88:
    std::cout << "ADC  B";
    break;
  case 0x89:
    std::cout << "ADC  C";
    break;
  case 0x8A:
    std::cout << "ADC  D";
    break;
  case 0x8B:
    std::cout << "ADC  E";
    break;
  case 0x8C:
    std::cout << "ADC  H";
    break;
  case 0x8D:
    std::cout << "ADC  L";
    break;
  case 0x8E:
    std::cout << "ADC  M";
    break;
  case 0x8F:
    std::cout << "ADC  A";
    break;
  case 0x90:
    std::cout << "SUB  B";
    break;
  case 0x91:
    std::cout << "SUB  C";
    break;
  case 0x92:
    std::cout << "SUB  D";
    break;
  case 0x93:
    std::cout << "SUB  E";
    break;
  case 0x94:
    std::cout << "SUB  H";
    break;
  case 0x95:
    std::cout << "SUB  L";
    break;
  case 0x96:
    std::cout << "SUB  M";
    break;
  case 0x97:
    std::cout << "SUB  A";
    break;
  case 0x98:
    std::cout << "SBB  B";
    break;
  case 0x99:
    std::cout << "SBB  C";
    break;
  case 0x9A:
    std::cout << "SBB  D";
    break;
  case 0x9B:
    std::cout << "SBB  E";
    break;
  case 0x9C:
    std::cout << "SBB  H";
    break;
  case 0x9D:
    std::cout << "SBB  L";
    break;
  case 0x9E:
    std::cout << "SBB  M";
    break;
  case 0x9F:
    std::cout << "SBB  A";
    break;
  case 0xA0:
    std::cout << "ANA  B";
    break;
  case 0xA1:
    std::cout << "ANA  C";
    break;
  case 0xA2:
    std::cout << "ANA  D";
    break;
  case 0xA3:
    std::cout << "ANA  E";
    break;
  case 0xA4:
    std::cout << "ANA  H";
    break;
  case 0xA5:
    std::cout << "ANA  L";
    break;
  case 0xA6:
    std::cout << "ANA  M";
    break;
  case 0xA7:
    std::cout << "ANA A";
    break;
  case 0xA8:
    std::cout << "XRA  B";
    break;
  case 0xA9:
    std::cout << "XRA  C";
    break;
  case 0xAA:
    std::cout << "XRA  D";
    break;
  case 0xAB:
    std::cout << "XRA  E";
    break;
  case 0xAC:
    std::cout << "XRA  H";
    break;
  case 0xAD:
    std::cout << "XRA  L";
    break;
  case 0xAE:
    std::cout << "XRA  M";
    break;
  case 0xAF:
    std::cout << "XRA  A";
    break;
  case 0xB0:
    std::cout << "ORA  B";
    break;
  case 0xB1:
    std::cout << "ORA  C";
    break;
  case 0xB2:
    std::cout << "ORA  D";
    break;
  case 0xB3:
    std::cout << "ORA  E";
    break;
  case 0xB4:
    std::cout << "ORA  H";
    break;
  case 0xB5:
    std::cout << "ORA  L";
    break;
  case 0xB6:
    std::cout << "ORA  M";
    break;
  case 0xB7:
    std::cout << "ORA  A";
    break;
  case 0xB8:
    std::cout << "CMP  B";
    break;
  case 0xB9:
    std::cout << "CMP  C";
    break;
  case 0xBA:
    std::cout << "CMP  D";
    break;
  case 0xBB:
    std::cout << "CMP  E";
    break;
  case 0xBC:
    std::cout << "CMP  H";
    break;
  case 0xBD:
    std::cout << "CMP  L";
    break;
  case 0xBE:
    std::cout << "CMP  M";
    break;
  case 0xBF:
    std::cout << "CMP  A";
    break;
  case 0xC0:
    std::cout << "RNZ";
    break;
  case 0xC1:
    std::cout << "POP  B";
    break;
  case 0xC2:
    std::cout << std::format("JNZ  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xC3:
    std::cout << std::format("JMP  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xC4:
    std::cout << std::format("CNZ  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xC5:
    std::cout << "PUSH B";
    break;
  case 0xC6:
    std::cout << std::format("ADI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xC7:
    std::cout << "RST  0";
    break;
  case 0xC8:
    std::cout << "RZ";
    break;
  case 0xC9:
    std::cout << "RET";
    break;
  case 0xCA:
    std::cout << std::format("JZ   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xCB:
    std::cout << "NOOP";
    break;
  case 0xCC:
    std::cout << std::format("CZ   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xCD:
    std::cout << std::format("CALL  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xCE:
    std::cout << std::format("ACI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xCF:
    std::cout << "RST  1";
    break;
  case 0xD0:
    std::cout << "RNC";
    break;
  case 0xD1:
    std::cout << "POP D";
    break;
  case 0xD2:
    std::cout << std::format("JNC  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xD3:
    std::cout << std::format("OUT  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xD4:
    std::cout << std::format("CNC  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xD5:
    std::cout << "PUSH D";
    break;
  case 0xD6:
    std::cout << std::format("SUI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xD7:
    std::cout << "RST  2";
    break;
  case 0xD8:
    std::cout << "RC";
    break;
  case 0xD9:
    std::cout << "NOOP";
    break;
  case 0xDA:
    std::cout << std::format("JC   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xDB:
    std::cout << std::format("IN   {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xDC:
    std::cout << std::format("CC   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xDD:
    std::cout << "NOOP";
    break;
  case 0xDE:
    std::cout << std::format("SBI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xDF:
    std::cout << "RST  3";
    break;
  case 0xE0:
    std::cout << "RPO";
    break;
  case 0xE1:
    std::cout << "POP  H";
    break;
  case 0xE2:
    std::cout << std::format("JPO  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xE3:
    std::cout << "XTHL";
    break;
  case 0xE4:
    std::cout << std::format("CPO  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xE5:
    std::cout << "PUSH H";
    break;
  case 0xE6:
    std::cout << std::format("ANI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xE7:
    std::cout << "RST  4";
    break;
  case 0xE8:
    std::cout << "RPE";
    break;
  case 0xE9:
    std::cout << "PCHL";
    break;
  case 0xEA:
    std::cout << std::format("JPE  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
  case 0xEB:
    std::cout << "XCHG";
    break;
  case 0xEC:
    std::cout << std::format("CPE  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xED:
    std::cout << "NOOP";
    break;
  case 0xEE:
    std::cout << std::format("XRI  {:02x}", code_buffer[pc + 1]);
    break;
  case 0xEF:
    std::cout << "RST  5";
    break;
  case 0xF0:
    std::cout << "RP";
    break;
  case 0xF1:
    std::cout << "POP PSW";
    break;
  case 0xF2:
    std::cout << std::format("JP   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xF3:
    std::cout << "DI";
    break;
  case 0xF4:
    std::cout << std::format("CP   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xF5:
    std::cout << "PUSH PSW";
    break;
  case 0xF6:
    std::cout << std::format("ORI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xF7:
    std::cout << "RST  6";
    break;
  case 0xF8:
    std::cout << "RM";
    break;
  case 0xF9:
    std::cout << "SPHL";
    break;
  case 0xFA:
    std::cout << std::format("JM   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xFB:
    std::cout << "EI";
    break;
  case 0xFC:
    std::cout << std::format("CM   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xFD:
    std::cout << "NOOP";
    break;
  case 0xFE:
    std::cout << std::format("CPI  {:02x}", code_buffer[pc + 1]);
    break;
  case 0xFF:
    std::cout << "RST  7";
    break;
  default:
    std::cout << std::format("UNKNOWN OPCODE: 0x{:02x}", code) << std::endl;
    exit(1);
  }

  std::cout << std::endl;

  return opbytes;
}
