#include "bus.hpp"
#include "cpu.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static Bus bus;
static CPU cpu(bus);

std::vector<uint8_t> ReadROMFile(const std::string &path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open ROM: " + path);
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buffer(size);
  if (file.read(reinterpret_cast<char *>(buffer.data()), size)) {
    return buffer;
  }

  return {};
}

void InitializeSystem(std::string path) {
  try {
    std::vector<uint8_t> rom = ReadROMFile(path);

    bus.LoadROM(rom, 0x0100);
  } catch (const std::exception &e) {
    std::cerr << "Initialization error: " << e.what() << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2)
    return -1;

  InitializeSystem(argv[1]);

  // CPUDIAG setup
  cpu.SetPC(0x0100);
  bus.Write(0x0005, 0xC9);

  while (true) {
    if (cpu.GetPC() == 0x0000) {
      std::cout << "Reached 0x0000. Emulator stopped." << std::endl;
      break;
    }
    // BDOS
    if (cpu.GetPC() == 0x0005) {
      if (cpu.GetC() == 9) {
        uint16_t addr = (cpu.GetD() << 8) | cpu.GetE();
        while (bus.Read(addr) != '$')
          std::cout << (char)bus.Read(addr++);
      } else if (cpu.GetC() == 2)
        std::cout << (char)cpu.GetE();

      uint16_t ret_addr =
          bus.Read(cpu.GetSP()) | (bus.Read(cpu.GetSP() + 1) << 8);
      cpu.SetSP(cpu.GetSP() + 2);
      cpu.SetPC(ret_addr);

      continue;
    }

    cpu.Step();
  }
}
