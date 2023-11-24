ares_components(
  z80
  ay38910
)

target_sources(ares PRIVATE
  keyboard/keyboard.cpp
  expansion/expansion.cpp
  system/system.cpp
  cpu/cpu.cpp
  ula/ula.cpp
  psg/psg.cpp
  tape/tape.cpp
)
