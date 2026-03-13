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
    std::cout << std::format("LXI   B,${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0x02:
    std::cout << "STAX  B";
    break;
  case 0x03:
    std::cout << "INX   B";
    break;
  case 0x04:
    std::cout << "INR   B";
    break;
  case 0x05:
    std::cout << "DCR   B";
    break;
  case 0x06:
    std::cout << std::format("MVI   B,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x07:
    std::cout << "RLC";
    break;
  case 0x08:
    std::cout << "NOP";
    break;
  case 0x0E:
    std::cout << std::format("MVI C,${:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0x0F:
    std::cout << "RRC";
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
  case 0x27:
    std::cout << "DAA";
    break;
  case 0x2B:
    std::cout << "DCX  H";
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
  case 0x35:
    std::cout << "DCR  M";
    break;
  case 0x3A:
    std::cout << std::format("LDA  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
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
  case 0x5F:
    std::cout << "MOV  E,A";
    break;
  case 0x66:
    std::cout << "MOV  H,M";
    break;
  case 0x67:
    std::cout << "MOV  H,A";
    break;
  case 0x6F:
    std::cout << "MOV  L,A";
    break;
  case 0x7C:
    std::cout << "MOV A,H";
    break;
  case 0x7D:
    std::cout << "MOV A,L";
    break;
  case 0x7E:
    std::cout << "MOV  A,M";
    break;
  case 0x89:
    std::cout << "ADC  C";
    break;
  case 0xA7:
    std::cout << "ANA  A";
    break;
  case 0xAF:
    std::cout << "XRA  A";
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
  case 0xCC:
    std::cout << std::format("CZ   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xCD:
    std::cout << std::format("CALL ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xCE:
    std::cout << std::format("ACI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xD0:
    std::cout << "RNC";
    break;
  case 0xD1:
    std::cout << "POP  D";
    break;
  case 0xD2:
    std::cout << std::format("JNC  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
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
  case 0xD8:
    std::cout << "RC";
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
  case 0xDE:
    std::cout << std::format("SBI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
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
    opbytes = 2;
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
  case 0xE8:
    std::cout << "RPE";
    break;
  case 0xEA:
    std::cout << std::format("JPE  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xEB:
    std::cout << "XCHG";
    break;
  case 0xEC:
    std::cout << std::format("CPE  ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
    break;
  case 0xEE:
    std::cout << std::format("XRI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  case 0xEF:
    std::cout << "RST 5";
    break;
  case 0xF0:
    std::cout << "RP";
    break;
  case 0xF1:
    std::cout << "POP  PSW";
    break;
  case 0xF2:
    std::cout << std::format("JP   ${:02x}{:02x}", code_buffer[pc + 2],
                             code_buffer[pc + 1]);
    opbytes = 3;
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
  case 0xF8:
    std::cout << "RM";
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
  case 0xFE:
    std::cout << std::format("CPI  {:02x}", code_buffer[pc + 1]);
    opbytes = 2;
    break;
  default:
    std::cout << std::format("UNKNOWN OPCODE: 0x{:02x}", code) << std::endl;
    exit(1);
  }

  std::cout << std::endl;

  return opbytes;
}
