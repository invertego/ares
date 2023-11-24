ares_components(
  m68000
  z80
  sh2
  ssp1601
  sn76489
  ym2612
  m24c
)

target_sources(ares PRIVATE
  bus/bus.cpp
  cpu/cpu.cpp
  apu/apu.cpp
  vdp/vdp.cpp
  opn2/opn2.cpp
  m32x/m32x.cpp
  mcd/mcd.cpp
  system/system.cpp
  cartridge/cartridge.cpp
  controller/controller.cpp
)
