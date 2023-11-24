ares_components(
  z80
  sn76489
  ym2413
)

target_sources(ares PRIVATE
  cpu/cpu.cpp
  vdp/vdp.cpp
  psg/psg.cpp
  opll/opll.cpp
  system/system.cpp
  cartridge/cartridge.cpp
  controller/controller.cpp
  expansion/expansion.cpp
)
