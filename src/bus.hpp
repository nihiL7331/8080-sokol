#ifndef BUS_HPP
#define BUS_HPP

#include "audio.hpp"
#include <array>
#include <cstdint>
#include <vector>

class Bus {
private:
  // 64KB, maps to all possible memory addresses that the 8080 can access.
  // It's better than separte arrays for ROM, RAM, VRAM because its O(1) lookup
  // + it's easier/cleaner
  std::array<uint8_t, 0x10000> memory{};
  uint16_t sh_reg = 0x0000;
  uint8_t sh_off = 0x00;
  uint8_t p1 = 0x00;
  uint8_t p2 = 0x00;
  uint8_t lp3 = 0x00;
  uint8_t lp5 = 0x00;

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
    case 3: {
      uint8_t ris_edge = val & ~lp3;
      if (ris_edge & 0x01)
        PlaySound(&sfx_ufo, true);
      else if (ris_edge & 0x02)
        PlaySound(&sfx_shoot);
      else if (ris_edge & 0x04)
        PlaySound(&sfx_player_die);
      else if (ris_edge & 0x08)
        PlaySound(&sfx_invader_die);
      else if (ris_edge & 0x10)
        PlaySound(&sfx_extra_ship);

      if ((lp3 & 0x01) && !(val & 0x01))
        StopSound(&sfx_ufo);
      lp3 = val;
      break;
    }
    case 5: {
      uint8_t ris_edge = val & ~lp5;
      if (ris_edge & 0x01)
        PlaySound(&sfx_fleet_1);
      else if (ris_edge & 0x02)
        PlaySound(&sfx_fleet_2);
      else if (ris_edge & 0x04)
        PlaySound(&sfx_fleet_3);
      else if (ris_edge & 0x08)
        PlaySound(&sfx_fleet_4);
      else if (ris_edge & 0x10)
        PlaySound(&sfx_ufo_hit);
      lp5 = val;
      break;
    }
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
