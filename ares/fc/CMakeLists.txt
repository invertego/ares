ares_components(
  mos6502
  ym2149
  ym2413
  m24c
  sst39sf0x0
)

target_sources(ares PRIVATE
  system/system.cpp
  controller/controller.cpp
  expansion/expansion.cpp
  cartridge/cartridge.cpp
  cpu/cpu.cpp
  apu/apu.cpp
  ppu/ppu.cpp
  fds/fds.cpp
)
