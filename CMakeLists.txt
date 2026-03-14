cmake_minimum_required(VERSION 3.20)

project(8080Sokol LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(emu8080
  src/main_emu.cpp
  src/disassembler.cpp
  src/cpu.cpp
  src/sokol.cpp
)

# add_executable(disassembler
#   src/main_disasm.cpp
#   src/disassembler.cpp
# )

target_include_directories(emu8080 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor)
# target_include_directories(disassembler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

if(APPLE)
  enable_language(OBJCXX)
  set_source_files_properties(src/sokol.cpp PROPERTIES LANGUAGE OBJCXX)

  target_link_libraries(emu8080 PRIVATE
    "-framework Cocoa"
    "-framework QuartzCore"
    "-framework OpenGL"
    "-framework AudioToolbox"
  )
elseif(UNIX AND NOT APPLE)
  target_link_libraries(emu8080 PRIVATE
    GL
    dl
    m
    pthread
    X11
    Xi
    Xcursor
    Xrandr
    asound
  )
elseif(WIN32)
  target_link_libraries(emu8080 PRIVATE
    kernel32
    user32
    shell32
    gdi32
    d3d11
    dxgi
  )
endif()
