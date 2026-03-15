# (Yet another) 8080/Space Invaders emulator written in C++.

## Usage

The code can be compiled via **CMake**.

```bash
cmake -B build
cmake --build build
./build/emu8080 [--log]
```

Enable disassembler trace using the `--log` flag.

## Functionality

- Working audio
- 1:1 CPU cycles
- Passes the CPUDIAG (with proper memory setup)
- 2P supported

## Controls

- **C**: Coin
- **1**: P1 Start
- **2**: P2 Start
- **Space**: Fire
- **Left Arrow**: Left
- **Right Arrow**: Right

## Dependencies

Uses **Sokol** for rendering and audio.

## Platforms

Tested with **macOS** and **Linux**.

---

<sub>Written in 3 days for learning purposes.</sub>
