#ifndef BUS_HPP
#define BUS_HPP

#include <array>
#include <cstdint>
#include <vector>

class Bus {
private:
  // 64KB, maps to all possible memory addresses that the 8080 can access.
  // It's better than separte arrays for ROM, RAM, VRAM because its O(1) lookup
  // + it's easier/cleaner
  std::array<uint8_t, 0x10000> memory{};
  uint16_t sh_reg = 0;
  uint8_t sh_off = 0;
  uint8_t p1 = 0x00;
  uint8_t p2 = 0x00;

public:
  Bus() = default;

  uint8_t Read(uint16_t addr) const {
    // RAM mirror handle - redirect to RAM
    if (addr >= 0x2000 && addr < 0x6000) {
      addr = 0x2000 | (addr & 0x1FFF);
    }

    return memory[addr];
  }

  void Write(uint16_t addr, uint8_t data) {
    // ROM is read-only, so we skip writing to it
    if (addr < 0x2000)
      return;

    // RAM mirror handle - if in 0x4000 - 0x5FFF range,
    // move back to RAM area
    if (addr < 0x6000) {
      addr = 0x2000 | (addr & 0x1FFF);
    }

    // Write to RAM/VRAM
    memory[addr] = data;
  }

  void LoadROM(const std::vector<uint8_t> &rom_data, uint16_t start_addr) {
    if (start_addr + rom_data.size() > memory.size())
      return;

    std::copy(rom_data.begin(), rom_data.end(), memory.begin() + start_addr);
  }

  uint8_t InPort(uint8_t port) {
    switch (port) {
    case 1:
      return p1;
      break;
    case 2:
      return p2;
      break;
    case 3: { // READ
      return (sh_reg >> (8 - sh_off)) & 0xFF;
      break;
    }
    }
    return 0;
  }

  void OutPort(uint8_t port, uint8_t val) {
    switch (port) {
    case 2: { // SET SH OFF
      sh_off = val & 0x07;
      break;
    }
    case 4: { // SH DATA TO REG
      sh_reg = (sh_reg >> 8) | (val << 8);
      break;
    }
    case 3:
      break;
    case 5:
      break;
    }
  }

  void SetPort(uint8_t port, uint8_t val) {
    switch (port) {
    case 1:
      p1 = val;
      break;
    case 2:
      p2 = val;
      break;
    }
  }

  // Cleaner memory getter for disassembler
  const std::array<uint8_t, 0x10000> &GetMem() const { return memory; }
};

#endif
