ares_components(
  mos6502
)

target_sources(ares PRIVATE
  system/system.cpp
  controller/controller.cpp
  cartridge/cartridge.cpp
  cpu/cpu.cpp
  tia/tia.cpp
  riot/riot.cpp
)
