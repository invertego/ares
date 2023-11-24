ares_components(
  huc6280
  msm5205
)

target_sources(ares PRIVATE
  cpu/cpu.cpp
  vdp/vdp.cpp
  vdp-performance/vdp.cpp
  psg/psg.cpp
  pcd/pcd.cpp
  system/system.cpp
  cartridge/cartridge.cpp
  controller/controller.cpp
)
