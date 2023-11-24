ares_components(
  v30mz
  m93lcx6
)

target_sources(ares PRIVATE
  system/system.cpp
  memory/memory.cpp
  eeprom/eeprom.cpp
  cartridge/cartridge.cpp
  cpu/cpu.cpp
  ppu/ppu.cpp
  apu/apu.cpp
  serial/serial.cpp
)
