ares_components(
  wdc65816
  spc700
  arm7tdmi
  gsu
  hg51b
  upd96050
)

target_sources(ares PRIVATE
  system/system.cpp
  controller/controller.cpp
  cartridge/cartridge.cpp
  memory/memory.cpp

  cpu/cpu.cpp
  smp/smp.cpp
  dsp/dsp.cpp
  ppu/ppu.cpp
  ppu-performance/ppu.cpp

  coprocessor/coprocessor.cpp
  expansion/expansion.cpp
  slot/slot.cpp
)
