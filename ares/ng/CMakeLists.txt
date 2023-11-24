ares_components(
  m68000
  z80
)

target_sources(ares PRIVATE
  system/system.cpp
  cpu/cpu.cpp
  apu/apu.cpp
  lspc/lspc.cpp
  opnb/opnb.cpp
  cartridge/cartridge.cpp
  controller/controller.cpp
  card/card.cpp
)
