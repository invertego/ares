ares_components(
  m68hc05
)

target_sources(ares PRIVATE
  memory/memory.cpp
  system/system.cpp
  disc/disc.cpp
  cpu/cpu.cpp
  gpu/gpu.cpp
  spu/spu.cpp
  mdec/mdec.cpp
  interrupt/interrupt.cpp
  peripheral/peripheral.cpp
  dma/dma.cpp
  timer/timer.cpp
)
