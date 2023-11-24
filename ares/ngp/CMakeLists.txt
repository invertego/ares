ares_components(
  tlcs900h
  z80
  t6w28
)

target_sources(ares PRIVATE
  system/system.cpp
  cartridge/cartridge.cpp
  cpu/cpu.cpp
  apu/apu.cpp
  kge/kge.cpp
  psg/psg.cpp
)
