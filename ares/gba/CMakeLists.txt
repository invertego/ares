ares_components(
  arm7tdmi
)

target_sources(ares PRIVATE
  memory/memory.cpp
  system/system.cpp
  cartridge/cartridge.cpp
  player/player.cpp
  cpu/cpu.cpp
  ppu/ppu.cpp
  apu/apu.cpp
)
