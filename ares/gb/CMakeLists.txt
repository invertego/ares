ares_components(
  sm83
  m93lcx6
)

target_sources(ares PRIVATE
  system/system.cpp
  cartridge/cartridge.cpp
  bus/bus.cpp
  cpu/cpu.cpp
  ppu/ppu.cpp
  apu/apu.cpp
)
