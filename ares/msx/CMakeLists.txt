ares_components(
  z80
  tms9918
  v9938
  ay38910
  ym2413
)

target_sources(ares PRIVATE
  system/system.cpp
  keyboard/keyboard.cpp
  cartridge/cartridge.cpp
  controller/controller.cpp
  cpu/cpu.cpp
  vdp/vdp.cpp
  psg/psg.cpp
  tape/tape.cpp
  rtc/rtc.cpp
)
