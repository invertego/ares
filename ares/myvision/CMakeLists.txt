ares_components(
  z80
  tms9918
  ay38910
)

target_sources(ares PRIVATE
  cpu/cpu.cpp
  vdp/vdp.cpp
  psg/psg.cpp
  system/system.cpp
  cartridge/cartridge.cpp
)
