ares_components(
  z80
  tms9918
  sn76489
  i8255
)

target_sources(ares PRIVATE
  cpu/cpu.cpp
  vdp/vdp.cpp
  psg/psg.cpp
  ppi/ppi.cpp
  system/system.cpp
  cartridge/cartridge.cpp
  controller/controller.cpp
)
