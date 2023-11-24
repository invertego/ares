ares_components(
  sm5k
)

target_sources(ares PRIVATE
  memory/memory.cpp
  system/system.cpp
  cartridge/cartridge.cpp
  cic/cic.cpp
  controller/controller.cpp
  dd/dd.cpp
  mi/mi.cpp
  vi/vi.cpp
  ai/ai.cpp
  pi/pi.cpp
  pif/pif.cpp
  ri/ri.cpp
  si/si.cpp
  rdram/rdram.cpp
  cpu/cpu.cpp
  rsp/rsp.cpp
  rdp/rdp.cpp
)

if(vulkan)
  if(platform STREQUAL macos)
#    molten = $(ares.path)/../thirdparty/MoltenVK/libMoltenVK.dylib
#    ifeq ($(wildcard $(molten)),)  
#      molten = $(shell brew --prefix molten-vk)/lib/libMoltenVK.dylib
#      ifeq ($(wildcard $(molten)),)
#        $(error Compiling Ares N64 Vulkan backend requires MoltenVK. Install it via Homebrew, compile it using thirdparty/MoltenVK/build-moltenvk.sh, or disable with vulkan=false")
#      endif
#    endif
#    ares.dylibs += $(molten)
  endif()

  target_sources(ares PRIVATE
    vulkan/vulkan.cpp
  )
 
  target_sources(ares PRIVATE
#    $(wildcard vulkan/parallel-rdp/parallel-rdp/*.cpp)
    vulkan/parallel-rdp/parallel-rdp/command_ring.cpp
    vulkan/parallel-rdp/parallel-rdp/rdp_device.cpp
    vulkan/parallel-rdp/parallel-rdp/rdp_dump_write.cpp
    vulkan/parallel-rdp/parallel-rdp/rdp_renderer.cpp
    vulkan/parallel-rdp/parallel-rdp/video_interface.cpp
    vulkan/parallel-rdp/vulkan/buffer.cpp
    vulkan/parallel-rdp/vulkan/buffer_pool.cpp
    vulkan/parallel-rdp/vulkan/command_buffer.cpp
    vulkan/parallel-rdp/vulkan/command_pool.cpp
    vulkan/parallel-rdp/vulkan/context.cpp
    vulkan/parallel-rdp/vulkan/cookie.cpp
    vulkan/parallel-rdp/vulkan/descriptor_set.cpp
    vulkan/parallel-rdp/vulkan/device.cpp
    vulkan/parallel-rdp/vulkan/event_manager.cpp
    vulkan/parallel-rdp/vulkan/fence.cpp
    vulkan/parallel-rdp/vulkan/fence_manager.cpp
    vulkan/parallel-rdp/vulkan/image.cpp
    vulkan/parallel-rdp/vulkan/indirect_layout.cpp
    vulkan/parallel-rdp/vulkan/memory_allocator.cpp
    vulkan/parallel-rdp/vulkan/pipeline_event.cpp
    vulkan/parallel-rdp/vulkan/query_pool.cpp
    vulkan/parallel-rdp/vulkan/render_pass.cpp
    vulkan/parallel-rdp/vulkan/sampler.cpp
    vulkan/parallel-rdp/vulkan/semaphore.cpp
    vulkan/parallel-rdp/vulkan/semaphore_manager.cpp
    vulkan/parallel-rdp/vulkan/shader.cpp
    vulkan/parallel-rdp/vulkan/texture/texture_format.cpp
    vulkan/parallel-rdp/util/arena_allocator.cpp
    vulkan/parallel-rdp/util/logging.cpp
    vulkan/parallel-rdp/util/thread_id.cpp
    vulkan/parallel-rdp/util/aligned_alloc.cpp
    vulkan/parallel-rdp/util/timer.cpp
    vulkan/parallel-rdp/util/timeline_trace_file.cpp
    vulkan/parallel-rdp/util/thread_name.cpp

    vulkan/parallel-rdp/volk/volk.c
  )

  target_include_directories(ares PRIVATE
    vulkan/parallel-rdp/parallel-rdp
    vulkan/parallel-rdp/volk
    vulkan/parallel-rdp/vulkan
    vulkan/parallel-rdp/vulkan-headers/include
    vulkan/parallel-rdp/util
  )

#  PARALLEL_RDP_LDFLAGS := -pthread
  if(platform STREQUAL windows)
    target_compile_definitions(ares PRIVATE VK_USE_PLATFORM_WIN32_KHR)
#    PARALLEL_RDP_LDFLAGS += -lwinmm
  else()
#    PARALLEL_RDP_LDFLAGS += -ldl
  endif()

else()
  message(WARNING "Ares n64 core requires Vulkan for RDP emulation, only titles using Software Rendering will function")
endif()
